import os

from pySequenceParser import sequenceParser as seq


def createFile(path, filename, mode=0o664):
    """
    Create an empty file.
    """
    ff = os.path.join(path, filename)
    open(ff, 'w', mode).close()


def createFolder(path, folderName, mode=0o664):
    """
    Create an empty folder.
    """
    dd = os.path.join(path, folderName)
    os.mkdir(dd)
    os.chmod(dd, mode)


def createSymLink(path, target, symLinkName):
    """
    Create a symbolic link.
    """
    src = os.path.join(path, target)
    dst = os.path.join(path, symLinkName)
    os.symlink(src, dst)


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
