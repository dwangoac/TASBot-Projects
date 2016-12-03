use std::cmp::max;
use std::env::args_os;
use std::fs::{File, OpenOptions};
use std::io::{stdout,Read, Write};
use std::time::{Duration, Instant};
use std::os::unix::io::AsRawFd;

static INIT_CODE: &'static [u8] = b"se1\x00\x00se2\x00\x00se3\x00\x00se4\x00\x00sp1\x01sc1\x81sc2\x81sp2\x01sc3\x81sc4\x81sA\x0fse1\x80\x0fr\x0f";

fn duration_n(x: Duration) -> u64
{
	(x.as_secs() as u64) * 10000000000 + (x.subsec_nanos() as u64)
}

fn read_frame(buf: &[u8], ptr: usize) -> [u8; 9]
{
	let mut frame = [0;9];
	frame[0] = b'A';
	frame[1] = buf.get(ptr).map(|x|{*x}).unwrap_or(0)^0xFF;
	frame[2] = buf.get(ptr+1).map(|x|{*x}).unwrap_or(0)^0xFF;
	frame[3] = buf.get(ptr+2).map(|x|{*x}).unwrap_or(0)^0xFF;
	frame[4] = buf.get(ptr+3).map(|x|{*x}).unwrap_or(0)^0xFF;
	frame[5] = buf.get(ptr+8).map(|x|{*x}).unwrap_or(0)^0xFF;
	frame[6] = buf.get(ptr+9).map(|x|{*x}).unwrap_or(0)^0xFF;
	frame[7] = buf.get(ptr+10).map(|x|{*x}).unwrap_or(0)^0xFF;
	frame[8] = buf.get(ptr+11).map(|x|{*x}).unwrap_or(0)^0xFF;
	frame
}

fn main()
{
	const MAX_QUERY: usize = 256;
	let mut buffer = Vec::new();
	let mut query = [0u8; MAX_QUERY];
	let mut response = [0u8; 9*MAX_QUERY];
	let mut port = None;
	let mut filename = None;
	let mut dummy = true;
	let mut peak_q = 0;
	for i in args_os() {
		if dummy {
			dummy = false;
		} else if port.is_none() {
			port = Some(i.clone());
		} else if filename.is_none() {
			filename = Some(i.clone());
		}
	}
	if filename.is_none() {
		panic!("Need port and filename");
	}
	let mut port_d = OpenOptions::new().read(true).write(true).open(port.clone().unwrap()).unwrap();
	let mut movie_d = File::open(filename.clone().unwrap()).unwrap();
	let port_fd = port_d.as_raw_fd();
	port_d.write_all(INIT_CODE).unwrap();
	movie_d.read_to_end(&mut buffer).unwrap();
	let mut mptr = 0;
	for i in 0..145 {
		let frame = read_frame(&buffer, mptr);
		mptr += 16;
		port_d.write_all(&frame).unwrap();
	}
	let mut max_p = 0;
	let mut max_r = 0;
	let mut first = true;
	let mut t5 = Instant::now();
	println!("Standby.");
	port_d.write_all("sd0".as_bytes()).unwrap();
	while mptr < buffer.len() {
		let t1 = Instant::now();
		let bytes = port_d.read(&mut query).unwrap();
		let t2 = Instant::now();
		let mut count = 0;
		for i in 0..bytes {
			if query[i] == b'f' /*&& query[i] <= b'i'*/ {
				count += 1;
			}
		}
		if count > peak_q { peak_q = count; }
		for i in 0..count {
			let frame = read_frame(&buffer, mptr);
			mptr += 16;
			(&mut response[9*i..9*i+9]).copy_from_slice(&frame);
		}
		port_d.write_all(&response[..9*count]).unwrap();
		//The first read timing is not reliable.
		let dt1 = duration_n(t2.duration_since(t1));
		max_r = max(max_r, dt1);
		max_p = if first { 0u64 } else { max(max_p, duration_n(t1.duration_since(t5))) };
		if count > 0 && mptr % 1600 == 0 {
			print!("\x1B[1Gf: {}, maxq={} maxr={} maxp={}\x1B[K", mptr/16, peak_q,
				max_r / 1000, max_p / 1000);
			stdout().flush().unwrap();
		}
		t5 = t2;
		first = false;
	}
	print!("\x1B[1Gf: {}, maxq={} maxr={} maxp={}\x1B[K", mptr/16, peak_q,
		max_r / 1000, max_p / 1000);
	println!("\nDone.");
}

/*
sp1 0x01
sc1 0x81
sc2 0x81
sp2 0x01
sc3 0x81
sc4 0x81
sA 0x0F
se1 0x80 0x0F
r 0x0F


('sA', '00001111')
('se1', '10000000', '00001111')
('r', '00001111')


*/
