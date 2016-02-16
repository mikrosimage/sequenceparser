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


def setUpSimpleMultiSequence():
    global path_bbb
    path_bbb = tempfile.mkdtemp('bbb', dir=root_aaa)
    # create sequence a1b2c#.j2c
    createFile(path_bbb, 'a1b2c1.j2c')
    createFile(path_bbb, 'a1b2c2.j2c')
    createFile(path_bbb, 'a1b2c3.j2c')
    # create sequence a1b3c#.j2c with holes
    createFile(path_bbb, 'a1b3c6.j2c')
    createFile(path_bbb, 'a1b3c2.j2c')
    createFile(path_bbb, 'a1b3c3.j2c')
    # create sequence a1b9c#.j2c with holes
    createFile(path_bbb, 'a1b9c6.j2c')
    createFile(path_bbb, 'a1b9c2.j2c')


@with_setup(setUpSimpleMultiSequence, tearDownOneTest)
def testSimpleMultiSequence():
    """
    Check sequence detection with mutiple sequences.
    """
    listSequence = getSequencesFromPath(path_bbb, seq.eDetectionDefault)
    assert_equals(len(listSequence), 3)


def setUpSimpleMultiSequenceMultiLevel():
    global path_bbb
    path_bbb = tempfile.mkdtemp('bbb', dir=root_aaa)
    # create sequence a1b2c#.j2c
    createFile(path_bbb, 'a1b2c1.j2c')
    createFile(path_bbb, 'a1b2c2.j2c')
    createFile(path_bbb, 'a1b2c3.j2c')
    # create sequence a1b#c4.j2c
    createFile(path_bbb, 'a1b3c4.j2c')
    createFile(path_bbb, 'a1b4c4.j2c')
    createFile(path_bbb, 'a1b5c4.j2c')
    createFile(path_bbb, 'a1b6c4.j2c')


@with_setup(setUpSimpleMultiSequenceMultiLevel, tearDownOneTest)
def testSimpleMultiSequenceMultiLevel():
    """
    Check sequence detection with mutiple sequences with multiple levels.
    """
    listSequence = getSequencesFromPath(path_bbb, seq.eDetectionDefault)
    assert_equals(len(listSequence), 2)
    assert_equals(listSequence[0].getSequence().getNbFiles(), 3)
    assert_equals(listSequence[1].getSequence().getNbFiles(), 4)


def setUpMultiSequenceMultiLevelMultiPadding():
    global path_bbb
    path_bbb = tempfile.mkdtemp('bbb', dir=root_aaa)
    # create sequence a1b2c#.j2c with a hole
    createFile(path_bbb, 'a1b2c1.j2c')
    createFile(path_bbb, 'a1b2c3.j2c')
    # create one file
    createFile(path_bbb, 'a1b002c2.j2c')
    # create sequence a1b#c4.j2c
    createFile(path_bbb, 'a1b3c4.j2c')
    createFile(path_bbb, 'a1b4c4.j2c')
    createFile(path_bbb, 'a1b5c4.j2c')
    createFile(path_bbb, 'a1b6c4.j2c')


@with_setup(setUpMultiSequenceMultiLevelMultiPadding, tearDownOneTest)
def testMultiSequenceMultiLevelMultiPadding():
    """
    Check sequence detection with mutiple sequences with multiple levels with multiples padding.
    """
    listSequence = getSequencesFromPath(path_bbb, seq.eDetectionDefault)
    assert_equals(len(listSequence), 2)


def setUpMultiSequenceMultiLevelMultiPadding2():
    global path_bbb
    path_bbb = tempfile.mkdtemp('bbb', dir=root_aaa)
    # create sequence a1b2c#.j2c with a hole
    createFile(path_bbb, 'a1b2c1.j2c')
    createFile(path_bbb, 'a1b2c3.j2c')
    # create sequence a1b002c#.j2c
    createFile(path_bbb, 'a1b002c2.j2c')
    createFile(path_bbb, 'a1b002c3.j2c')
    # create sequence a1b#c4.j2c
    createFile(path_bbb, 'a1b3c4.j2c')
    createFile(path_bbb, 'a1b4c4.j2c')
    createFile(path_bbb, 'a1b5c4.j2c')
    createFile(path_bbb, 'a1b6c4.j2c')


