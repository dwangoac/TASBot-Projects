use std::io::{Read as IoRead,Write as IoWrite};
use std::io::{stdin,stdout};
use std::cmp::min;

//Assumed to be at least 257.
const SPEED: usize = 300;
const INFRAME_SIZE: usize = 8;
const OUTFRAME_SIZE: usize = 16;
const VOFFSET_X: usize = 8;
const VOFFSET_Y: usize = 7;
const VSIZE_X: usize = 16;
const VSIZE_Y: usize = 14;

macro_rules! mask
{
	($input:expr,$inidx:expr,$inbit:expr,$outbit:expr,$inflip:expr) => {
		if ($input[$inidx] ^ $inflip) & (1u8<<$inbit) != 0 { 1u8<<$outbit } else { 0x00 };
	}
}

//8 bytes -> 16bytes.
fn trans_one_iframe(out: &mut [u8], input: &[u8])
{
	assert!(out.len() == OUTFRAME_SIZE);
	assert!(input.len() == INFRAME_SIZE);
	const INFLIP1: u8 = 0;
	const INFLIP2: u8 = 0xAC;
	for i in 0..4 { out[i] = 0; out[8+i] = 0; }
	for i in 0..8 {
		out[0] |= mask!(input, 6-(i&4)/2, 0+2*(3-i%4), 7-i, INFLIP1);
		out[1] |= mask!(input, 2-(i&4)/2, 0+2*(3-i%4), 7-i, INFLIP1);
		out[2] |= mask!(input, 6-(i&4)/2, 1+2*(3-i%4), 7-i, INFLIP1);
		out[3] |= mask!(input, 2-(i&4)/2, 1+2*(3-i%4), 7-i, INFLIP1);
		out[8] |= mask!(input, 7-(i&4)/2, 0+2*(3-i%4), 7-i, INFLIP2);
		out[9] |= mask!(input, 3-(i&4)/2, 0+2*(3-i%4), 7-i, INFLIP2);
		out[10] |= mask!(input, 7-(i&4)/2, 1+2*(3-i%4), 7-i, INFLIP2);
		out[11] |= mask!(input, 3-(i&4)/2, 1+2*(3-i%4), 7-i, INFLIP2);
	}
}

//4 rows of 8 bytes -> 16bytes.
pub fn trans_one_iframe_bitplane(out: &mut [u8], input: &[&[u8]], bplane: u32)
{
	//The asserts here eliminate any bounds checks from hot code.
	assert!(out.len() == OUTFRAME_SIZE);
	assert!(input.len() == 4);
	let input0 = input[0];
	let input1 = input[1];
	let input2 = input[2];
	let input3 = input[3];
	let bshift0 = bplane*2;
	let bshift1 = bplane*2+1;
	assert!(input0.len() == 8);
	assert!(input1.len() == 8);
	assert!(input2.len() == 8);
	assert!(input3.len() == 8);

	for i in 0..8 {
		//Fourth row, bits starting from 7 indexes 0 and 2 alternating.
		let bit = 7 - (i%8)/2;
		let byte = 2 - 2*(i%2);
		out[byte] |= ((input3[i] >> bshift0) & 1) << bit;
		out[byte+8] |= ((input3[i] >> bshift1) & 1) << bit;
		//Third row, bits starting from 3 indexes 0 and 2 alternating.
		let bit = 3 - (i%8)/2;
		let byte = 2 - 2*(i%2);
		out[byte] |= ((input2[i] >> bshift0) & 1) << bit;
		out[byte+8] |= ((input2[i] >> bshift1) & 1) << bit;
		//Second row, bits starting from 7 indexes 1 and 3 alternating.
		let bit = 7 - (i%8)/2;
		let byte = 3 - 2*(i%2);
		out[byte] |= ((input1[i] >> bshift0) & 1) << bit;
		out[byte+8] |= ((input1[i] >> bshift1) & 1) << bit;
		//First row, bits starting from 3 indexes 1 and 3 alternating.
		let bit = 3 - (i%8)/2;
		let byte = 3 - 2*(i%2);
		out[byte] |= ((input0[i] >> bshift0) & 1) << bit;
		out[byte+8] |= ((input0[i] >> bshift1) & 1) << bit;
	}
	//Do the bitflips in output domain.
	out[8] ^= 0x22;
	out[9] ^= 0x22;
	out[10] ^= 0xee;
	out[11] ^= 0xee;
}

