export KERNEL   := ${shell uname -a | cut -f1 -d' '}
PARTY3           = third-party
BOOSTVERSION     = boost_1_46_1
WHICHBOOST      := ${PARTY3}/${BOOSTVERSION}
BZIP2VERSION     = bzip2-1.0.6
WHICHBZIP2      := ${PARTY3}/${BZIP2VERSION}
JANSSONVERSION   = jansson-2.4
WHICHJANSSON    := ${PARTY3}/${JANSSONVERSION}
ZLIBVERSION      = zlib-1.2.7
WHICHZLIB       := ${PARTY3}/${ZLIBVERSION}
APPDIR           = applications/bed
BINDIR           = bin
OSXPKGROOT       = packaging/os_x
OSXBUILDDIR      = ${OSXPKGROOT}/build
OSXPKGDIR        = ${OSXPKGROOT}/resources/bin
OSXLIBDIR        = ${OSXPKGROOT}/resources/lib
THISDIR          = ${shell pwd}

dist: prep_partial_nondarwin_static build_all

dist_gprof: prep_partial_nondarwin_static build_all_gprof

dist_darwin_intel_fat: prep_partial_nondarwin_static build_all_darwin_intel_fat

static: prep_force_static build_all

#
# Generic build
#

build_all: boost_support_c bzip2_support_c jansson_support_c zlib_support_c sort_c bedops_c closestfeatures_c bedmap_c bedextract_c starch_c wig2bed_c

#
# GNU gprof build
#

build_all_gprof: boost_support_c bzip2_support_c jansson_support_c zlib_support_c sort_c_gprof bedops_c_gprof closestfeatures_c_gprof bedmap_c_gprof bedextract_c_gprof starch_c_gprof wig2bed_c_gprof

#
# Darwin fat - 10.6-10.9, i386 (32-bit) and x86-64 (64-bit) support
#

build_all_darwin_intel_fat: boost_support_c sort_c_darwin_intel_fat bedops_c_darwin_intel_fat closestfeatures_c_darwin_intel_fat bedmap_c_darwin_intel_fat bedextract_c_darwin_intel_fat starch_c_darwin_intel_fat wig2bed_c_darwin_intel_fat

#
# Install target
#

install: prep_c install_conversion_scripts install_starchcluster_scripts
	cp ${APPDIR}/sort-bed/bin/sort-bed ${BINDIR}/sort-bed
	cp ${APPDIR}/bedops/bin/bedops ${BINDIR}/bedops
	cp ${APPDIR}/closestfeats/bin/closest-features ${BINDIR}/closest-features
	cp ${APPDIR}/bedmap/bin/bedmap ${BINDIR}/bedmap
	cp ${APPDIR}/bedextract/bin/bedextract ${BINDIR}/bedextract
	cp ${APPDIR}/starch/bin/starch ${BINDIR}/starch
	cp ${APPDIR}/starch/bin/unstarch ${BINDIR}/unstarch
	cp ${APPDIR}/starch/bin/starchcat ${BINDIR}/starchcat
	cp ${APPDIR}/conversion/bin/wig2bed_bin ${BINDIR}/wig2bed_bin

install_gprof: prep_c install_conversion_scripts install_starchcluster_scripts
	cp ${APPDIR}/sort-bed/bin/gprof.sort-bed ${BINDIR}/gprof.sort-bed
	cp ${APPDIR}/bedops/bin/gprof.bedops ${BINDIR}/gprof.bedops
	cp ${APPDIR}/closestfeats/bin/gprof.closest-features ${BINDIR}/gprof.closest-features
	cp ${APPDIR}/bedmap/bin/gprof.bedmap ${BINDIR}/gprof.bedmap
	cp ${APPDIR}/bedextract/bin/gprof.bedextract ${BINDIR}/gprof.bedextract
	cp ${APPDIR}/starch/bin/gprof.starch ${BINDIR}/gprof.starch
	cp ${APPDIR}/starch/bin/gprof.unstarch ${BINDIR}/gprof.unstarch
	cp ${APPDIR}/starch/bin/gprof.starchcat ${BINDIR}/gprof.starchcat
	cp ${APPDIR}/conversion/bin/gprof.wig2bed_bin ${BINDIR}/gprof.wig2bed_bin

