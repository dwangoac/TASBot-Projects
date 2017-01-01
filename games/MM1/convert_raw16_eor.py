import sys, os

f = open(sys.argv[1], "rb")
fo = open(sys.argv[1] + ".r16eor", "wb")

data = f.read()
for i in range(0, len(data)-4, 4):
	outp = []
	outp = outp + [data[i]^0x90, data[i+1]^0x90, 0, 0, 0, 0, 0, 0]
	outp = outp + [data[i+2]^0x80, data[i+3]^0x80, 0, 0, 0, 0, 0, 0]
	fo.write(bytes(outp))
	
f.close()
fo.close()
print("Converted " + str(len(data)) + " bytes.")