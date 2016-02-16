import tempfile
import shutil

from pySequenceParser import sequenceParser as seq
from . import createFile

from nose.tools import *

root_tmpTestSequence = ''
path_root = ''

path_trash = ''
path_dpx = ''
path_negative = ''

path_film = ''
path_noStrictPadding = ''
path_strictPadding = ''


def setUp():
    """
    Create temporary folders and sequences.
    """
    global root_tmpTestSequence
    global path_root

    global path_trash
    global path_dpx
    global path_negative

    global path_film
    global path_noStrictPadding
    global path_strictPadding

    root_tmpTestSequence = tempfile.mkdtemp('tmpTestSequence')
    path_root = tempfile.mkdtemp('root', dir=root_tmpTestSequence)

    path_film = tempfile.mkdtemp('film', dir=path_root)
    path_strictPadding = tempfile.mkdtemp('strictPadding', dir=path_film)
    # create sequence img.%04d.dpx
    for i in range(0, 100):
        padding = '%04d' % i
        createFile(path_strictPadding, 'img.' + str(padding) + '.dpx')
    # create sequence .img.%04d.dpx
    for i in range(0, 100):
        padding = '%04d' % i
        createFile(path_strictPadding, '.img.' + str(padding) + '.dpx')
    # create sequence img.%04d.jpg
    for i in range(0, 100):
        padding = '%04d' % i
        createFile(path_strictPadding, 'img.' + str(padding) + '.jpg')
    # create sequence imgBroken.%04d.jpg with holes
    for i in range(0, 100):
        padding = '%04d' % i
        createFile(path_strictPadding, 'imgBroken.' + str(padding) + '.jpg')
        if i == 2: i = 9
        if i == 55: i = 99

    path_noStrictPadding = tempfile.mkdtemp('noStrictPadding', dir=path_film)
    # create sequence img.%04d.dpx
    for i in range(0, 100):
        padding = '%04d' % i
        createFile(path_noStrictPadding, 'img.' + str(padding) + '.dpx')
    # create sequence .img.%04d.dpx
    for i in range(0, 100):
        padding = '%04d' % i
        createFile(path_noStrictPadding, '.img.' + str(padding) + '.dpx')
    # create sequence img.%04d.jpg
    for i in range(0, 100):
        padding = '%04d' % i
        createFile(path_noStrictPadding, 'img.' + str(padding) + '.jpg')
    # create sequence imgBroken.%04d.jpg with holes
    for i in range(0, 100):
        padding = '%04d' % i
        createFile(path_noStrictPadding, 'imgBroken.' + str(padding) + '.jpg')
        if i == 2: i = 9
        if i == 55: i = 99

    # create file plop.txt
    path_trash = tempfile.mkdtemp('trash', dir=path_root)
    createFile(path_trash, 'plop.txt')

    # create file img.dpx
    path_dpx = tempfile.mkdtemp('dpx', dir=path_trash)
    createFile(path_dpx, 'img.dpx')

    path_negative = tempfile.mkdtemp('negative', dir=path_dpx)
    # create sequence img.-%04d.dpx
    for i in range(0, 100):
        padding = '%04d' % i
        createFile(path_negative, 'img.-' + str(padding) + '.dpx')
    # create sequence seqTest.-%04d.dpx
    for i in range(0, 100):
        padding = '%04d' % i
        createFile(path_negative, 'seqTest.-' + str(padding) + '.dpx')


def tearDown():
    """
    Remove temporary folders and files.
    """
    global root_tmpTestSequence
    shutil.rmtree(root_tmpTestSequence)


def checkItemsInDirectory(path, detectionOptions, nbFolders, nbFiles, nbSequences, nbFileObjects, filters=[]):
    """
    Browse a folder with the given parameters.
    Check the number of objects of each types found.
    """
    listFileObject = []
    listFolder = []
    listFile = []
    listSequence = []

    items = seq.browse(path, detectionOptions, filters)
    for item in items:
        itemType = item.getType()
        if itemType | seq.eTypeAll:
            listFileObject.append(item)
        if itemType == seq.eTypeFolder:
            listFolder.append(item)
        if itemType == seq.eTypeFile:
            listFile.append(item)
        if itemType == seq.eTypeSequence:
            listSequence.append(item)

    assert_equals(len(listFileObject), nbFileObjects)
    assert_equals(len(listFolder), nbFolders)
    assert_equals(len(listFile), nbFiles)
    assert_equals(len(listSequence), nbSequences)


