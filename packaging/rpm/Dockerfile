FROM centos:7

# install the necessary tooling
<<<<<<< HEAD
RUN yum -y install tar \ 
=======
RUN yum -y install tar \
>>>>>>> upstream/master
                   make \
                   gcc \
                   gcc-c++ \
                   rpm-build \
                   glibc-static \
                   libstdc++-static

# copy the source context into the local image & build/install
#  note: make sure .dockerignore is up to date
<<<<<<< HEAD
RUN mkdir /bedops-2.4.14
ADD . /bedops-2.4.14
RUN tar zcf /bedops-2.4.14.tar.gz bedops-2.4.14
RUN rm -rf /bedops-2.4.14
RUN rpmbuild -ta bedops-2.4.14.tar.gz
RUN rm /bedops-2.4.14.tar.gz

=======
RUN mkdir /bedops-2.4.15
ADD . /bedops-2.4.15
RUN tar zcf /bedops-2.4.15.tar.gz bedops-2.4.15
RUN rm -rf /bedops-2.4.15
RUN rpmbuild -ta bedops-2.4.15.tar.gz
RUN rm /bedops-2.4.15.tar.gz
>>>>>>> upstream/master
