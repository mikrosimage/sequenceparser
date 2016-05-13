# How to use sequenceParser

#### Definitions
* Sequence  
It's many files with a common pattern of numbers.
* File  
It's all file without numbers in the filname and sequences with only one file.
* Folders  
Just folders.


#### API
The original code is in C++, but almost all the methods are translated into python/java without any changes.  
So a good way to start is to have a look at the [C++ code](src/sequenceParser) (since the generated code is ugly).  

To simplify the following help, the code is written in python.

###### Interact with the filesystem
The entry points to interact with the filesystem are the [__browse__](src/sequenceParser/filesystem.hpp) methods.
```python
items = sequenceParser.browse("/path/to/browse")
```
These methods return a list of __Item__.  
All the rest of your code will consist in manipulate these __Item__ objects, without any other interaction with the filesystem.

###### Item
All elements in the filesystem (folder, file, link...) are consider as [__Item__](src/sequenceParser/Item.hpp) in the library.  
The __Item__ handles general attributes such as the path, the name, and the type of the element.  
To get the type of an __Item__:
```python
if item.getType() == sequenceParser.eTypeFolder:
    print "This is a folder!"
elif item.getType() == sequenceParser.eTypeFile:
    print "This is a file!"
elif item.getType() == sequenceParser.eTypeSequence:
    print "This is a sequence!"
elif item.getType() == sequenceParser.eTypeLink:
    print "This is a link!"
else
    raise ValueError
```

###### ItemStat
From an __Item__, you can create an [__ItemStat__](src/sequenceParser/ItemStat.hpp) to have access to system info of the element.
```python
itemStat = sequenceParser.ItemStat(item)
```

###### Sequence
If an __Item__ represents several files with a common pattern, it holds a [__Sequence__](src/sequenceParser/Sequence.hpp).
```python
if item.getType() == sequenceParser.eTypeSequence:
    sequence = item.getSequence()
else:
    pass
```

With this object, we have access to all the information we expect about a sequence:
```python
firstTime = sequence.getFirstTime()
lastTime = sequence.getLastTime()
padding = sequence.getMaxPadding()

nbFiles = sequence.getNbFiles()
nbMissingFiles = sequence.getNbMissingFiles() # in case of holes
```

###### Tell me a story about padding...
To manage ambiguous cases of sequence detection, the sequenceParser library makes the difference between a fixed, variable and an unknown padding.

* Fixed padding  
The most common and easiest case:
    * seq.001.jpg
    * seq.002.jpg
    * seq.002.jpg
```python
fixedPadding = sequence.getFixedPadding()  # 3
maxPadding = sequence.getMaxPadding()      # 3
print sequence                             # seq.###.jpg [1:3]
```

* Variable padding
When not all frames have the same padding:
    * seq.101.jpg
    * seq.100.jpg
    * seq.99.jpg
```python
fixedPadding = sequence.getFixedPadding()  # 0
maxPadding = sequence.getMaxPadding()      # 2
print sequence                             # seq.@.jpg [99:101]
```

* Unknown padding  
When no frame begins with a '0' padding character:
    * seq.101.jpg
    * seq.102.jpg
    * seq.103.jpg
```python
fixedPadding = sequence.getFixedPadding()  # 0
maxPadding = sequence.getMaxPadding()      # 3
print sequence                             # seq.@.jpg [101:103]
```

###### Browse more!
When you browse the disks, you have access to several arguments to specify what you are looking for.
* detection options  
[__eDetectionXXX__](src/sequenceParser/common.hpp) is an enum used to choose how to consider sequences.  
* filters  
A list of strings used to limit the search (for example to limit to jpg files, use "*.jpg").

```python
# Get all dpx files inside "/path/to/browse", and detect sequence with the following rules:
#   - at least 2 files to detect a sequence (eDetectionDefault)
#   - ignore dot files (eDetectionDefault)
#   - detect negative numbers (instead of detecting "-" as a non-digit character) (eDetectionNegative)
items = sequenceParser.browse("/path/to/browse", sequenceParser.eDetectionDefault | sequenceParser.eDetectionNegative, "*.dpx")
```

For more information, see the [__python examples__](examples/README.md).

#### Environment
###### In C++
Set environment:
```bash
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/path/to/boost/lib:/path/to/sequenceParser/lib
export PATH=$PATH:/path/to/sequenceParser/bin
```

###### In Java
Add argument to the JVM:
```bash
-Djava.library.path=/path/to/sequenceParser/lib/java
```
Set environment:
```bash
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/path/to/boost/lib
```

###### In Python
Set environment:
```bash
export PYTHONPATH=$PYTHONPATH:/path/to/sequenceParser/lib/python<version>/site-packages/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/path/to/boost/lib
```

#### Projects which uses sequenceParser
###### TuttleOFX
TuttleOFX is a library to connect and batch operations with OpenFx plugins. It comes with a set of plugins that allows you to batch process on movies and file sequences.  
TuttOFX uses sequenceParser in:
* [tuttleIOPluginLib](https://github.com/tuttleofx/TuttleOFX/tree/develop/libraries/tuttle/src/tuttle/ioplugin/context), its common library for io plugins.
* [sam](https://github.com/tuttleofx/TuttleOFX/tree/develop/applications/sam), its command line to process sequences.

###### Dude2
Dude2 is a disk usage monitoring tool.
Dude2 uses sequenceParser in:
* its [incremental scan](https://github.com/mikrosimage/Dude2/tree/develop/FileSystemScanner/src/eu/mikrosimage/filesystem/scanner).