def checkFirstSequence(path, detectionOptions, minValue, maxValue, filters):
    """
    Browse a folder with the given parameters.
    Check if at least one sequence is found.
    Check the first and last frame of this sequence.
    """
    listSequence = []

    items = seq.browse(path, detectionOptions, filters)
    for item in items:
        if item.getType() == seq.eTypeSequence:
            listSequence.append(item)

    assert_greater(len(listSequence), 0)

    # first sequence
    sequence = listSequence[0].getSequence()
    assert_equals(sequence.getFirstTime(), minValue)
    assert_equals(sequence.getLastTime(), maxValue)


def testFolder():
    """
    Check detection of folders.
    """
    # check folder
    checkItemsInDirectory(root_tmpTestSequence, seq.eDetectionDefault, 1, 0, 0, 1)
    checkItemsInDirectory(path_root, seq.eDetectionDefault, 2, 0, 0, 2)
    checkItemsInDirectory(path_trash, seq.eDetectionDefault, 1, 1, 0, 2)
    checkItemsInDirectory(path_dpx, seq.eDetectionDefault, 1, 1, 0, 2)
    checkItemsInDirectory(path_negative, seq.eDetectionDefault, 0, 0, 2, 2)

def testNegativeSequences():
    """
    Check detection of negative sequences.
    """
    checkItemsInDirectory(path_negative, seq.eDetectionDefault, 0, 0, 0, 0, ['img.####.dpx'])
    checkItemsInDirectory(path_negative, seq.eDetectionDefault, 0, 0, 0, 0, ['img.#####.dpx'])
    checkItemsInDirectory(path_negative, seq.eDetectionDefault, 0, 0, 1, 1, ['img.-####.dpx'])
    checkFirstSequence(path_negative, seq.eDetectionDefault, 0, 99, ['img.-####.dpx'])
    checkItemsInDirectory(path_negative, seq.eDetectionDefault, 0, 0, 0, 0, ['seqTest.@.dpx'])
    checkItemsInDirectory(path_negative, seq.eDetectionDefault, 0, 0, 1, 1, ['seqTest.-@.dpx'])
    checkFirstSequence(path_negative, seq.eDetectionDefault, 0, 99, ['seqTest.-@.dpx'])

    checkItemsInDirectory(path_negative, seq.eDetectionNegative | seq.eDetectionSequenceNeedAtLeastTwoFiles, 0, 0, 2, 2)

    checkItemsInDirectory(path_negative, seq.eDetectionNegative, 0, 0, 1, 1, ['seqTest.@.dpx'])
    checkFirstSequence(path_negative, seq.eDetectionNegative, -99, -0, ['seqTest.@.dpx'])
    
    checkItemsInDirectory(path_negative, seq.eDetectionNegative, 0, 0, 1, 1, ['seqTest.-@.dpx'])
    checkFirstSequence(path_negative, seq.eDetectionNegative, -99, -0, ['seqTest.-@.dpx'])

    checkItemsInDirectory(path_negative, seq.eDetectionNegative, 0, 0, 0, 0, ['img.#### .dpx'])
    checkItemsInDirectory(path_negative, seq.eDetectionNegative, 0, 0, 1, 1, ['img.####.dpx'])
    checkItemsInDirectory(path_negative, seq.eDetectionNegative, 0, 0, 0, 0, ['img.#####.dpx'])
    checkItemsInDirectory(path_negative, seq.eDetectionNegative, 0, 0, 1, 1, ['img.-####.dpx'])
    checkFirstSequence(path_negative, seq.eDetectionNegative, -99, 0, ['img.-####.dpx'])

