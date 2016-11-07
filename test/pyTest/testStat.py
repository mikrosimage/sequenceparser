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
        "plop.txt",
        "foo.001.png",
        "foo.002.png",
        "foo.003.png",
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


def testFileStat():
    """
    Check stats of a single file.
    - 1 hard link
    - size = minSize = maxSize = realSize = sizeOnDisk
    - check the different times
    """
    itemFile = seq.Item(seq.eTypeFile, os.path.join(root_path, "plop.txt"))
    itemStat = seq.ItemStat(itemFile)
    # nb hard links
    assert_equals(itemStat.nbHardLinks, 1)
    assert_equals(itemStat.fullNbHardLinks, 1)
    # check size
    assert_equals(itemStat.size, itemStat.minSize)
    assert_equals(itemStat.size, itemStat.maxSize)
    assert_equals(itemStat.realSize, itemStat.size / itemStat.nbHardLinks)
    assert_greater_equal(itemStat.sizeOnDisk, itemStat.size)
    # time
    currentTime = round(time.time())
    assert_less_equal(itemStat.accessTime, currentTime)
    assert_less_equal(itemStat.modificationTime, currentTime)
    assert_less_equal(itemStat.lastChangeTime, currentTime)


def testSymLinkStat():
    """
    Check stats of a folder.
    - 1 hard link
    - sizes
    """
    src = os.path.join(root_path, "plop_sym_link.txt")
    dst = os.path.join(root_path, "plop.txt")
    itemFile = seq.Item(seq.eTypeLink, src)
    itemStat = seq.ItemStat(itemFile)
    # nb hard links
    assert_equals(itemStat.nbHardLinks, 1)
    assert_equals(itemStat.fullNbHardLinks, 1)
    # check size
    assert_equals(itemStat.size, len(dst))
    assert_equals(itemStat.size, itemStat.minSize)
    assert_equals(itemStat.size, itemStat.maxSize)
    assert_equals(itemStat.realSize, itemStat.size / itemStat.nbHardLinks)


def testFolderStat():
    """
    Check stats of a folder.
    - 2 hard links (. and ..)
    - sizes
    """
    itemFile = seq.Item(seq.eTypeFile, os.path.join(root_path, "dir1"))
    itemStat = seq.ItemStat(itemFile)
    # nb hard links
    assert_equals(itemStat.nbHardLinks, 2)
    assert_equals(itemStat.fullNbHardLinks, 2)
    # check size
    assert_equals(itemStat.size, itemStat.minSize)
    assert_equals(itemStat.size, itemStat.maxSize)
    #assert_equals(itemStat.realSize, itemStat.size) # 0L != -1L
    assert_greater_equal(itemStat.sizeOnDisk, itemStat.size)

def testFolderLinkStat():
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
        elif item.getFilename() == "foo.###.png":
            assert_equals(item.getType(), seq.eTypeSequence)
        elif item.getFilename() == "dir1":
            assert_equals(item.getType(), seq.eTypeFolder)

def testSequenceStat():
    """
    Check stats of a sequence.
    - number of hard links
    - sizes
    """
    itemSequence = getSequencesFromPath(root_path, seq.eDetectionDefault)[0]
    nbFilesInSequence = itemSequence.getSequence().getNbFiles()
    itemStat = seq.ItemStat(itemSequence)
    # nb hard links
    assert_equals(itemStat.nbHardLinks, 1)
    assert_equals(itemStat.fullNbHardLinks, 3)
    # check size
    assert_equals(itemStat.size, itemStat.minSize * nbFilesInSequence)
    assert_equals(itemStat.size, itemStat.maxSize * nbFilesInSequence)
    assert_equals(itemStat.realSize, itemStat.size / itemStat.nbHardLinks)
    assert_greater_equal(itemStat.sizeOnDisk, itemStat.size)
