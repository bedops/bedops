FROM debian:jessie

# install the necessary tooling
RUN apt-get update && apt-get install -y \
     bzip2 \
     g++ \
     git \
     make \
     tcsh

# copy the source context into the local image & build/install
#  note: make sure .dockerignore is up to date
RUN mkdir /bedops
ADD . /bedops
RUN cd /bedops && make -j`nproc` && make install && cp bin/* /usr/local/bin

# clean up to save space
RUN apt-get -y autoremove g++ git make bzip2
RUN apt-get -y clean all
RUN rm -rf /var/lib/apt/* 
RUN rm -rf /bedops

# note: default entrypoint is sh -c
