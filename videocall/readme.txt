To generate the video data, in transout/ do the following:

cargo build --release
ffmpeg -y -i foo.mp4 -r 10 -vf scale=112:104 -f rawvideo -pix_fmt rgb24 /dev/stdout | ./target/release/dirty-quantize | ./target/release/snes-transinput-videophone > bar.dat


To run the video data, append it to a video payload setup movie:

cat smw-vidphone-setup.r16m bar.dat > out.r16m
or
cat zelda-vidphone-setup.r16m bar.dat > out.r16m

Then play the r16m as 16 bit y-cable.