def testStrictPadding():
    """
    Check detection sequences of strict padding.
    """
    checkItemsInDirectory(path_strictPadding, seq.eDetectionDefault, 0, 0, 3, 3)
    checkItemsInDirectory(path_strictPadding, seq.eDetectionNone, 0, 0, 4, 4)

    checkItemsInDirectory(path_strictPadding, seq.eDetectionDefault, 0, 0, 1, 1, ['img.####.dpx'])
#    checkItemsInDirectory(path_strictPadding, seq.eDetectionDefault, 0, 1, 0, 1, ['img.0050.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionDefault, 0, 0, 1, 1, ['*.0050.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionDefault, 0, 0, 1, 1, ['*0050.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionDefault, 0, 0, 1, 1, ['???.0050.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionDefault, 0, 0, 1, 1, ['????0050.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionDefault, 0, 0, 1, 1, ['*.####.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionDefault, 0, 0, 1, 1, ['*.@.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionDefault, 0, 0, 1, 1, ['???.####.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionDefault, 0, 0, 1, 1, ['???.@.dpx'])

    checkItemsInDirectory(path_strictPadding, seq.eDetectionSequenceNeedAtLeastTwoFiles | seq.eDetectionIgnoreDotFile, 0, 1, 0, 1, ['*.0050.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionSequenceNeedAtLeastTwoFiles | seq.eDetectionIgnoreDotFile, 0, 1, 0, 1, ['*0050.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionSequenceNeedAtLeastTwoFiles, 0, 1, 0, 1, ['???.0050.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionSequenceNeedAtLeastTwoFiles, 0, 1, 0, 1, ['????0050.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionSequenceNeedAtLeastTwoFiles | seq.eDetectionIgnoreDotFile, 0, 0, 1, 1, ['*.####.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionSequenceNeedAtLeastTwoFiles | seq.eDetectionIgnoreDotFile, 0, 0, 1, 1, ['*.@.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionSequenceNeedAtLeastTwoFiles, 0, 0, 1, 1, ['???.####.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionSequenceNeedAtLeastTwoFiles, 0, 0, 1, 1, ['???.@.dpx'])

#    checkItemsInDirectory(path_strictPadding, seq.eDetectionNone, 0, 1, 0, 1, ['img.0050.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionDefault, 0, 0, 0, 0, ['img.50.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionNone, 0, 0, 0, 0, ['img.50.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionDefault, 0, 0, 1, 1, ['img.0500.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionNone, 0, 0, 0, 0, ['img.0500.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionDefault, 0, 0, 0, 0, ['img.000050.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionNone, 0, 0, 0, 0, ['img.000050.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionDefault, 0, 0, 1, 1, ['img.####.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionNone, 0, 0, 1, 1, ['img.####.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionDefault, 0, 0, 0, 0, ['img.###.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionNone, 0, 0, 0, 0, ['img.###.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionDefault, 0, 0, 0, 0, ['img.#####.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionNone, 0, 0, 0, 0, ['img.#####.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionDefault, 0, 0, 1, 1, ['img.%04d.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionNone, 0, 0, 1, 1, ['img.%04d.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionDefault, 0, 0, 0, 0, ['img.%03d.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionNone, 0, 0, 0, 0, ['img.%03d.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionDefault, 0, 0, 0, 0, ['img.%05d.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionNone, 0, 0, 0, 0, ['img.%05d.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionDefault, 0, 0, 1, 1, ['img.@.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionNone, 0, 0, 1, 1, ['img.@.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionDefault, 0, 0, 0, 0, ['.img.0050.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionNone, 0, 0, 1, 1, ['.img.0050.dpx']) # return sequence

    checkItemsInDirectory(path_strictPadding, seq.eDetectionDefault, 0, 0, 0, 0, ['.img.50.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionNone, 0, 0, 0, 0, ['.img.50.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionDefault, 0, 0, 0, 0, ['.img.0500.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionNone, 0, 0, 0, 0, ['.img.0500.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionDefault, 0, 0, 0, 0, ['.img.000050.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionNone, 0, 0, 0, 0, ['.img.000050.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionDefault, 0, 0, 0, 0, ['.img.####.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionNone, 0, 0, 1, 1, ['.img.####.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionDefault, 0, 0, 0, 0, ['.img.###.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionNone, 0, 0, 0, 0, ['.img.###.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionDefault, 0, 0, 0, 0, ['.img.#####.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionNone, 0, 0, 0, 0, ['.img.#####.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionDefault, 0, 0, 0, 0, ['.img.%04d.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionNone, 0, 0, 1, 1, ['.img.%04d.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionDefault, 0, 0, 0, 0, ['.img.%03d.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionNone, 0, 0, 0, 0, ['.img.%03d.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionDefault, 0, 0, 0, 0, ['.img.%05d.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionNone, 0, 0, 0, 0, ['.img.%05d.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionDefault, 0, 0, 0, 0, ['.img.@.dpx'])
    checkItemsInDirectory(path_strictPadding, seq.eDetectionNone, 0, 0, 1, 1, ['.img.@.dpx'])

def testNoStrictPadding():
    """
    Check detection sequences of no strict padding.
    """
    checkItemsInDirectory(path_noStrictPadding, seq.eDetectionDefault, 0, 0, 3, 3)
    checkItemsInDirectory(path_noStrictPadding, seq.eDetectionNone, 0, 0, 4, 4)

    checkItemsInDirectory(path_noStrictPadding, seq.eDetectionDefault, 0, 0, 1, 1, ['img.0050.dpx'])
    checkItemsInDirectory(path_noStrictPadding, seq.eDetectionNone, 0, 0, 1, 1, ['img.0050.dpx'])
    checkItemsInDirectory(path_noStrictPadding, seq.eDetectionDefault, 0, 0, 0, 0, ['img.50.dpx'])
    checkItemsInDirectory(path_noStrictPadding, seq.eDetectionNone, 0, 0, 0, 0, ['img.50.dpx'])
#    checkItemsInDirectory(path_noStrictPadding, seq.eDetectionDefault, 0, 0, 0, 0, ['img.0500.dpx'])
#    checkItemsInDirectory(path_noStrictPadding, seq.eDetectionNone, 0, 0, 0, 0, ['img.0500.dpx'])
    checkItemsInDirectory(path_noStrictPadding, seq.eDetectionDefault, 0, 0, 1, 1, ['img.0050.dpx'])
    checkItemsInDirectory(path_noStrictPadding, seq.eDetectionNone, 0, 0, 1, 1, ['img.0050.dpx'])
    checkItemsInDirectory(path_noStrictPadding, seq.eDetectionDefault, 0, 0, 1, 1, ['img.####.dpx'])
    checkItemsInDirectory(path_noStrictPadding, seq.eDetectionNone, 0, 0, 1, 1, ['img.####.dpx'])
    checkItemsInDirectory(path_noStrictPadding, seq.eDetectionDefault, 0, 0, 0, 0, ['img.###.dpx'])
    checkItemsInDirectory(path_noStrictPadding, seq.eDetectionNone, 0, 0, 0, 0, ['img.###.dpx'])
    checkItemsInDirectory(path_noStrictPadding, seq.eDetectionDefault, 0, 0, 0, 0, ['img.#####.dpx'])
    checkItemsInDirectory(path_noStrictPadding, seq.eDetectionNone, 0, 0, 0, 0, ['img.#####.dpx'])
    checkItemsInDirectory(path_noStrictPadding, seq.eDetectionDefault, 0, 0, 1, 1, ['img.%04d.dpx'])
    checkItemsInDirectory(path_noStrictPadding, seq.eDetectionNone, 0, 0, 1, 1, ['img.%04d.dpx'])
    checkItemsInDirectory(path_noStrictPadding, seq.eDetectionDefault, 0, 0, 0, 0, ['img.%05d.dpx'])
    checkItemsInDirectory(path_noStrictPadding, seq.eDetectionNone, 0, 0, 0, 0, ['img.%05d.dpx'])
    checkItemsInDirectory(path_noStrictPadding, seq.eDetectionDefault, 0, 0, 0, 0, ['img.%03d.dpx'])
    checkItemsInDirectory(path_noStrictPadding, seq.eDetectionNone, 0, 0, 0, 0, ['img.%03d.dpx'])
    checkItemsInDirectory(path_noStrictPadding, seq.eDetectionDefault, 0, 0, 1, 1, ['img.@.dpx'])
    checkItemsInDirectory(path_noStrictPadding, seq.eDetectionNone, 0, 0, 1, 1, ['img.@.dpx'])

    checkItemsInDirectory(path_noStrictPadding, seq.eDetectionDefault, 0, 0, 0, 0, ['.img.0050.dpx'])
    checkItemsInDirectory(path_noStrictPadding, seq.eDetectionNone, 0, 0, 1, 1, ['.img.0050.dpx'])
    checkItemsInDirectory(path_noStrictPadding, seq.eDetectionDefault, 0, 0, 0, 0, ['.img.50.dpx'])
    checkItemsInDirectory(path_noStrictPadding, seq.eDetectionNone, 0, 0, 0, 0, ['.img.50.dpx'])
    checkItemsInDirectory(path_noStrictPadding, seq.eDetectionDefault, 0, 0, 0, 0, ['.img.0500.dpx'])
    checkItemsInDirectory(path_noStrictPadding, seq.eDetectionNone, 0, 0, 0, 0, ['.img.0500.dpx'])
    checkItemsInDirectory(path_noStrictPadding, seq.eDetectionDefault, 0, 0, 0, 0, ['.img.0050.dpx'])
    checkItemsInDirectory(path_noStrictPadding, seq.eDetectionNone, 0, 0, 1, 1, ['.img.0050.dpx'])
    checkItemsInDirectory(path_noStrictPadding, seq.eDetectionDefault, 0, 0, 0, 0, ['.img.####.dpx'])
    checkItemsInDirectory(path_noStrictPadding, seq.eDetectionNone, 0, 0, 1, 1, ['.img.####.dpx'])
    checkItemsInDirectory(path_noStrictPadding, seq.eDetectionDefault, 0, 0, 0, 0, ['.img.###.dpx'])
    checkItemsInDirectory(path_noStrictPadding, seq.eDetectionNone, 0, 0, 0, 0, ['.img.###.dpx'])
    checkItemsInDirectory(path_noStrictPadding, seq.eDetectionDefault, 0, 0, 0, 0, ['.img.#####.dpx'])
    checkItemsInDirectory(path_noStrictPadding, seq.eDetectionNone, 0, 0, 0, 0, ['.img.#####.dpx'])
    checkItemsInDirectory(path_noStrictPadding, seq.eDetectionDefault, 0, 0, 0, 0, ['.img.%04d.dpx'])
    checkItemsInDirectory(path_noStrictPadding, seq.eDetectionNone, 0, 0, 1, 1, ['.img.%04d.dpx'])
    checkItemsInDirectory(path_noStrictPadding, seq.eDetectionDefault, 0, 0, 0, 0, ['.img.%05d.dpx'])
    checkItemsInDirectory(path_noStrictPadding, seq.eDetectionNone, 0, 0, 0, 0, ['.img.%05d.dpx'])
    checkItemsInDirectory(path_noStrictPadding, seq.eDetectionDefault, 0, 0, 0, 0, ['.img.%03d.dpx'])
    checkItemsInDirectory(path_noStrictPadding, seq.eDetectionNone, 0, 0, 0, 0, ['.img.%03d.dpx'])
    checkItemsInDirectory(path_noStrictPadding, seq.eDetectionDefault, 0, 0, 0, 0, ['.img.@.dpx'])
    checkItemsInDirectory(path_noStrictPadding, seq.eDetectionNone, 0, 0, 1, 1, ['.img.@.dpx'])