@with_setup(setUpMultiSequenceMultiLevelMultiPadding2, tearDownOneTest)
def testMultiSequenceMultiLevelMultiPadding2():
    """
    Check sequence detection with mutiple sequences with multiple levels with multiples padding.
    """
    listSequence = getSequencesFromPath(path_bbb, seq.eDetectionDefault)
    assert_equals(len(listSequence), 3)


def setUpMultiSequenceMultiLevelMultiPaddingWithNegValues():
    global path_bbb
    path_bbb = tempfile.mkdtemp('bbb', dir=root_aaa)
    # create sequence a1b2c#.j2c with a hole
    createFile(path_bbb, 'a1b2c1.j2c')
    createFile(path_bbb, 'a1b2c3.j2c')
    # create sequence a1b#c4.j2c with a hole
    createFile(path_bbb, 'a1b3c4.j2c')
    createFile(path_bbb, 'a1b5c4.j2c')
    createFile(path_bbb, 'a1b6c4.j2c')
    # create sequence a1b002c#.j2c with holes
    createFile(path_bbb, 'a1b002c2.j2c')
    createFile(path_bbb, 'a1b002c-3.j2c')
    # create sequence a1b#c-4.j2c with holes
    createFile(path_bbb, 'a1b4c-4.j2c')
    createFile(path_bbb, 'a1b5c-4.j2c')
    createFile(path_bbb, 'a1b-6c-4.j2c')


@with_setup(setUpMultiSequenceMultiLevelMultiPaddingWithNegValues, tearDownOneTest)
def testMultiSequenceMultiLevelMultiPaddingWithNegValues():
    """
    Check sequence detection with mutiple sequences with multiple levels with multiples padding with negative values.
    """
    listSequence = getSequencesFromPath(path_bbb, seq.eDetectionNegative | seq.eDetectionSequenceNeedAtLeastTwoFiles)
    assert_equals(len(listSequence), 4)


def setUpMultiSequenceMinusSeparatorAmbiguityWithNegValues():
    global path_bbb
    path_bbb = tempfile.mkdtemp('bbb', dir=root_aaa)
    # create sequence a1b2#.j2c with holes
    createFile(path_bbb, 'a1b2-1.j2c')
    createFile(path_bbb, 'a1b2-3.j2c')
    createFile(path_bbb, 'a1b2+5.j2c')
    createFile(path_bbb, 'a1b2+10.j2c')
    createFile(path_bbb, 'a1b24.j2c')


@with_setup(setUpMultiSequenceMinusSeparatorAmbiguityWithNegValues, tearDownOneTest)
def testMultiSequenceMinusSeparatorAmbiguityWithNegValues():
    """
    Check sequence detection with mutiple sequences with negative values with ambiguity.
    """
    listSequence = getSequencesFromPath(path_bbb, seq.eDetectionNegative | seq.eDetectionSequenceNeedAtLeastTwoFiles)
    assert_equals(len(listSequence), 1)
    assert_equals(listSequence[0].getSequence().getFirstTime(), -3)
    assert_equals(listSequence[0].getSequence().getLastTime(), 10)
    assert_equals(listSequence[0].getSequence().getFrameRanges().size(), 4)
    assert_equals(listSequence[0].getSequence().getNbFiles(), 4)


def setUpMultiSequenceSingleFile():
    global path_bbb
    path_bbb = tempfile.mkdtemp('bbb', dir=root_aaa)
    # create one file
    createFile(path_bbb, 'a1b24.jpg')


@with_setup(setUpMultiSequenceSingleFile, tearDownOneTest)
def testMultiSequenceSingleFile():
    """
    Check sequence detection with a single file.
    """
    listSequence = getSequencesFromPath(path_bbb, seq.eDetectionNegative)
    assert_equals(len(listSequence), 1)
    assert_equals(listSequence[0].getSequence().getFirstTime(), 24)
    assert_equals(listSequence[0].getSequence().getLastTime(), 24)
    assert_equals(listSequence[0].getSequence().getNbFiles(), 1)

    listSequence = getSequencesFromPath(path_bbb, seq.eDetectionNegative | seq.eDetectionSingleFileSeqUseFirstNumber)
    assert_equals(len(listSequence), 1)
    assert_equals(listSequence[0].getSequence().getFirstTime(), 1)
    assert_equals(listSequence[0].getSequence().getLastTime(), 1)
    assert_equals(listSequence[0].getSequence().getNbFiles(), 1)
