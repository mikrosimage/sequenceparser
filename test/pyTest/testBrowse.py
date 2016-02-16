import tempfile
import os
import shutil

from pySequenceParser import sequenceParser as seq

from nose.tools import *

root_path = ''


def setUp():
    global root_path
    root_path = tempfile.mkdtemp()
    files_to_create = [
        "plop.txt",
        "foo.001.png",
        "foo.002.png",
        "foo.003.png",
        "foo.006.png",
        "a.1",
        "a.2",
        "4",
        "5",
        "6",
        "11",
    ]
    for f in files_to_create:
        # create an empty file
        ff = os.path.join(root_path, f)
        open(ff, 'w').close()
    dirs_to_create = [
        "dir1",
        "dir2",
        "dir3",
        "dir_d",
        "dir_f",
    ]
    for d in dirs_to_create:
        dd = os.path.join(root_path, d)
        os.mkdir(dd)


def tearDown():
    global root_path
    shutil.rmtree(root_path)


def testBrowse():
    global root_path
    items = seq.browse(root_path)
    for item in items:
        print("item:", item)
        print("item:", item.getFolder())
        print("item:", item.getFilename())
        print("item:", item.getType())
        if item.getType() == seq.eTypeSequence:
            sequence = item.getSequence()
            print("item sequence:", sequence.getFirstFilename())
            print("item sequence:", sequence.getFirstTime(), sequence.getLastTime(), sequence.getDuration(), sequence.getFilenameWithStandardPattern())
            for f in sequence.getFramesIterable():
                print("file:", sequence.getFilenameAt(f))