install_starchcluster_scripts: prep_c
	cp ${APPDIR}/starch/bin/starchcluster ${BINDIR}/starchcluster
	cp ${APPDIR}/starch/bin/starchcluster.gnu_parallel ${BINDIR}/starchcluster.gnu_parallel

install_conversion_scripts: prep_c
	cp ${APPDIR}/conversion/src/bam2bed.py ${BINDIR}/bam2bed
	cp ${APPDIR}/conversion/src/gff2bed.py ${BINDIR}/gff2bed
	cp ${APPDIR}/conversion/src/gtf2bed.py ${BINDIR}/gtf2bed
	cp ${APPDIR}/conversion/src/psl2bed.py ${BINDIR}/psl2bed
	cp ${APPDIR}/conversion/src/sam2bed.py ${BINDIR}/sam2bed
	cp ${APPDIR}/conversion/src/vcf2bed.py ${BINDIR}/vcf2bed
	cp ${APPDIR}/conversion/src/wig2bed.bash ${BINDIR}/wig2bed
	cp ${APPDIR}/conversion/src/bam2starch.py ${BINDIR}/bam2starch
	cp ${APPDIR}/conversion/src/gff2starch.py ${BINDIR}/gff2starch
	cp ${APPDIR}/conversion/src/gtf2starch.py ${BINDIR}/gtf2starch
	cp ${APPDIR}/conversion/src/psl2starch.py ${BINDIR}/psl2starch
	cp ${APPDIR}/conversion/src/sam2starch.py ${BINDIR}/sam2starch
	cp ${APPDIR}/conversion/src/vcf2starch.py ${BINDIR}/vcf2starch
	cp ${APPDIR}/conversion/src/wig2starch.bash ${BINDIR}/wig2starch

install_osx_packaging_bins: prep_c
	mkdir -p ${OSXPKGDIR}
	cp ${APPDIR}/sort-bed/bin/sort-bed ${OSXPKGDIR}/sort-bed
	cp ${APPDIR}/bedops/bin/bedops ${OSXPKGDIR}/bedops
	cp ${APPDIR}/closestfeats/bin/closest-features ${OSXPKGDIR}/closest-features
	cp ${APPDIR}/bedmap/bin/bedmap ${OSXPKGDIR}/bedmap
	cp ${APPDIR}/bedextract/bin/bedextract ${OSXPKGDIR}/bedextract
	cp ${APPDIR}/starch/bin/starch ${OSXPKGDIR}/starch
	cp ${APPDIR}/starch/bin/unstarch ${OSXPKGDIR}/unstarch
	cp ${APPDIR}/starch/bin/starchcat ${OSXPKGDIR}/starchcat
	cp ${APPDIR}/starch/bin/starchcluster ${OSXPKGDIR}/starchcluster
	cp ${APPDIR}/starch/bin/starchcluster.gnu_parallel ${OSXPKGDIR}/starchcluster.gnu_parallel
	cp ${APPDIR}/conversion/bin/wig2bed_bin ${OSXPKGDIR}/wig2bed_bin
	cp ${APPDIR}/conversion/src/bam2bed.py ${OSXPKGDIR}/bam2bed
	cp ${APPDIR}/conversion/src/gff2bed.py ${OSXPKGDIR}/gff2bed
	cp ${APPDIR}/conversion/src/gtf2bed.py ${OSXPKGDIR}/gtf2bed
	cp ${APPDIR}/conversion/src/psl2bed.py ${OSXPKGDIR}/psl2bed
	cp ${APPDIR}/conversion/src/sam2bed.py ${OSXPKGDIR}/sam2bed
	cp ${APPDIR}/conversion/src/vcf2bed.py ${OSXPKGDIR}/vcf2bed
	cp ${APPDIR}/conversion/src/wig2bed.bash ${OSXPKGDIR}/wig2bed
	cp ${APPDIR}/conversion/src/bam2starch.py ${OSXPKGDIR}/bam2starch
	cp ${APPDIR}/conversion/src/gff2starch.py ${OSXPKGDIR}/gff2starch
	cp ${APPDIR}/conversion/src/gtf2starch.py ${OSXPKGDIR}/gtf2starch
	cp ${APPDIR}/conversion/src/psl2starch.py ${OSXPKGDIR}/psl2starch
	cp ${APPDIR}/conversion/src/sam2starch.py ${OSXPKGDIR}/sam2starch
	cp ${APPDIR}/conversion/src/vcf2starch.py ${OSXPKGDIR}/vcf2starch
	cp ${APPDIR}/conversion/src/wig2starch.bash ${OSXPKGDIR}/wig2starch
	mkdir -p ${OSXLIBDIR}

