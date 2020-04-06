FROM centos:7

# install the necessary tooling
RUN yum -y install tar \
                   make \
                   gcc \
                   gcc-c++ \
                   rpm-build \
                   glibc-static \
                   libstdc++-static

# copy the source context into the local image & build/install
#  note: make sure .dockerignore is up to date
RUN mkdir /bedops-2.4.39
ADD . /bedops-2.4.39
RUN tar zcf /bedops-2.4.39.tar.gz bedops-2.4.39
RUN rm -rf /bedops-2.4.39
RUN rpmbuild -ta bedops-2.4.39.tar.gz
RUN rm /bedops-2.4.39.tar.gz
