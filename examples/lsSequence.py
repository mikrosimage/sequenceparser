#!/usr/bin/python
# -*- coding: utf-8 -*-

import os, sys
from pySequenceParser import sequenceParser as sq

if len(sys.argv) > 2:
    print( 'lsSequence takes only one path.' )
    sys.exit()

path = os.curdir
if len(sys.argv) == 2 :
    path = sys.argv[1]

items = sq.browse( path )
sequences = [f for f in items if f.getType() == sq.eTypeSequence]

for s in sequences :
    print s.getSequence().getFixedPadding()
    print s.getSequence().getMaxPadding()
    print s.getSequence()
    print s.getSequence().getFilenameWithPrintfPattern()

