#!/usr/bin/python
# -*- coding: utf-8 -*-

import os, sys
from pySequenceParser import sequenceParser as sq

if len(sys.argv) > 2:
    print( 'lsFolder takes only one path.' )
    sys.exit()

path = os.curdir
if len(sys.argv) == 2 :
    path = sys.argv[1]

items = sq.browse( path )
dirs = [d for d in items if d.getType() == sq.eTypeFolder]

for d in dirs :
    print d

