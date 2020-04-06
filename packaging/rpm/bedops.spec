Name:           bedops
Version:        2.4.39
Release:        1
Summary:        A suite of tools to address common questions raised in genomic studies.
Group:          Applications/Productivity
License:        GPLv2
URL:            http://bedops.readthedocs.org/
Source0:        %{name}-%{version}.tar.gz
Requires:       tcsh
BuildRequires:  glibc-static
BuildRequires:  libstdc++-static

%description
BEDOPS is a suite of tools to address common questions raised in genomic studies — mostly with regard to overlap and proximity relationships between data sets. It aims to be scalable and flexible, facilitating the efficient and accurate analysis and management of large-scale genomic data.

%prep
%autosetup

%build
make %{?_smp_mflags}
%install
make install BINDIR=%{buildroot}%_bindir
%clean
make clean

%files
%{_bindir}/*
%doc LICENSE README.md