prep_partial_nondarwin_static:
ifneq (${KERNEL}, Darwin)
	touch ${APPDIR}/sort-bed/src/.forcestatic
	touch ${APPDIR}/bedops/src/.forcestatic
	touch ${APPDIR}/closestfeats/src/.forcestatic
	touch ${APPDIR}/bedmap/src/.forcestatic
	touch ${APPDIR}/conversion/src/wig2bed/.forcestatic
	touch ${APPDIR}/bedextract/src/.forcestatic
endif

prep_force_static:
ifeq (${KERNEL}, Darwin)
	$(error Static builds are not supported on OS X. Please use regular make)
else
	touch ${APPDIR}/sort-bed/src/.forcestatic
	touch ${APPDIR}/bedops/src/.forcestatic
	touch ${APPDIR}/closestfeats/src/.forcestatic
	touch ${APPDIR}/bedmap/src/.forcestatic
	touch ${APPDIR}/conversion/src/wig2bed/.forcestatic
	touch ${APPDIR}/bedextract/src/.forcestatic
	touch ${APPDIR}/starch/src/.forcestatic
endif

clean_force_static:
	rm -rf ${APPDIR}/sort-bed/src/.forcestatic
	rm -rf ${APPDIR}/bedops/src/.forcestatic
	rm -rf ${APPDIR}/closestfeats/src/.forcestatic
	rm -rf ${APPDIR}/bedmap/src/.forcestatic
	rm -rf ${APPDIR}/conversion/src/wig2bed/.forcestatic
	rm -rf ${APPDIR}/bedextract/src/.forcestatic
	rm -rf ${APPDIR}/starch/src/.forcestatic

prep_c:
	mkdir -p ${BINDIR}

#
# third-party libraries
#

boost_support_c:
	test -s ${WHICHBOOST} || { bzcat ${WHICHBOOST}.tar.bz2 | tar -x -C ${PARTY3}; }
	ln -sf ${BOOSTVERSION} ${PARTY3}/boost
BOOSTDONE=1
	export BOOSTDONE

bzip2_support_c:
	test -s ${WHICHBZIP2} || { bzcat ${WHICHBZIP2}.tar.bz2 | tar -x -C ${PARTY3}; }
	ln -sf ${BZIP2VERSION} ${PARTY3}/bzip2
BZIP2DONE=1
	export BZIP2DONE

jansson_support_c: 
	test -s ${WHICHJANSSON} || { bzcat ${WHICHJANSSON}.tar.bz2 | tar -x -C ${PARTY3}; }
	ln -sf ${JANSSONVERSION} ${PARTY3}/jansson
JANSSONDONE=1
	export JANSSONDONE

zlib_support_c:
	test -s ${WHICHZLIB} || { bzcat ${WHICHZLIB}.tar.bz2 | tar -x -C ${PARTY3}; }
	ln -sf ${ZLIBVERSION} ${PARTY3}/zlib
ZLIBDONE=1
	export ZLIBDONE

#
# Generic build targets
#

sort_c:
	make -C ${APPDIR}/sort-bed/src
bedops_c:
	make -C ${APPDIR}/bedops/src
closestfeatures_c:
	make -C ${APPDIR}/closestfeats/src
bedmap_c:
	make -C ${APPDIR}/bedmap/src
bedextract_c:
	make -C ${APPDIR}/bedextract/src
starch_c:
	make -C ${APPDIR}/starch/src
wig2bed_c:
	make -C ${APPDIR}/conversion/src/wig2bed

#
# GNU gprof targets
#

sort_c_gprof:
	make -C ${APPDIR}/sort-bed/src gprof
bedops_c_gprof:
	make -C ${APPDIR}/bedops/src gprof
closestfeatures_c_gprof:
	make -C ${APPDIR}/closestfeats/src gprof
bedmap_c_gprof:
	make -C ${APPDIR}/bedmap/src gprof
bedextract_c_gprof:
	make -C ${APPDIR}/bedextract/src gprof
starch_c_gprof:
	make -C ${APPDIR}/starch/src gprof
wig2bed_c_gprof:
	make -C ${APPDIR}/conversion/src/wig2bed gprof

#
# Darwin fat build targets
#

