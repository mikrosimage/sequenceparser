#!/usr/bin/python
# -*- coding: utf-8 -*-

import os, sys
from pySequenceParser import sequenceParser as sq

if len(sys.argv) > 2:
	print( 'lsFile takes only one path.' )
	sys.exit()

path = os.curdir
if len(sys.argv) == 2 :
    path = sys.argv[1]

items = sq.browse( path )
files = [f for f in items if f.getType() == sq.eTypeFile]

for f in files :
    print f
