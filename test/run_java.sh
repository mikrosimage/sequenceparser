

# export LD_LIBRARY_PATH=

export DISTPATH=../dist/`hostname`/gcc-`gcc -dumpversion`/production/java

javac -cp $DISTPATH/sequenceParser.jar StatLibraryTestMain.java 
java -Djava.library.path="$DISTPATH" -cp "$DISTPATH/sequenceParser.jar:." StatLibraryTestMain $1