sort_c_darwin_intel_fat: sort_c_darwin_intel_i386 sort_c_darwin_intel_x86_64
	lipo -create ${APPDIR}/sort-bed/bin/sort-bed_i386 ${APPDIR}/sort-bed/bin/sort-bed_x86_64 -output ${APPDIR}/sort-bed/bin/sort-bed

sort_c_darwin_intel_i386:
	ARCH=i386 CC=clang CXX=clang++ make -C ${APPDIR}/sort-bed/src -f Makefile.darwin

sort_c_darwin_intel_x86_64:
	ARCH=x86_64 CC=clang CXX=clang++ make -C ${APPDIR}/sort-bed/src -f Makefile.darwin

bedops_c_darwin_intel_fat: bedops_c_darwin_intel_i386 bedops_c_darwin_intel_x86_64
	lipo -create ${APPDIR}/bedops/bin/bedops_i386 ${APPDIR}/bedops/bin/bedops_x86_64 -output ${APPDIR}/bedops/bin/bedops

bedops_c_darwin_intel_i386:
	ARCH=i386 CC=clang CXX=clang++ make -C ${APPDIR}/bedops/src -f Makefile.darwin

bedops_c_darwin_intel_x86_64:
	ARCH=x86_64 CC=clang CXX=clang++ make -C ${APPDIR}/bedops/src -f Makefile.darwin

closestfeatures_c_darwin_intel_fat: closestfeatures_c_darwin_intel_i386 closestfeatures_c_darwin_intel_x86_64
	lipo -create ${APPDIR}/closestfeats/bin/closest-features_i386 ${APPDIR}/closestfeats/bin/closest-features_x86_64 -output ${APPDIR}/closestfeats/bin/closest-features

closestfeatures_c_darwin_intel_i386:
	ARCH=i386 CC=clang CXX=clang++ make -C ${APPDIR}/closestfeats/src -f Makefile.darwin

closestfeatures_c_darwin_intel_x86_64:
	ARCH=x86_64 CC=clang CXX=clang++ make -C ${APPDIR}/closestfeats/src -f Makefile.darwin

bedmap_c_darwin_intel_fat: bedmap_c_darwin_intel_i386 bedmap_c_darwin_intel_x86_64
	lipo -create ${APPDIR}/bedmap/bin/bedmap_i386 ${APPDIR}/bedmap/bin/bedmap_x86_64 -output ${APPDIR}/bedmap/bin/bedmap

bedmap_c_darwin_intel_i386:
	ARCH=i386 CC=clang CXX=clang++ make -C ${APPDIR}/bedmap/src -f Makefile.darwin

bedmap_c_darwin_intel_x86_64:
	ARCH=x86_64 CC=clang CXX=clang++ make -C ${APPDIR}/bedmap/src -f Makefile.darwin

bedextract_c_darwin_intel_fat: bedextract_c_darwin_intel_i386 bedextract_c_darwin_intel_x86_64
	lipo -create ${APPDIR}/bedextract/bin/bedextract_i386 ${APPDIR}/bedextract/bin/bedextract_x86_64 -output ${APPDIR}/bedextract/bin/bedextract

bedextract_c_darwin_intel_i386:
	ARCH=i386 CC=clang CXX=clang++ make -C ${APPDIR}/bedextract/src -f Makefile.darwin

bedextract_c_darwin_intel_x86_64:
	ARCH=x86_64 CC=clang CXX=clang++ make -C ${APPDIR}/bedextract/src -f Makefile.darwin

wig2bed_c_darwin_intel_fat: wig2bed_c_darwin_intel_i386 wig2bed_c_darwin_intel_x86_64
	lipo -create ${APPDIR}/conversion/bin/wig2bed_bin_i386 ${APPDIR}/conversion/bin/wig2bed_bin_x86_64 -output ${APPDIR}/conversion/bin/wig2bed_bin

wig2bed_c_darwin_intel_i386:
	ARCH=i386 CC=clang CXX=clang++ make -C ${APPDIR}/conversion/src/wig2bed -f Makefile.darwin

wig2bed_c_darwin_intel_x86_64:
	ARCH=x86_64 CC=clang CXX=clang++ make -C ${APPDIR}/conversion/src/wig2bed -f Makefile.darwin

