use std::io::{stdin, stdout, stderr, Read, Write};
const LEVELS: [u8;6] = [0, 51, 102, 153, 204, 255];

fn emit_palette()
{
	let mut palette = [0u8; 768];
	for i in 0..216 {
		let r = LEVELS[i / 36];
		let g = LEVELS[i / 6 % 6];
		let b = LEVELS[i % 6];
		palette[3*i+3] = r;
		palette[3*i+4] = g;
		palette[3*i+5] = b;
	}
	stdout().write_all(&palette).unwrap();
}

//Framedata is 112*104*4 bytes.
fn quantize_frame(framedata: &[u8])
{
	let mut out = [0; 112*104];
	let mut idx = 0;
	for i in 0..112*104 {
		let r = framedata[idx+0] as u32;
		let g = framedata[idx+1] as u32;
		let b = framedata[idx+2] as u32;
		let r = (5 * r + 127) / 255;
		let g = (5 * g + 127) / 255;
		let b = (5 * b + 127) / 255;
		let e = 36 * r + 6 * g + b + 1;
		out[i] = e as u8;
		idx += 3;
	}
	stdout().write_all(&out).unwrap();
}

fn main()
{
	let mut buf = [0;112*104*3];
	emit_palette();
	loop {
		stdin().read_exact(&mut buf).unwrap();
		quantize_frame(&buf);
	}
}
