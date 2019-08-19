FROM centos:7

#Install dependencies
RUN yum install -y wget cmake tar gcc gcc-c++ pcre-devel python-devel make zlib-devel bzip2-devel

#Install boost
RUN cd /opt/ && \ 
    wget https://sourceforge.net/projects/boost/files/boost/1.55.0/boost_1_55_0.tar.gz && \
    tar xzf boost_1_55_0.tar.gz && \
    cd boost_1_55_0 && \
    ./bootstrap.sh && \
    ./b2 install --with=all && \
    rm -rf boost_1_55_0.tar.gz && \
    rm -rf boost_1_55_0.tar
     


#Install JAVA
RUN yum install -y java-1.8.0-openjdk java-1.8.0-openjdk-devel

#Install Swig 2.0.11

RUN cd /opt/ && \
    wget -O swig-2.0.11.tar.gz https://sourceforge.net/projects/swig/files/swig/swig-2.0.11/swig-2.0.11.tar.gz/download && \
    tar xzf swig-2.0.11.tar.gz && \
    cd swig-2.0.11 && \
    ./configure && \
    make && make install && \
    cd /opt/ && rm -rf swig-2.0.11*
