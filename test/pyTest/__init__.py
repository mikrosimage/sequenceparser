import os

from pySequenceParser import sequenceParser as seq


def createFile(path, filename):
    """
    Create an empty file.
    """
    ff = os.path.join(path, filename)
    open(ff, 'w').close()


def getSequencesFromPath(path, detectionOptions, filters=[]):
    """
    Get a list of sequences found thanks to a browse with the given parameters.
    """
    listSequence = []
    items = seq.browse(path, detectionOptions, filters)
    for item in items:
        if item.getType() == seq.eTypeSequence:
            listSequence.append(item)
    return listSequence
