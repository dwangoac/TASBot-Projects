import sys, os

f = open(sys.argv[1], "rb")
fo = open(sys.argv[1] + ".r16m", "wb")

data = f.read()
for i in range(0, len(data), 8):
	b = [0, 0, 0, 0, 0, 0, 0, 0]
	
	for n in range(0, 8, 2):
		outp = [0, 0]
		try:
			d = (data[i+n]>>1)
			data0 = d&1
			data1 = (d>>1) & 1
			data2 = (d>>2) & 1
			data3 = (d>>3) & 1
			data4 = (d>>4) & 1
			data5 = (d>>5) & 1
			data6 = (d>>6) & 1

			outp[0] = data3
			outp[1] = (data6 ^ 1)

			outp[0] = (outp[0]<<1) + data2
			outp[1] = (outp[1]<<1) + data5

			outp[0] = (outp[0]<<1) + data1
			outp[1] = (outp[1]<<1) + data4

			outp[0] = (outp[0]<<1) + data0
			outp[1] = (outp[1]<<1) + 0
			
			d = (data[i+n+1]>>1)
			data0 = d&1
			data1 = (d>>1) & 1
			data2 = (d>>2) & 1
			data3 = (d>>3) & 1
			data4 = (d>>4) & 1
			data5 = (d>>5) & 1
			data6 = (d>>6) & 1

			outp[0] = (outp[0]<<1) + data3
			outp[1] = (outp[1]<<1) + (data6 ^ 1)

			outp[0] = (outp[0]<<1) + data2
			outp[1] = (outp[1]<<1) + data5

			outp[0] = (outp[0]<<1) + data1
			outp[1] = (outp[1]<<1) + data4

			outp[0] = (outp[0]<<1) + data0
			outp[1] = (outp[1]<<1) + 0		
			
			b[n] = outp[0]
			b[n+1] = outp[1]
		except:
			pass
	
	fo.write( bytes([b[0],b[2],b[1],b[3],0,0,0,0,b[4],b[6],b[5],b[7],0,0,0,0] ))
	
f.close()
fo.close()
print("Converted " + str(len(data)) + " bytes.")