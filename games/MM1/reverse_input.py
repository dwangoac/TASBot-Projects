#!/usr/bin/env python
import sys
import os
name = ""
base_name = ""

if sys.argv[1] == "":
	print("You need to specify a file to convert")
	sys.exit()
else:
	name = sys.argv[1]
	base_name = sys.argv[1][:-4]

f = open(os.path.dirname(os.path.realpath(__file__)) + "\\" + name, "r")
fo = open(os.path.dirname(os.path.realpath(__file__)) + "\\" + base_name + ".rev.inp", "w")

lines = f.readlines()
lines.reverse()
fo.writelines(lines)
f.close()
fo.close()