import tempfile
import shutil

from pySequenceParser import sequenceParser as seq
from . import createFile, createFolder, createSymLink

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
    createSymLink(root_path, "plop.txt", "plop_sym_link.txt")
    createSymLink(root_path, "dir1", "dir2")

def tearDown():
    global root_path
    shutil.rmtree(root_path)

def testLinkType():
    """
    Check item types return by browse method.
    """
    items = seq.browse(root_path)
    for item in items:
        if item.getFilename() in ("plop_sym_link.txt", "dir2"):
            assert_equals(item.getType(), seq.eTypeLink)
        elif item.getFilename() == "plop.txt":
            assert_equals(item.getType(), seq.eTypeFile)
        elif item.getFilename() == "dir1":
            assert_equals(item.getType(), seq.eTypeFolder)
