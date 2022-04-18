# see: https://wiki.debian.org/IntroDebianPackaging
FROM debian:jessie

RUN apt-get update && apt-get -y upgrade
RUN apt-get install -y libc6-dev build-essential tcsh devscripts debhelper 
RUN apt-get clean

# copy the source context into the local image
#  note: make sure .dockerignore is up to date
ADD . /bedops

# populate base debian package tree
RUN mkdir /bedops_2.4.41
WORKDIR /bedops_2.4.41
RUN mkdir -p DEBIAN usr/bin usr/share/doc/bedops
RUN cp /bedops/LICENSE /bedops/README.md usr/share/doc/bedops
RUN cp /bedops/packaging/deb/control DEBIAN

# build and install bedops into debian package tree
WORKDIR /bedops
RUN make -j `nproc` && make install BINDIR=/bedops_2.4.41/usr/bin
WORKDIR /
RUN dpkg-deb --build bedops_2.4.41

# deb file should now be located in / directory
