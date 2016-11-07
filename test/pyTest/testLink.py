import time
import tempfile
import os
import shutil

from pySequenceParser import sequenceParser as seq
from . import createFile, createFolder, getSequencesFromPath

from nose.tools import *

root_path = ''

def setUp():
    global root_path
    root_path = tempfile.mkdtemp()

    # create files
    files_to_create = [
        "plop.txt"
    ]
    for f in files_to_create:
        createFile(root_path, f)

    # create folder
    createFolder(root_path, "dir1")

    # create sym link
    os.symlink(os.path.join(root_path, "plop.txt"), os.path.join(root_path, "plop_sym_link.txt"))
    os.symlink(os.path.join(root_path, "dir1"), os.path.join(root_path, "dir2"))

def tearDown():
    global root_path
    shutil.rmtree(root_path)

def testLinkType():
    """
    Check stats of a synbolic link folder.
    - 2 hard links (. and ..)
    - sizes
    """
    print("testFolderLinkStat:")
    print("===================")
    items = seq.browse(root_path)
    for item in items:
        if item.getFilename() in ("plop_sym_link.txt", "dir2"):
            assert_equals(item.getType(), seq.eTypeLink)
        elif item.getFilename() == "plop.txt":
            assert_equals(item.getType(), seq.eTypeFile)
        elif item.getFilename() == "dir1":
            assert_equals(item.getType(), seq.eTypeFolder)
