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

gprof: default

install: prep_c install_conversion_scripts install_starchcluster_scripts
	-cp ${APPDIR}/sort-bed/bin/sort-bed ${BINDIR}/
	-cp ${APPDIR}/bedops/bin/bedops ${BINDIR}/
	-cp ${APPDIR}/closestfeats/bin/closest-features ${BINDIR}/
	-cp ${APPDIR}/bedmap/bin/bedmap ${BINDIR}/
	-cp ${APPDIR}/bedextract/bin/bedextract ${BINDIR}/
	-cp ${APPDIR}/starch/bin/starch ${BINDIR}/
	-cp ${APPDIR}/starch/bin/unstarch ${BINDIR}/
	-cp ${APPDIR}/starch/bin/starchcat ${BINDIR}/
	-cp ${APPDIR}/conversion/bin/wig2bed_bin ${BINDIR}/




#######################
# install details

prep_c:
	mkdir -p ${BINDIR}

install_debug: prep_c install_conversion_scripts install_starchcluster_scripts
	-cp ${APPDIR}/sort-bed/bin/debug.sort-bed ${BINDIR}/
	-cp ${APPDIR}/bedops/bin/debug.bedops ${BINDIR}/
	-cp ${APPDIR}/closestfeats/bin/debug.closest-features ${BINDIR}/
	-cp ${APPDIR}/bedmap/bin/debug.bedmap ${BINDIR}/
	-cp ${APPDIR}/bedextract/bin/debug.bedextract ${BINDIR}/
	-cp ${APPDIR}/starch/bin/debug.starch ${BINDIR}/
	-cp ${APPDIR}/starch/bin/debug.unstarch ${BINDIR}/
	-cp ${APPDIR}/starch/bin/debug.starchcat ${BINDIR}/
	-cp ${APPDIR}/conversion/bin/debug.wig2bed_bin ${BINDIR}/

install_gprof: prep_c install_conversion_scripts install_starchcluster_scripts
	-cp ${APPDIR}/sort-bed/bin/gprof.sort-bed ${BINDIR}/
	-cp ${APPDIR}/bedops/bin/gprof.bedops ${BINDIR}/
	-cp ${APPDIR}/closestfeats/bin/gprof.closest-features ${BINDIR}/
	-cp ${APPDIR}/bedmap/bin/gprof.bedmap ${BINDIR}/
	-cp ${APPDIR}/bedextract/bin/gprof.bedextract ${BINDIR}/
	-cp ${APPDIR}/starch/bin/gprof.starch ${BINDIR}/
	-cp ${APPDIR}/starch/bin/gprof.unstarch ${BINDIR}/
	-cp ${APPDIR}/starch/bin/gprof.starchcat ${BINDIR}/
	-cp ${APPDIR}/conversion/bin/gprof.wig2bed_bin ${BINDIR}/

install_starchcluster_scripts: prep_c
	-cp ${APPDIR}/starch/bin/starchcluster_sge ${BINDIR}/starchcluster_sge
	-cp ${APPDIR}/starch/bin/starchcluster_gnuParallel ${BINDIR}/starchcluster_gnuparallel

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
	-cp ${APPDIR}/conversion/src/bam2bedcluster_sge.tcsh ${BINDIR}/bam2bedcluster_sge
	-cp ${APPDIR}/conversion/src/bam2bedcluster_gnuParallel.tcsh ${BINDIR}/bam2bedcluster_gnuParallel
	-cp ${APPDIR}/conversion/src/bam2starchcluster_sge.tcsh ${BINDIR}/bam2starchcluster_sge
	-cp ${APPDIR}/conversion/src/bam2starchcluster_gnuParallel.tcsh ${BINDIR}/bam2starchcluster_gnuParallel

install_osx_packaging_bins: prep_c
	mkdir -p ${OSXPKGDIR}
	-cp ${APPDIR}/sort-bed/bin/sort-bed ${OSXPKGDIR}/
	-cp ${APPDIR}/bedops/bin/bedops ${OSXPKGDIR}/
	-cp ${APPDIR}/closestfeats/bin/closest-features ${OSXPKGDIR}/
	-cp ${APPDIR}/bedmap/bin/bedmap ${OSXPKGDIR}/
	-cp ${APPDIR}/bedextract/bin/bedextract ${OSXPKGDIR}/
	-cp ${APPDIR}/starch/bin/starch ${OSXPKGDIR}/
	-cp ${APPDIR}/starch/bin/unstarch ${OSXPKGDIR}/
	-cp ${APPDIR}/starch/bin/starchcat ${OSXPKGDIR}/
	-cp ${APPDIR}/starch/bin/starchcluster_sge ${OSXPKGDIR}/
	-cp ${APPDIR}/starch/bin/starchcluster_gnuParallel ${OSXPKGDIR}/
	-cp ${APPDIR}/conversion/bin/wig2bed_bin ${OSXPKGDIR}/
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
	-cp ${APPDIR}/conversion/src/bam2bedcluster_sge.tcsh ${OSXPKGDIR}/bam2bedcluster_sge
	-cp ${APPDIR}/conversion/src/bam2bedcluster_gnuParallel.tcsh ${OSXPKGDIR}/bam2bedcluster_gnuParallel
	-cp ${APPDIR}/conversion/src/bam2starchcluster_sge.tcsh ${OSXPKGDIR}/bam2starchcluster_sge
	-cp ${APPDIR}/conversion/src/bam2starchcluster_gnuParallel.tcsh ${OSXPKGDIR}/bam2starchcluster_gnuParallel
	mkdir -p ${OSXLIBDIR}
