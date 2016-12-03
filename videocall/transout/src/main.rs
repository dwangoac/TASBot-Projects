use std::io::{Read as IoRead,Write as IoWrite};
use std::io::{stdin,stdout};

macro_rules! mask
{
	($input:expr,$inidx:expr,$inbit:expr,$outbit:expr,$inflip:expr) => {
		if ($input[$inidx] ^ $inflip) & (1u8<<$inbit) != 0 { 1u8<<$outbit } else { 0x00 };
	}
}

//8 bytes -> 16bytes.
fn trans_one_iframe(out: &mut [u8], input: &[u8])
{
	assert!(out.len() == 16);
	assert!(input.len() == 8);
	const INFLIP1: u8 = 0;
	const INFLIP2: u8 = 0xAC;
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
	assert!(out.len() == 16);
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

//8 rows of 8 bytes -> 128bytes.
#[inline(never)]
pub fn trans_one_tile(out: &mut [u8], input: &[&[u8]])
{
	assert!(out.len() == 128);
	assert!(input.len() == 8);
	//This is backwards, since the frames are read backwards.
	trans_one_iframe_bitplane(&mut out[112..128], &input[0..4], 0);
	trans_one_iframe_bitplane(&mut out[96..112], &input[4..8], 0);
	trans_one_iframe_bitplane(&mut out[80..96], &input[0..4], 1);
	trans_one_iframe_bitplane(&mut out[64..80], &input[4..8], 1);
	trans_one_iframe_bitplane(&mut out[48..64], &input[0..4], 2);
	trans_one_iframe_bitplane(&mut out[32..48], &input[4..8], 2);
	trans_one_iframe_bitplane(&mut out[16..32], &input[0..4], 3);
	trans_one_iframe_bitplane(&mut out[0..16], &input[4..8], 3);
}

//Digests 1456 iframes, outputs 1458 iframes.
fn trans_frame_image() -> bool
{
	const INPUT_FRAMES: usize = 1456;
	const OUTPUT_FRAMES: usize = 1458;
	const OUTPUT_VFRAMES: usize = 6;
	const OUTPUT_FRAMES_PER_VFRAME: usize = OUTPUT_FRAMES/OUTPUT_VFRAMES;
	const INFRAME_SIZE: usize = 8;
	const OUTFRAME_SIZE: usize = 16;
	const TILE_SIZE: usize = 8*OUTFRAME_SIZE;
	let mut input = vec![0;INPUT_FRAMES*INFRAME_SIZE];
	let mut output = vec![0;OUTPUT_FRAMES*OUTFRAME_SIZE];
	let mut bufuse = 0;
	while bufuse < INPUT_FRAMES*INFRAME_SIZE {
		let r = stdin().read(&mut input[bufuse..]).unwrap();
		if r == 0 { return false; }
		bufuse += r;
	}
	for i in 0..INPUT_FRAMES/8 {
		let x = i % 14;
		let y = i / 14;
		let o = OUTPUT_FRAMES-8-i*8;
		trans_one_tile(&mut output[OUTFRAME_SIZE*o..][..TILE_SIZE],
			&[&input[y*896+x*8+0*112..][..8], &input[y*896+x*8+1*112..][..8],
			&input[y*896+x*8+2*112..][..8], &input[y*896+x*8+3*112..][..8],
			&input[y*896+x*8+4*112..][..8], &input[y*896+x*8+5*112..][..8],
			&input[y*896+x*8+6*112..][..8], &input[y*896+x*8+7*112..][..8]]);
	}
	for i in 0..OUTPUT_VFRAMES {
		let base = (OUTPUT_VFRAMES - i - 1) * OUTFRAME_SIZE * OUTPUT_FRAMES_PER_VFRAME;
		let end = (OUTPUT_VFRAMES - i) * OUTFRAME_SIZE * OUTPUT_FRAMES_PER_VFRAME;
		let mut ptr = base;
		while ptr < end {
			let r = stdout().write(&mut output[ptr..end]).unwrap();
			ptr = ptr + r;
		}
	}
	return true;
}

fn trans_palette() -> bool
{
	let mut input = vec![0;768];
	let mut output = vec![0;1024];
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
		let o = 63-i;
		trans_one_iframe(&mut output[16*o..][..16], &input[8*i..][..8]);
	}
	let mut ptr = 0;
	while ptr < output.len() {
		let r = stdout().write(&mut output[ptr..]).unwrap();
		ptr = ptr + r;
	}
	return true;
}

fn main()
{
	if !trans_palette() { return; }
	loop { if !trans_frame_image() { return; } }
}
