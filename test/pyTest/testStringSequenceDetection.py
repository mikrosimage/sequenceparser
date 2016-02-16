import tempfile
import shutil

from pySequenceParser import sequenceParser as seq
from . import createFile, getSequencesFromPath

from nose.tools import *

root_aaa = ''
path_bbb = ''


def setUp():
    """
    Main setup called after all tests.
    """
    global root_aaa
    root_aaa = tempfile.mkdtemp('aaa')


def tearDown():
    """
    Main tearDown called after all tests.
    Remove temporary folders and files.
    """
    global root_aaa
    shutil.rmtree(root_aaa)


def tearDownOneTest():
    """
    Common tearDown called after each test.
    """
    global path_bbb
    shutil.rmtree(path_bbb)


def testNothing():
    """
    Check sequence detection with nothing to browse.
    """
    listSequence = getSequencesFromPath(root_aaa, seq.eDetectionDefault)
    assert_equals(len(listSequence), 0)


def setUpSingleFilename():
    global path_bbb
    path_bbb = tempfile.mkdtemp('bbb', dir=root_aaa)
    createFile(path_bbb, "a1b2.j2c")


@with_setup(setUpSingleFilename, tearDownOneTest)
def testSingleFilename():
    """
    Check sequence detection with one file.
    """
    listSequence = getSequencesFromPath(path_bbb, seq.eDetectionDefault)
    assert_equals(len(listSequence), 0)


def setUpSingleSequence():
    global path_bbb
    path_bbb = tempfile.mkdtemp('bbb', dir=root_aaa)
    # create sequence a1b#.j2c
    createFile(path_bbb, "a1b2.j2c")
    createFile(path_bbb, "a1b3.j2c")
    createFile(path_bbb, "a1b4.j2c")


@with_setup(setUpSingleSequence, tearDownOneTest)
def testSingleSequence():
    """
    Check sequence detection with one sequence.
    """
    listSequence = getSequencesFromPath(path_bbb, seq.eDetectionDefault)
    assert_equals(len(listSequence), 1)


def setUpSingleSequenceMultiRanges():
    global path_bbb
    path_bbb = tempfile.mkdtemp('bbb', dir=root_aaa)
    # create sequence a1b#.j2c
    createFile(path_bbb, "a1b2.j2c")
    createFile(path_bbb, "a1b3.j2c")
    createFile(path_bbb, "a1b4.j2c")
    # create sequence a1b##.j2c
    createFile(path_bbb, "a1b14.j2c")
    createFile(path_bbb, "a1b15.j2c")
    createFile(path_bbb, "a1b16.j2c")
    createFile(path_bbb, "a1b20.j2c")
    createFile(path_bbb, "a1b22.j2c")
    createFile(path_bbb, "a1b24.j2c")


@with_setup(setUpSingleSequenceMultiRanges, tearDownOneTest)
def testSingleSequenceMultiRanges():
    """
    Check sequence detection with one sequence with multiple ranges.
    """
    listSequence = getSequencesFromPath(path_bbb, seq.eDetectionDefault)
    assert_equals(len(listSequence), 1)

    sequence = listSequence[0].getSequence()
    assert_equals(sequence.getFrameRanges().size(), 3)

    times = [2, 3, 4, 14, 15, 16, 20, 22, 24]
    for frame, time in zip(sequence.getFramesIterable(), times):
        assert_equals(frame, time)

    subtimes = [14, 15, 16]
    for frame, time in zip(sequence.getFramesIterable(6, 17), subtimes):
        assert_equals(frame, time)
