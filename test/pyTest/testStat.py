import time
import tempfile
import os
import shutil
import getpass
import grp

from pySequenceParser import sequenceParser as seq
from . import createFile, createFolder, createSymLink, getSequencesFromPath

from nose.tools import *

root_path = ''
user_name = getpass.getuser()
grp_name = grp.getgrnam(user_name).gr_name


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
    createSymLink(root_path, "plop.txt", "plop_sym_link.txt")


def tearDown():
    global root_path
    shutil.rmtree(root_path)


def checkCommonParameters(itemStat):
    """
    Check attributes which are common to several ItemStats.
    """
    # user and group
    assert_equals(itemStat.userName, user_name)
    assert_equals(itemStat.groupName, grp_name)
    # permissions
    assert_equals(itemStat.ownerCanRead, True)
    assert_equals(itemStat.ownerCanWrite, True)
    assert_equals(itemStat.ownerCanExecute, False)
    assert_equals(itemStat.groupCanRead, True)
    assert_equals(itemStat.groupCanWrite, True)
    assert_equals(itemStat.groupCanExecute, False)
    assert_equals(itemStat.otherCanRead, True)
    assert_equals(itemStat.otherCanWrite, False)
    assert_equals(itemStat.otherCanExecute, False)


def testFileStat():
    """
    Check stats of a single file.
    - 1 hard link
    - size = minSize = maxSize = realSize = sizeOnDisk
    - check the different times
    """
    itemFile = seq.Item(seq.eTypeFile, os.path.join(root_path, "plop.txt"))
    itemStat = seq.ItemStat(itemFile)
    checkCommonParameters(itemStat)
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
    # user and group
    assert_equals(itemStat.userName, user_name)
    assert_equals(itemStat.groupName, grp_name)
    # nb hard links
    assert_equals(itemStat.nbHardLinks, 1)
    assert_equals(itemStat.fullNbHardLinks, 1)
    # check size
    assert_equals(itemStat.size, len(dst))
    assert_equals(itemStat.size, itemStat.minSize)
    assert_equals(itemStat.size, itemStat.maxSize)
    assert_equals(itemStat.realSize, itemStat.size / itemStat.nbHardLinks)
    # permissions
    # On Linux, the permissions are always 0777 and can't be changed.
    assert_equals(itemStat.ownerCanRead, True)
    assert_equals(itemStat.ownerCanWrite, True)
    assert_equals(itemStat.ownerCanExecute, True)
    assert_equals(itemStat.groupCanRead, True)
    assert_equals(itemStat.groupCanWrite, True)
    assert_equals(itemStat.groupCanExecute, True)
    assert_equals(itemStat.otherCanRead, True)
    assert_equals(itemStat.otherCanWrite, True)
    assert_equals(itemStat.otherCanExecute, True)


def testFolderStat():
    """
    Check stats of a folder.
    - 2 hard links (. and ..)
    - sizes
    """
    itemFile = seq.Item(seq.eTypeFolder, os.path.join(root_path, "dir1"))
    itemStat = seq.ItemStat(itemFile)
    checkCommonParameters(itemStat)
    # nb hard links
    assert_equals(itemStat.nbHardLinks, 2)
    assert_equals(itemStat.fullNbHardLinks, 2)
    # check size
    assert_equals(itemStat.size, itemStat.minSize)
    assert_equals(itemStat.size, itemStat.maxSize)
    assert_equals(itemStat.realSize, itemStat.size)
    assert_greater_equal(itemStat.sizeOnDisk, itemStat.size)


def testSequenceStat():
    """
    Check stats of a sequence.
    - number of hard links
    - sizes
    """
    itemSequence = getSequencesFromPath(root_path, seq.eDetectionDefault)[0]
    nbFilesInSequence = itemSequence.getSequence().getNbFiles()
    itemStat = seq.ItemStat(itemSequence)
    checkCommonParameters(itemStat)
    # nb hard links
    assert_equals(itemStat.nbHardLinks, 1)
    assert_equals(itemStat.fullNbHardLinks, 3)
    # check size
    assert_equals(itemStat.size, itemStat.minSize * nbFilesInSequence)
    assert_equals(itemStat.size, itemStat.maxSize * nbFilesInSequence)
    assert_equals(itemStat.realSize, itemStat.size / itemStat.nbHardLinks)
    assert_greater_equal(itemStat.sizeOnDisk, itemStat.size)


def testUndefinedStat():
    """
    Check stats of a file which is tagged as undefined.
    """
    itemFile = seq.Item(seq.eTypeUndefined, os.path.join(root_path, "plop.txt"))
    itemStat = seq.ItemStat(itemFile)
    # user and group
    assert_equals(itemStat.userName, "unknown")
    assert_equals(itemStat.groupName, "unknown")
    # check id
    assert_equals(itemStat.deviceId, 0)
    assert_equals(itemStat.inodeId, 0)
    assert_equals(itemStat.userId, 0)
    assert_equals(itemStat.groupId, 0)
    # nb hard links
    assert_equals(itemStat.nbHardLinks, 0)
    assert_equals(itemStat.fullNbHardLinks, 0)
    # check size
    assert_equals(itemStat.size, 0)
    assert_equals(itemStat.minSize, 0)
    assert_equals(itemStat.maxSize, 0)
    assert_equals(itemStat.realSize, 0)
    assert_equals(itemStat.sizeOnDisk, 0)
    # time
    assert_equals(itemStat.accessTime, 0)
    assert_equals(itemStat.modificationTime, -1)
    assert_equals(itemStat.lastChangeTime, -1)
    # permissions
    assert_equals(itemStat.ownerCanRead, False)
    assert_equals(itemStat.ownerCanWrite, False)
    assert_equals(itemStat.ownerCanExecute, False)
    assert_equals(itemStat.groupCanRead, False)
    assert_equals(itemStat.groupCanWrite, False)
    assert_equals(itemStat.groupCanExecute, False)
    assert_equals(itemStat.otherCanRead, False)
    assert_equals(itemStat.otherCanWrite, False)
    assert_equals(itemStat.otherCanExecute, False)
