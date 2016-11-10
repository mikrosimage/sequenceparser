### How to compile with docker
```Shell
docker build -t sequenceparser:centos6 .
docker run -v <path to sequenceparser>:/opt/sequenceparser sequenceparser:centos6 /opt/sequenceparser/docker/centos6/build.sh
```
The compiled libraries is installed into centos6 folder at root folder of sequenceparser repository
