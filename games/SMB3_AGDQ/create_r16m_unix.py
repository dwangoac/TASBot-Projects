#!/usr/bin/env python
import sys, os

name = ""
base_name = ""

if sys.argv[1] == "":
	print("You need to specify a file to convert")
	sys.exit()
else:
	name = sys.argv[1]
	base_name = sys.argv[1][:-4]

fi = open(os.path.dirname(os.path.realpath(__file__)) + "/" + name, "r")
fo = open(os.path.dirname(os.path.realpath(__file__)) + "/" + base_name + ".r16m", "wb")

for l in fi:
    inputs = l.replace('\n','').replace('\r','').split('|')
    for i in inputs:
        if len(i)>1:
            o = 0
            o += (1 if i[0] != '.' else 0)
            o += (2 if i[1] != '.' else 0)
            o += (4 if i[2] != '.' else 0)
            o += (8 if i[3] != '.' else 0)
            o += (16 if i[4] != '.' else 0)
            o += (32 if i[5] != '.' else 0)
            o += (64 if i[6] != '.' else 0)
            o += (128 if i[7] != '.' else 0)
            fo.write(bytes([o]))
            fo.write(bytes([0]*7))
