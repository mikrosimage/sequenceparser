#!/usr/bin/python
# -*- coding: utf-8 -*-
import os, sys
import sequenceParser

if len(sys.argv) > 2:
	print( 'lsSequence takes only one path.' )
	sys.exit()

path = os.curdir
if len(sys.argv) == 2 :
	path = sys.argv[1]

files = sequenceParser.sequenceInDirectory( path )

for f in files :
	print f

