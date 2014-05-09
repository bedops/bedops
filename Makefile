export KERNEL   := ${shell uname -a | cut -f1 -d' '}
APPDIR           = applications/bed
BINDIR           = bin
OSXPKGROOT       = packaging/os_x
OSXBUILDDIR      = ${OSXPKGROOT}/build
OSXPKGDIR        = ${OSXPKGROOT}/resources/bin
OSXLIBDIR        = ${OSXPKGROOT}/resources/lib


default:
ifeq ($(KERNEL), Darwin)
	$(MAKE) $(MAKECMDGOALS) -f system.mk/Makefile.darwin
else
	$(MAKE) $(MAKECMDGOALS) -f system.mk/Makefile.linux
endif

clean: default

support: default

debug: default

install: prep_c install_conversion_scripts install_starchcluster_scripts
	-cp ${APPDIR}/sort-bed/bin/sort-bed ${BINDIR}/sort-bed
	-cp ${APPDIR}/bedops/bin/bedops ${BINDIR}/bedops
	-cp ${APPDIR}/closestfeats/bin/closest-features ${BINDIR}/closest-features
	-cp ${APPDIR}/bedmap/bin/bedmap ${BINDIR}/bedmap
	-cp ${APPDIR}/bedextract/bin/bedextract ${BINDIR}/bedextract
	-cp ${APPDIR}/starch/bin/starch ${BINDIR}/starch
	-cp ${APPDIR}/starch/bin/unstarch ${BINDIR}/unstarch
	-cp ${APPDIR}/starch/bin/starchcat ${BINDIR}/starchcat
	-cp ${APPDIR}/conversion/bin/wig2bed_bin ${BINDIR}/wig2bed_bin




#######################
# install details

prep_c:
	mkdir -p ${BINDIR}

install_gprof: prep_c install_conversion_scripts install_starchcluster_scripts
	-cp ${APPDIR}/sort-bed/bin/gprof.sort-bed ${BINDIR}/gprof.sort-bed
	-cp ${APPDIR}/bedops/bin/gprof.bedops ${BINDIR}/gprof.bedops
	-cp ${APPDIR}/closestfeats/bin/gprof.closest-features ${BINDIR}/gprof.closest-features
	-cp ${APPDIR}/bedmap/bin/gprof.bedmap ${BINDIR}/gprof.bedmap
	-cp ${APPDIR}/bedextract/bin/gprof.bedextract ${BINDIR}/gprof.bedextract
	-cp ${APPDIR}/starch/bin/gprof.starch ${BINDIR}/gprof.starch
	-cp ${APPDIR}/starch/bin/gprof.unstarch ${BINDIR}/gprof.unstarch
	-cp ${APPDIR}/starch/bin/gprof.starchcat ${BINDIR}/gprof.starchcat
	-cp ${APPDIR}/conversion/bin/gprof.wig2bed_bin ${BINDIR}/gprof.wig2bed_bin

install_starchcluster_scripts: prep_c
	-cp ${APPDIR}/starch/bin/starchcluster ${BINDIR}/starchcluster
	-cp ${APPDIR}/starch/bin/starchcluster.gnu_parallel ${BINDIR}/starchcluster.gnu_parallel

