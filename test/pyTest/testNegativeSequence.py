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


def setUpNoNegativeValues():
    global path_bbb
    path_bbb = tempfile.mkdtemp('bbb', dir=root_aaa)
    # create sequence a1b-2c#.j2c
    createFile(path_bbb, 'a1b-2c1.j2c')
    createFile(path_bbb, 'a1b-2c2.j2c')
    createFile(path_bbb, 'a1b-2c3.j2c')
    # create sequence a1b+3c#.j2c with holes
    createFile(path_bbb, 'a1b+3c6.j2c')
    createFile(path_bbb, 'a1b+3c2.j2c')
    createFile(path_bbb, 'a1b+3c0.j2c')
    createFile(path_bbb, 'a1b+3c3.j2c')
    createFile(path_bbb, 'a1b+3c9.j2c')
    # create sequence a1b9c#.j2c with holes
    createFile(path_bbb, 'a1b9c6.j2c')
    createFile(path_bbb, 'a1b9c2.j2c')


@with_setup(setUpNoNegativeValues, tearDownOneTest)
def testNoNegativeValues():
    """
    Check sequence detection with no negative values.
    """
    listSequence = getSequencesFromPath(path_bbb, seq.eDetectionDefault)
    assert_equals(len(listSequence), 3)

    # list sequences by nbFiles
    listSequence.sort(key=lambda x: x.getSequence().getNbFiles())

    assert_equals(listSequence[0].getSequence().getNbFiles(), 2)
    assert_equals(listSequence[0].getSequence().getFirstTime(), 2)
    assert_equals(listSequence[0].getSequence().getLastTime(), 6)

    assert_equals(listSequence[1].getSequence().getNbFiles(), 3)
    assert_equals(listSequence[1].getSequence().getFirstTime(), 1)
    assert_equals(listSequence[1].getSequence().getLastTime(), 3)

    assert_equals(listSequence[2].getSequence().getNbFiles(), 5)
    assert_equals(listSequence[2].getSequence().getFirstTime(), 0)
    assert_equals(listSequence[2].getSequence().getLastTime(), 9)


def setUpNegativeSequence():
    global path_bbb
    path_bbb = tempfile.mkdtemp('bbb', dir=root_aaa)
    # create sequence a1b2c#.j2c
    createFile(path_bbb, 'a1b2c-3.j2c')
    createFile(path_bbb, 'a1b2c-2.j2c')
    createFile(path_bbb, 'a1b2c-1.j2c')
    createFile(path_bbb, 'a1b2c0.j2c')
    createFile(path_bbb, 'a1b2c1.j2c')
    createFile(path_bbb, 'a1b2c2.j2c')
    createFile(path_bbb, 'a1b2c3.j2c')


@with_setup(setUpNegativeSequence, tearDownOneTest)
def testNegativeSequence():
    """
    Check sequence detection with negative values.
    """
    listSequence = getSequencesFromPath(path_bbb, seq.eDetectionNegative)
    assert_equals(len(listSequence), 1)

    sequence = listSequence[0].getSequence()
    assert_equals(sequence.getFirstTime(), -3)
    assert_equals(sequence.getLastTime(), 3)
    assert_equals(sequence.getNbFiles(), 7)
    assert_equals(sequence.hasMissingFile(), False)
    assert_equals(sequence.getFrameRanges()[-1].step, 1)
    

def setUpSignedSequencePlus():
    global path_bbb
    path_bbb = tempfile.mkdtemp('bbb', dir=root_aaa)
    # create sequence a1b2c#.j2c
    createFile(path_bbb, 'a1b2c-3.j2c')
    createFile(path_bbb, 'a1b2c-2.j2c')
    createFile(path_bbb, 'a1b2c-1.j2c')
    createFile(path_bbb, 'a1b2c+0.j2c')
    createFile(path_bbb, 'a1b2c+1.j2c')
    createFile(path_bbb, 'a1b2c+2.j2c')
    createFile(path_bbb, 'a1b2c+3.j2c')


@with_setup(setUpSignedSequencePlus, tearDownOneTest)
def testSignedSequencePlus():
    """
    Check sequence detection with plus values.
    """
    listSequence = getSequencesFromPath(path_bbb, seq.eDetectionNegative)
    assert_equals(len(listSequence), 1)

    sequence = listSequence[0].getSequence()
    assert_equals(sequence.getFirstTime(), -3)
    assert_equals(sequence.getLastTime(), 3)
    assert_equals(sequence.getNbFiles(), 7)
    assert_equals(sequence.hasMissingFile(), False)
    assert_equals(sequence.getFrameRanges()[-1].step, 1)
    

def setUpAmbiguousSignedSequencePlus():
    global path_bbb
    path_bbb = tempfile.mkdtemp('bbb', dir=root_aaa)
    # create sequence a1b2c#.j2c
    createFile(path_bbb, 'a1b2c-3.j2c')
    createFile(path_bbb, 'a1b2c-2.j2c')
    createFile(path_bbb, 'a1b2c-1.j2c')
    createFile(path_bbb, 'a1b2c+0.j2c')
    createFile(path_bbb, 'a1b2c+1.j2c')
    createFile(path_bbb, 'a1b2c+2.j2c')
    createFile(path_bbb, 'a1b2c+3.j2c')
    createFile(path_bbb, 'a1b2c2.j2c')
    createFile(path_bbb, 'a1b2c8.j2c')


@with_setup(setUpAmbiguousSignedSequencePlus, tearDownOneTest)
def testAmbiguousSignedSequencePlus():
    """
    Check sequence detection with plus values with ambiguity.
    """
    listSequence = getSequencesFromPath(path_bbb, seq.eDetectionNegative)
    assert_equals(len(listSequence), 1)

    sequence = listSequence[0].getSequence()
    assert_equals(sequence.getFirstTime(), -3)
    assert_equals(sequence.getLastTime(), 8)
    assert_equals(sequence.getNbFiles(), 9)
    assert_equals(sequence.hasMissingFile(), True)
    assert_equals(sequence.getFrameRanges()[-1].step, 1)
    

def setUpAmbiguousNegativeZeroMinus():
    global path_bbb
    path_bbb = tempfile.mkdtemp('bbb', dir=root_aaa)
    # create sequence a1b2c#.j2c
    createFile(path_bbb, 'a1b2c-3.j2c')
    createFile(path_bbb, 'a1b2c-2.j2c')
    createFile(path_bbb, 'a1b2c-1.j2c')
    createFile(path_bbb, 'a1b2c-0.j2c')


@with_setup(setUpAmbiguousNegativeZeroMinus, tearDownOneTest)
def testAmbiguousNegativeZeroMinus():
    """
    Check sequence detection with minus values with a -0.
    """
    listSequence = getSequencesFromPath(path_bbb, seq.eDetectionNegative | seq.eDetectionSequenceNeedAtLeastTwoFiles)
    assert_equals(len(listSequence), 1)