starch_c_darwin_intel_fat: starchcluster starch_c_darwin_intel_i386 starch_c_darwin_intel_x86_64
	lipo -create ${APPDIR}/starch/bin/starch_i386 ${APPDIR}/starch/bin/starch_x86_64 -output ${APPDIR}/starch/bin/starch
	lipo -create ${APPDIR}/starch/bin/unstarch_i386 ${APPDIR}/starch/bin/unstarch_x86_64 -output ${APPDIR}/starch/bin/unstarch
	lipo -create ${APPDIR}/starch/bin/starchcat_i386 ${APPDIR}/starch/bin/starchcat_x86_64 -output ${APPDIR}/starch/bin/starchcat

starch_c_darwin_intel_i386: starchcluster
	ARCH=i386 CC=clang CXX=clang++ make -C ${APPDIR}/starch/src -f Makefile.darwin

starch_c_darwin_intel_x86_64: starchcluster
	ARCH=x86_64 CC=clang CXX=clang++ make -C ${APPDIR}/starch/src -f Makefile.darwin

starchcluster:
	mkdir -p ${APPDIR}/starch/bin
	cp ${APPDIR}/starch/src/starchcluster ${APPDIR}/starch/bin/starchcluster
	cp ${APPDIR}/starch/src/starchcluster.gnu_parallel  ${APPDIR}/starch/bin/starchcluster.gnu_parallel 

clean: clean_force_static
	rm -f ${BINDIR}/sort-bed
	rm -f ${BINDIR}/bedops
	rm -f ${BINDIR}/closest-features
	rm -f ${BINDIR}/bedmap
	rm -f ${BINDIR}/bedextract
	rm -f ${BINDIR}/starch
	rm -f ${BINDIR}/unstarch
	rm -f ${BINDIR}/starchcat
	rm -f ${BINDIR}/starchcluster
	rm -f ${BINDIR}/starchcluster.gnu_parallel
	rm -f ${BINDIR}/bam2bed
	rm -f ${BINDIR}/gff2bed
	rm -f ${BINDIR}/gtf2bed
	rm -f ${BINDIR}/psl2bed
	rm -f ${BINDIR}/sam2bed
	rm -f ${BINDIR}/vcf2bed
	rm -f ${BINDIR}/wig2bed
	rm -f ${BINDIR}/wig2bed_bin
	rm -f ${BINDIR}/bam2starch
	rm -f ${BINDIR}/gff2starch
	rm -f ${BINDIR}/gtf2starch
	rm -f ${BINDIR}/psl2starch
	rm -f ${BINDIR}/sam2starch
	rm -f ${BINDIR}/vcf2starch
	rm -f ${BINDIR}/wig2starch
	rm -f ${OSXPKGDIR}/*
	rm -f ${OSXLIBDIR}/*
	rm -Rf ${OSXBUILDDIR}/*

very_clean: clean clean_force_static
	rm -f ${APPDIR}/sort-bed/bin/*
	rm -f ${APPDIR}/bedops/bin/*
	rm -f ${APPDIR}/closestfeats/bin/*
	rm -f ${APPDIR}/bedmap/bin/*
	rm -f ${APPDIR}/bedextract/bin/*
	rm -f ${APPDIR}/conversion/bin/*
	rm -f ${APPDIR}/starch/bin/*
	rm -rf ${APPDIR}/sort-bed/src/objects
	rm -rf ${APPDIR}/bedops/src/objects
	rm -rf ${APPDIR}/closestfeats/src/objects
	rm -rf ${APPDIR}/bedmap/src/objects
	rm -rf ${APPDIR}/bedextract/src/objects
	rm -rf ${APPDIR}/starch/src/objects
	rm -rf ${APPDIR}/starch/src/objects
	rm -rf ${APPDIR}/starch/src/objects
	rm -rf ${APPDIR}/starch/src/objects
	rm -rf ${APPDIR}/starch/lib
	rm -rf ${BINDIR}
	rm -rf ${WHICHBOOST}
	rm -f ${PARTY3}/boost
	rm -rf ${WHICHBZIP2}
	rm -f ${PARTY3}/bzip2
	rm -rf ${WHICHJANSSON}
	rm -f ${PARTY3}/jansson
	rm -rf ${WHICHZLIB}
	rm -f ${PARTY3}/zlib
	rm -f ${OSXPKGDIR}/*
	rm -f ${OSXLIBDIR}/*
	rm -Rf ${OSXBUILDDIR}/*