//8 rows of 8 bytes -> 128bytes
#[inline(never)]
pub fn trans_one_tile(out: &mut [u8], input: &[&[u8]])
{
	assert!(out.len() == 8 * OUTFRAME_SIZE);
	assert!(input.len() == 8);
	//This is backwards, since the frames are read backwards.
	trans_one_iframe_bitplane(&mut out[7*OUTFRAME_SIZE..8*OUTFRAME_SIZE], &input[0..4], 0);
	trans_one_iframe_bitplane(&mut out[6*OUTFRAME_SIZE..7*OUTFRAME_SIZE], &input[4..8], 0);
	trans_one_iframe_bitplane(&mut out[5*OUTFRAME_SIZE..6*OUTFRAME_SIZE], &input[0..4], 1);
	trans_one_iframe_bitplane(&mut out[4*OUTFRAME_SIZE..5*OUTFRAME_SIZE], &input[4..8], 1);
	trans_one_iframe_bitplane(&mut out[3*OUTFRAME_SIZE..4*OUTFRAME_SIZE], &input[0..4], 2);
	trans_one_iframe_bitplane(&mut out[2*OUTFRAME_SIZE..3*OUTFRAME_SIZE], &input[4..8], 2);
	trans_one_iframe_bitplane(&mut out[1*OUTFRAME_SIZE..2*OUTFRAME_SIZE], &input[0..4], 3);
	trans_one_iframe_bitplane(&mut out[0*OUTFRAME_SIZE..1*OUTFRAME_SIZE], &input[4..8], 3);
}

//Digests 1560 iframes, outputs 1566 iframes.
//Low tiles are assumed to be at 634-828, high tiles at 829-1023.
fn trans_frame_image3(high: bool) -> bool
{
	const WIDTH: usize = VSIZE_X;
	const HEIGHT: usize = VSIZE_Y;
	const INPUT_FRAMES: usize = WIDTH * HEIGHT * 8;
	const VIDEO_FRAMES: usize = 6;
	const OUTPUT_FRAMES: usize = VIDEO_FRAMES * SPEED;
	//Check feasibility.
	assert!(INPUT_FRAMES + VIDEO_FRAMES <= VIDEO_FRAMES * SPEED);
	const TILE_SIZE: usize = 8*OUTFRAME_SIZE;
	let mut input = vec![0;INPUT_FRAMES*INFRAME_SIZE];
	let mut output = vec![0;OUTPUT_FRAMES*OUTFRAME_SIZE];
	//Read the input bitmap.
	let mut bufuse = 0;
	while bufuse < input.len() {
		let r = stdin().read(&mut input[bufuse..]).unwrap();
		if r == 0 { return false; }
		bufuse += r;
	}
	for i in 0..INPUT_FRAMES/8 {
		//The coordinates of the tile processed.
		let x = i % WIDTH;
		let y = i / WIDTH;
		let mut tmp = [0u8; TILE_SIZE];	//Space for tile data in SNES bitplane format.
		let width = 8 * WIDTH;
		let width8 = 8 * width;
		trans_one_tile(&mut tmp, &[
			&input[y*width8+x*8+0*width..][..8], &input[y*width8+x*8+1*width..][..8],
			&input[y*width8+x*8+2*width..][..8], &input[y*width8+x*8+3*width..][..8],
			&input[y*width8+x*8+4*width..][..8], &input[y*width8+x*8+5*width..][..8],
			&input[y*width8+x*8+6*width..][..8], &input[y*width8+x*8+7*width..][..8]]);
		//Copy the frames to correct places.
		for j in 0..8 {
			let sfnum = 8 * i + (7 - j);
			let fnum = sfnum / (SPEED - 1);
			let snum = sfnum % (SPEED - 1);
			let idx = fnum * SPEED + (SPEED - 2) - snum;
			(&mut output[idx*OUTFRAME_SIZE..][..OUTFRAME_SIZE]).copy_from_slice(
				&tmp[j*OUTFRAME_SIZE..][..OUTFRAME_SIZE]);
		}
	}
	//Write command headers.
	for i in 0..VIDEO_FRAMES {
		//Low tiles have base offset of 634*32=20288, stepping by 4*(SPEED-1).
		//High tiles have base offset of 829*32=26528, stepping by 4*(SPEED-1).
		let step = INFRAME_SIZE * (SPEED - 1) / 2;
		let sub = INPUT_FRAMES * INFRAME_SIZE / 2;
		let offset = step * i + (if high { 32768 - sub } else { 32768 - 2 * sub });
		let csize = (SPEED - 1) * INFRAME_SIZE;
		let csize = min(csize, 65536 - 2 * offset);
		let ntile = if high == (i == (VIDEO_FRAMES - 1)) { 4 } else { 0 };
		let cmd = [1, offset as u8, (offset >> 8) as u8, csize as u8, (csize >> 8) as u8, ntile, 0, 0];
		let fnum = (i+1)*SPEED-1;
		trans_one_iframe(&mut output[fnum*OUTFRAME_SIZE..][..OUTFRAME_SIZE], &cmd);
	}
	//Flush the entiere thing out.
	let mut ptr = 0;
	while ptr < output.len() {
		let r = stdout().write(&mut output[ptr..]).unwrap();
		ptr = ptr + r;
	}
	true
}

