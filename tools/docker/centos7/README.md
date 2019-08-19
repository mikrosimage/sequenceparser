### How to compile with docker
```Shell
docker build -t sequenceparser:centos7 .
docker run -v <path to sequenceparser>:/opt/sequenceparser sequenceparser:centos7 /opt/sequenceparser/tools/docker/centos7/build.sh
```
The compiled libraries is installed into centos6 folder at root folder of sequenceparser repository
