#!/usr/bin/env python
import sys
import os
name = ""
base_name = ""

latches = int(sys.argv[1])

if sys.argv[2] == "":
	print("You need to specify a file to convert")
	sys.exit()
else:
	name = sys.argv[2]
	base_name = sys.argv[2][:-2]

f = open(os.path.dirname(os.path.realpath(__file__)) + "/" + name, "rb")
fo = open(os.path.dirname(os.path.realpath(__file__)) + "/" + base_name + ".inp", "w")

data = f.read()
i = ""
for d in data:
	v = d
	i = ""
	i = i + ("R" if v & 1 != 0 else ".")
	i = i + ("L" if v & 2 != 0 else ".")
	i = i + ("D" if v & 4 != 0 else ".")
	i = i + ("U" if v & 8 != 0 else ".")
	i = i + ("T" if v & 16 != 0 else ".")
	i = i + ("S" if v & 32 != 0 else ".")
	i = i + ("B" if v & 64 != 0 else ".")
	i = i + ("A" if v & 128 != 0 else ".")
	for l in range(0, latches):
		fo.write(i + "|........\n")

f.close()
fo.close()
print("Converted " + str(len(data)) + " bytes.")
