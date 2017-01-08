import serial, sys, time, os, bz2, gc

# disable gc
gc.disable()

# connect to device
ser = serial.Serial('COM3', 2000000, timeout=0.1)

# send "ping" command to make sure device is there
ser.write(b'\xFF')
data = ser.read()
if data == b'\xFF':
	print("+++ Connected to device, device is ready to receive commands...")
else:
	print("!!! Device is not ready, exiting...")
	sys.exit()

f = None	
filename = sys.argv[1]
if filename[-3:].lower() == "bz2":
	f = bz2.BZ2File(filename, "r")
else:	
	f = open(filename, "rb")

# reset device
print("--- Sending reset command to device")
ser.write(b'\x00')
time.sleep(0.1)

# set window size
ser.write(bytes([0xA0, 0x40, 0x00]))  # 16384 = ~0.68ms

# set window off at
ser.write(bytes([0xA1, 0x08, 0x49])) # 2121

# set clock filter timers (DPCM fix)
ser.write(bytes([0xA4, 128])) # Port 1 timer (128 = 5us)
ser.write(bytes([0xB4, 128])) # Port 2 timer (128 = 5us)

# start run
print("--- Sending start command to device")
ser.write(b'\x01\x02\x02\x03\x00\x01\x00') # command 1 (play), 16-bits, 2 port, 3 datalines, sync, no window 1, no window 2

latches = 0
extra = 0
skip = 0

for n in range(0, skip):
	f.read(16)

cmd = None
data = None
inputs = None

# set to True to turn off DPCM fix at a specific frame
switch = True
switch_at = 16800

print("--- Starting read loop")
while True:
	cmd = ser.read()
	if cmd == b'\x0F':
		if extra > 0:
			inputs = f.read(80 - (extra * 16))
			data = []
			for i in range(0, len(inputs), 16):
				data = data + [inputs[i], inputs[i+1], inputs[i+2], inputs[i+3], inputs[i+4], inputs[i+5]]
				data = data + [inputs[i+8], inputs[i+9], inputs[i+10], inputs[i+11], inputs[i+12], inputs[i+13]]
			
			data = ([0] * (extra * 12)) + data
			ser.write(bytes([0x0F] + data))
			extra = 0
		else:
			inputs = f.read(80)
			data = []
			for i in range(0, len(inputs), 16):
				data = data + [inputs[i], inputs[i+1], inputs[i+2], inputs[i+3], inputs[i+4], inputs[i+5]]
				data = data + [inputs[i+8], inputs[i+9], inputs[i+10], inputs[i+11], inputs[i+12], inputs[i+13]]
			
			ser.write(bytes([0x0F] + data))
		
		latches = latches + 5
		
		if latches > switch_at and switch == True:
			ser.write(bytes([0xA4, 2])) #
			ser.write(bytes([0xB4, 2])) # switch off DPCM fix on both ports
			switch = False
			
		if latches % 600 == 0:
			print('*** Latches: [%d] - Data: [%x]' % (latches, data[0]))