install_conversion_scripts: prep_c
	-cp ${APPDIR}/conversion/src/bam2bed.py ${BINDIR}/bam2bed
	-cp ${APPDIR}/conversion/src/gff2bed.py ${BINDIR}/gff2bed
	-cp ${APPDIR}/conversion/src/gtf2bed.py ${BINDIR}/gtf2bed
	-cp ${APPDIR}/conversion/src/psl2bed.py ${BINDIR}/psl2bed
	-cp ${APPDIR}/conversion/src/sam2bed.py ${BINDIR}/sam2bed
	-cp ${APPDIR}/conversion/src/vcf2bed.py ${BINDIR}/vcf2bed
	-cp ${APPDIR}/conversion/src/wig2bed.bash ${BINDIR}/wig2bed
	-cp ${APPDIR}/conversion/src/bam2starch.py ${BINDIR}/bam2starch
	-cp ${APPDIR}/conversion/src/gff2starch.py ${BINDIR}/gff2starch
	-cp ${APPDIR}/conversion/src/gtf2starch.py ${BINDIR}/gtf2starch
	-cp ${APPDIR}/conversion/src/psl2starch.py ${BINDIR}/psl2starch
	-cp ${APPDIR}/conversion/src/sam2starch.py ${BINDIR}/sam2starch
	-cp ${APPDIR}/conversion/src/vcf2starch.py ${BINDIR}/vcf2starch
	-cp ${APPDIR}/conversion/src/wig2starch.bash ${BINDIR}/wig2starch
	-cp ${APPDIR}/conversion/src/bam2starchcluster.tcsh ${BINDIR}/bam2starchcluster

install_osx_packaging_bins: prep_c
	mkdir -p ${OSXPKGDIR}
	-cp ${APPDIR}/sort-bed/bin/sort-bed ${OSXPKGDIR}/sort-bed
	-cp ${APPDIR}/bedops/bin/bedops ${OSXPKGDIR}/bedops
	-cp ${APPDIR}/closestfeats/bin/closest-features ${OSXPKGDIR}/closest-features
	-cp ${APPDIR}/bedmap/bin/bedmap ${OSXPKGDIR}/bedmap
	-cp ${APPDIR}/bedextract/bin/bedextract ${OSXPKGDIR}/bedextract
	-cp ${APPDIR}/starch/bin/starch ${OSXPKGDIR}/starch
	-cp ${APPDIR}/starch/bin/unstarch ${OSXPKGDIR}/unstarch
	-cp ${APPDIR}/starch/bin/starchcat ${OSXPKGDIR}/starchcat
	-cp ${APPDIR}/starch/bin/starchcluster ${OSXPKGDIR}/starchcluster
	-cp ${APPDIR}/starch/bin/starchcluster.gnu_parallel ${OSXPKGDIR}/starchcluster.gnu_parallel
	-cp ${APPDIR}/conversion/bin/wig2bed_bin ${OSXPKGDIR}/wig2bed_bin
	-cp ${APPDIR}/conversion/src/bam2bed.py ${OSXPKGDIR}/bam2bed
	-cp ${APPDIR}/conversion/src/gff2bed.py ${OSXPKGDIR}/gff2bed
	-cp ${APPDIR}/conversion/src/gtf2bed.py ${OSXPKGDIR}/gtf2bed
	-cp ${APPDIR}/conversion/src/psl2bed.py ${OSXPKGDIR}/psl2bed
	-cp ${APPDIR}/conversion/src/sam2bed.py ${OSXPKGDIR}/sam2bed
	-cp ${APPDIR}/conversion/src/vcf2bed.py ${OSXPKGDIR}/vcf2bed
	-cp ${APPDIR}/conversion/src/wig2bed.bash ${OSXPKGDIR}/wig2bed
	-cp ${APPDIR}/conversion/src/bam2starch.py ${OSXPKGDIR}/bam2starch
	-cp ${APPDIR}/conversion/src/gff2starch.py ${OSXPKGDIR}/gff2starch
	-cp ${APPDIR}/conversion/src/gtf2starch.py ${OSXPKGDIR}/gtf2starch
	-cp ${APPDIR}/conversion/src/psl2starch.py ${OSXPKGDIR}/psl2starch
	-cp ${APPDIR}/conversion/src/sam2starch.py ${OSXPKGDIR}/sam2starch
	-cp ${APPDIR}/conversion/src/vcf2starch.py ${OSXPKGDIR}/vcf2starch
	-cp ${APPDIR}/conversion/src/wig2starch.bash ${OSXPKGDIR}/wig2starch
	-cp ${APPDIR}/conversion/src/bam2starchcluster.tcsh ${OSXPKGDIR}/bam2starchcluster
	mkdir -p ${OSXLIBDIR}