fn trans_palette() -> bool
{
	let mut input = [0;768];
	let mut output = [0;SPEED*16];
	let mut bufuse = 0;
	while bufuse < input.len() {
		let r = stdin().read(&mut input[bufuse..]).unwrap();
		if r == 0 { return false; }
		bufuse += r;
	}
	for i in 0..256 {
		let r = (input[3*i+0] >> 3) as u16;
		let g = (input[3*i+1] >> 3) as u16;
		let b = (input[3*i+2] >> 3) as u16;
		let c = 1024 * b + 32 * g + r;
		input[2*i+0] = c as u8;
		input[2*i+1] = (c >> 8) as u8;
	}
	for i in 0..64 {
		let o = (SPEED-2)-i;
		trans_one_iframe(&mut output[16*o..][..16], &input[8*i..][..8]);
	}
	//Load palette, load tilemap 0k, tiledata 0k.
	trans_one_iframe(&mut output[16*(SPEED-1)..][..16], &[2, 0, 0, 0, 0, 0, 0, 0]);
	let mut ptr = 0;
	while ptr < output.len() {
		let r = stdout().write(&mut output[ptr..]).unwrap();
		ptr = ptr + r;
	}
	return true;
}

fn trans_tilemap(high: bool) -> bool
{
	let voffset_x = VOFFSET_X;
	let voffset_y = VOFFSET_Y;
	let vsize_x = VSIZE_X;
	let vsize_y = VSIZE_Y;
	let mut output = [0;SPEED*16];
	//Load 1024 words at at VRAM offset 0 or 1024.
	trans_one_iframe(&mut output[16*(SPEED-1)..][..16], &[1, 0, if high { 4 } else { 0 }, 0, 8, 0, 0, 0]);
	//The tilemap data as 16-bit words. We assume tile 64 is is blank.
	let mut tmap_data = [64u16; 1024];
	//Fill the video area.
	let tilebase = 1024 - vsize_x * vsize_y * (if high { 1 } else { 2 });
	for x in 0..vsize_x {
		for y in 0..vsize_y {
			tmap_data[32*(voffset_y+y)+(voffset_x+x)] = (tilebase + vsize_x * y + x) as u16;
		}
	}
	//Borders for video area.
	for x in 0..vsize_x {
		tmap_data[32*(voffset_y-1)+(voffset_x+x)] = 65;			//Line bottom.
		tmap_data[32*(voffset_y+vsize_y)+(voffset_x+x)] = 66;		//Line top.
	}
	for y in 0..vsize_y {
		tmap_data[32*(voffset_y+y)+(voffset_x-1)] = 67;			//Line right.
		tmap_data[32*(voffset_y+y)+(voffset_x+vsize_x)] = 68;		//Line left.
	}
	tmap_data[32*(voffset_y-1)+(voffset_x-1)] = 69;				//Line bottom-right.
	tmap_data[32*(voffset_y-1)+(voffset_x+vsize_x)] = 70;			//Line bottom-left.
	tmap_data[32*(voffset_y+vsize_y)+(voffset_x-1)] = 71;			//Line top-right.
	tmap_data[32*(voffset_y+vsize_y)+(voffset_x+vsize_x)] = 72;		//Line top-left.
	//Translate tmap data into bytes.
	for i in 0..256 {
		let o = (SPEED-2) - i;
		let src = &tmap_data[4*i..4*i+4];
		trans_one_iframe(&mut output[16*o..][..16], &[
			src[0] as u8, (src[0] >> 8) as u8, src[1] as u8, (src[1] >> 8) as u8,
			src[2] as u8, (src[2] >> 8) as u8, src[3] as u8, (src[3] >> 8) as u8]);
	}
	//Write it out.
	let mut ptr = 0;
	while ptr < output.len() {
		let r = stdout().write(&mut output[ptr..]).unwrap();
		ptr = ptr + r;
	}
	return true;
}

