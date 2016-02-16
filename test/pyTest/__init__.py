import os


def createFile(path, filename):
    """
    Create an empty file.
    """
    ff = os.path.join(path, filename)
    open(ff, 'w').close()
