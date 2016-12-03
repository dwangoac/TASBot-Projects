use std::io::{Read as IoRead,Write as IoWrite};
use std::io::{stdin,stdout};

struct Poller<'a>
{
	rawdata: &'a [u8],
	counter: usize,
}

impl<'a> Poller<'a>
{
	fn poll_word(&mut self) -> u16
	{
		//Assume MDR=0 and 16-bit read. Assume all-1 for overread.
		if self.counter == 16 { return 0x1f03; }
		let a = (self.rawdata[0+self.counter/8] >> (7-self.counter%8))&1;
		let b = (self.rawdata[2+self.counter/8] >> (7-self.counter%8))&1;
		let c = (self.rawdata[8+self.counter/8] >> (7-self.counter%8))&1;
		let d = (self.rawdata[10+self.counter/8] >> (7-self.counter%8))&1;
		self.counter += 1;
		512 * (d as u16) + 256 * (c as u16) + 2 * (b as u16) + 1 * (a as u16) + 0x1c00
	}
}

fn one_iframe<'a>(output: &mut [u16], sp: &mut usize, mut poller: Poller<'a>)
{
	for _ in 0..4 {
		let mut a = poller.poll_word();
		a <<= 2;
		a ^= poller.poll_word();
		a <<= 2;
		a ^= poller.poll_word();
		a <<= 2;
		a ^= poller.poll_word();
		output[*sp] = a;
		*sp = (*sp).wrapping_sub(1);
	}
}

//Output is 243*4 words. Input is 243*16 bytes.
fn one_vframe(output: &mut [u16], input: &[u8])
{
	let mut sp = 243 * 4-1;
	for i in 0..243 {
		let p = Poller{rawdata:&input[16*i..][..16], counter:0};
		one_iframe(output, &mut sp, p);
	}
}

//Output is 6*243*4 words. Input is 23,328 bytes.
fn one_halfcycle(output: &mut [u16], input: &[u8])
{
	for i in 0..6 {
		one_vframe(&mut output[243*4*i..][..243*4], &input[243*16*i..][..243*16])
	}
}

//Decode a picture. Output is 11,648 bytes, the input is 5,824 words.
fn decode_picture(output: &mut [u8], input: &[u16])
{
	for i in 0..11648 { output[i] = 0; }
	for i in 0..4 {
		for j in 0..1456 {
			let word = input[8*i+(j/8*32)+(j%8)];
			let xtile = (j / 8) % 14;
			let ytile = (j / 8) / 14;
			let yoff = j % 8;
			let x = xtile * 8;
			let y = ytile * 8 + yoff;
			for k in 0..8 {
				if word >> (7-k) & 1 != 0 { output[y*112+x+k] |= 1 << (2 * i); }
				if word >> (15-k) & 1 != 0 { output[y*112+x+k] |= 2 << (2 * i); }
			}
		}
	}
}

//Output is 768 words. Input is 1024 bytes.
fn one_vframe_palette(output: &mut [u8], input: &[u8])
{
	let mut tmp = [0; 256];
	let mut sp = 255;
	for i in 0..64 {
		let p = Poller{rawdata:&input[16*i..][..16], counter:0};
		one_iframe(&mut tmp, &mut sp, p);
	}
	for i in 0..256 {
		let r = (tmp[i] & 0x1F) as u8;
		let g = (tmp[i] >> 5 & 0x1F) as u8;
		let b = (tmp[i] >> 10 & 0x1F) as u8;
		output[3*i+0] = (r << 3) | (r >> 2);
		output[3*i+1] = (g << 3) | (g >> 2);
		output[3*i+2] = (b << 3) | (b >> 2);
	}
}

fn main()
{
	let mut input = vec![0u8;1458*16];
	let mut intermediate = vec![0u16;1458*4];
	let mut output = vec![0u8;112*104];
	stdin().read_exact(&mut input[0..1024]).unwrap();
	one_vframe_palette(&mut output[0..768], &input[0..1024]);
	stdout().write_all(&output[0..768]).unwrap();
	loop {
		stdin().read_exact(&mut input).unwrap();
		one_halfcycle(&mut intermediate, &input);
		decode_picture(&mut output, &intermediate);
		stdout().write_all(&output).unwrap();
	}
}