fn trans_border_tiles() -> bool
{
	let mut output = [0;SPEED*16];
	//Load 512 bytes (for 8 tiles) at at VRAM offset 2080 (tile 65).
	trans_one_iframe(&mut output[16*(SPEED-1)..][..16], &[1, 32, 8, 0, 2, 0, 0, 0]);
	let brow = [  0,   0,   0,   0,   0,   0,   0,   0];
	let lrow = [255, 255,   0,   0,   0,   0,   0,   0];
	let rrow = [  0,   0,   0,   0,   0,   0, 255, 255];
	let frow = [255, 255, 255, 255, 255, 255, 255, 255];
	//Tile 65: Line bottom.
	trans_one_tile(&mut output[16*(SPEED-1-8*1)..][..128],
		&[&brow, &brow, &brow, &brow, &brow, &brow, &frow, &frow]);
	//Tile 66: Line top.
	trans_one_tile(&mut output[16*(SPEED-1-8*2)..][..128],
		&[&frow, &frow, &brow, &brow, &brow, &brow, &brow, &brow]);
	//Tile 67: Line right.
	trans_one_tile(&mut output[16*(SPEED-1-8*3)..][..128],
		&[&rrow, &rrow, &rrow, &rrow, &rrow, &rrow, &rrow, &rrow]);
	//Tile 68: Line left.
	trans_one_tile(&mut output[16*(SPEED-1-8*4)..][..128],
		&[&lrow, &lrow, &lrow, &lrow, &lrow, &lrow, &lrow, &lrow]);
	//Tile 69: Line bottom-right.
	trans_one_tile(&mut output[16*(SPEED-1-8*5)..][..128],
		&[&brow, &brow, &brow, &brow, &brow, &brow, &rrow, &rrow]);
	//Tile 70: Line bottom-right.
	trans_one_tile(&mut output[16*(SPEED-1-8*6)..][..128],
		&[&brow, &brow, &brow, &brow, &brow, &brow, &lrow, &lrow]);
	//Tile 71: Line top-right.
	trans_one_tile(&mut output[16*(SPEED-1-8*7)..][..128],
		&[&rrow, &rrow, &brow, &brow, &brow, &brow, &brow, &brow]);
	//Tile 72: Line top-right.
	trans_one_tile(&mut output[16*(SPEED-1-8*8)..][..128],
		&[&lrow, &lrow, &brow, &brow, &brow, &brow, &brow, &brow]);
	//Write it out.
	let mut ptr = 0;
	while ptr < output.len() {
		let r = stdout().write(&mut output[ptr..]).unwrap();
		ptr = ptr + r;
	}
	return true;
}

fn main()
{
	let mut highmem = true;		//Starts at himem.
	let mut count = 0usize;
	if !trans_palette() { return; }
	if !trans_border_tiles() { return; }
	if !trans_tilemap(false) { return; }
	if !trans_tilemap(true) { return; }
	loop {
		if !trans_frame_image3(highmem) { return; }
		highmem = !highmem;
		count = count + 1;
	}
}
