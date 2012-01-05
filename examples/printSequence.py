#!/usr/bin/python
# -*- coding: utf-8 -*-
import os, sys
import sequenceParser

if len(sys.argv) > 2:
	print( 'lsFile took only one path' )
	sys.exit()

detector = sequenceParser.Detector()

path = os.curdir
if len(sys.argv) == 2 :
	path = sys.argv[1]

detector.printSequenceInDirectory( path )

