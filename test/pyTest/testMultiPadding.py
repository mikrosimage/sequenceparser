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


def setUpSequenceWithPadding1():
    global path_bbb
    path_bbb = tempfile.mkdtemp('bbb', dir=root_aaa)
    # create sequence a1b2c0##.j2c
    createFile(path_bbb, 'a1b2c001.j2c')
    createFile(path_bbb, 'a1b2c002.j2c')
    createFile(path_bbb, 'a1b2c003.j2c')
    createFile(path_bbb, 'a1b2c023.j2c')


@with_setup(setUpSequenceWithPadding1, tearDownOneTest)
def testSequenceWithPadding1():
    """
    Check sequence detection with padding of one.
    """
    listSequence = getSequencesFromPath(path_bbb, seq.eDetectionDefault)
    assert_equals(len(listSequence), 1)
    assert_equals(listSequence[0].getSequence().getFixedPadding(), 3)
    assert_equals(listSequence[0].getSequence().getMaxPadding(), 3)


def setUpSequenceWithPadding2():
    global path_bbb
    path_bbb = tempfile.mkdtemp('bbb', dir=root_aaa)
    # create sequence a1b2c###.j2c
    createFile(path_bbb, 'a1b2c001.j2c')
    createFile(path_bbb, 'a1b2c002.j2c')
    createFile(path_bbb, 'a1b2c003.j2c')
    createFile(path_bbb, 'a1b2c809.j2c')
    createFile(path_bbb, 'a1b2c999.j2c')


@with_setup(setUpSequenceWithPadding2, tearDownOneTest)
def testSequenceWithPadding2():
    """
    Check sequence detection with padding of one.
    """
    listSequence = getSequencesFromPath(path_bbb, seq.eDetectionDefault)
    assert_equals(len(listSequence), 1)
    assert_equals(listSequence[0].getSequence().getFixedPadding(), 3)
    assert_equals(listSequence[0].getSequence().getMaxPadding(), 3)


def setUpSequenceWithoutPadding():
    global path_bbb
    path_bbb = tempfile.mkdtemp('bbb', dir=root_aaa)
    # create sequence without padding
    createFile(path_bbb, 'a1b2c99.j2c')
    createFile(path_bbb, 'a1b2c102.j2c')
    createFile(path_bbb, 'a1b2c1234.j2c')
    createFile(path_bbb, 'a1b2c12345.j2c')
    createFile(path_bbb, 'a1b2c123456.j2c')


@with_setup(setUpSequenceWithoutPadding, tearDownOneTest)
def testSequenceWithoutPadding():
    """
    Check sequence detection without padding.
    """
    listSequence = getSequencesFromPath(path_bbb, seq.eDetectionDefault)
    assert_equals(len(listSequence), 1)

    sequence = listSequence[0].getSequence()
    assert_equals(sequence.getFixedPadding(), 0)
    assert_equals(sequence.getMaxPadding(), 2)
    assert_equals(sequence.getFirstTime(), 99)
    assert_equals(sequence.getLastTime(), 123456)
    assert_equals(sequence.getNbFiles(), 5)
    assert_equals(sequence.hasMissingFile(), True)
    assert_equals(sequence.getFrameRanges().size(), 5)
#    assert_equals(sequence.getFrameRanges()[-1].step, 0)


def setUpDoublePaddingForTheSameSequence():
    global path_bbb
    path_bbb = tempfile.mkdtemp('bbb', dir=root_aaa)
    # create sequence wwith double padding
    createFile(path_bbb, 'a1b2c00099.j2c')
    createFile(path_bbb, 'a1b2c00102.j2c')

    createFile(path_bbb, 'a1b2c1234.j2c')
    createFile(path_bbb, 'a1b2c12345.j2c')
    createFile(path_bbb, 'a1b2c123456.j2c')


@with_setup(setUpDoublePaddingForTheSameSequence, tearDownOneTest)
def testDoublePaddingForTheSameSequence():
    """
    Check sequence detection with double padding in the same sequence.
    """
    listSequence = getSequencesFromPath(path_bbb, seq.eDetectionDefault)
    assert_equals(len(listSequence), 2)
