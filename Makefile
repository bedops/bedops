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
	-cp ${APPDIR}/conversion/bin/convert2bed ${BINDIR}/




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
	-cp ${APPDIR}/conversion/bin/debug.convert2bed ${BINDIR}/

install_gprof: prep_c install_conversion_scripts install_starchcluster_scripts
	-cp ${APPDIR}/sort-bed/bin/gprof.sort-bed ${BINDIR}/
	-cp ${APPDIR}/bedops/bin/gprof.bedops ${BINDIR}/
	-cp ${APPDIR}/closestfeats/bin/gprof.closest-features ${BINDIR}/
	-cp ${APPDIR}/bedmap/bin/gprof.bedmap ${BINDIR}/
	-cp ${APPDIR}/bedextract/bin/gprof.bedextract ${BINDIR}/
	-cp ${APPDIR}/starch/bin/gprof.starch ${BINDIR}/
	-cp ${APPDIR}/starch/bin/gprof.unstarch ${BINDIR}/
	-cp ${APPDIR}/starch/bin/gprof.starchcat ${BINDIR}/
	-cp ${APPDIR}/conversion/bin/gprof.convert2bed ${BINDIR}/

install_starchcluster_scripts: prep_c
	-cp ${APPDIR}/starch/bin/starchcluster_sge ${BINDIR}/
	-cp ${APPDIR}/starch/bin/starchcluster_gnuParallel ${BINDIR}/

install_conversion_scripts: prep_c
	-cp ${APPDIR}/conversion/src/wrappers/bam2bed ${BINDIR}/bam2bed
	-cp ${APPDIR}/conversion/src/wrappers/gff2bed ${BINDIR}/gff2bed
	-cp ${APPDIR}/conversion/src/wrappers/gtf2bed ${BINDIR}/gtf2bed
	-cp ${APPDIR}/conversion/src/wrappers/psl2bed ${BINDIR}/psl2bed
	-cp ${APPDIR}/conversion/src/wrappers/rmsk2bed ${BINDIR}/rmsk2bed
	-cp ${APPDIR}/conversion/src/wrappers/sam2bed ${BINDIR}/sam2bed
	-cp ${APPDIR}/conversion/src/wrappers/vcf2bed ${BINDIR}/vcf2bed
	-cp ${APPDIR}/conversion/src/wrappers/wig2bed ${BINDIR}/wig2bed
	-cp ${APPDIR}/conversion/src/wrappers/bam2starch ${BINDIR}/bam2starch
	-cp ${APPDIR}/conversion/src/wrappers/gff2starch ${BINDIR}/gff2starch
	-cp ${APPDIR}/conversion/src/wrappers/gtf2starch ${BINDIR}/gtf2starch
	-cp ${APPDIR}/conversion/src/wrappers/psl2starch ${BINDIR}/psl2starch
	-cp ${APPDIR}/conversion/src/wrappers/rmsk2starch ${BINDIR}/rmsk2starch
	-cp ${APPDIR}/conversion/src/wrappers/sam2starch ${BINDIR}/sam2starch
	-cp ${APPDIR}/conversion/src/wrappers/vcf2starch ${BINDIR}/vcf2starch
	-cp ${APPDIR}/conversion/src/wrappers/wig2starch ${BINDIR}/wig2starch
	-cp ${APPDIR}/conversion/src/wrappers/bam2bed_sge ${BINDIR}/bam2bed_sge
	-cp ${APPDIR}/conversion/src/wrappers/bam2bed_gnuParallel ${BINDIR}/bam2bed_gnuParallel
	-cp ${APPDIR}/conversion/src/wrappers/bam2starch_sge ${BINDIR}/bam2starch_sge
	-cp ${APPDIR}/conversion/src/wrappers/bam2starch_gnuParallel ${BINDIR}/bam2starch_gnuParallel

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
	-cp ${APPDIR}/starch/bin/starchcluster_sge ${OSXPKGDIR}/starchcluster_sge
	-cp ${APPDIR}/starch/bin/starchcluster_gnuParallel ${OSXPKGDIR}/starchcluster_gnuParallel
	-cp ${APPDIR}/conversion/bin/convert2bed ${OSXPKGDIR}/
	-cp ${APPDIR}/conversion/src/wrappers/bam2bed ${OSXPKGDIR}/bam2bed
	-cp ${APPDIR}/conversion/src/wrappers/gff2bed ${OSXPKGDIR}/gff2bed
	-cp ${APPDIR}/conversion/src/wrappers/gtf2bed ${OSXPKGDIR}/gtf2bed
	-cp ${APPDIR}/conversion/src/wrappers/psl2bed ${OSXPKGDIR}/psl2bed
	-cp ${APPDIR}/conversion/src/wrappers/rmsk2bed ${OSXPKGDIR}/rmsk2bed
	-cp ${APPDIR}/conversion/src/wrappers/sam2bed ${OSXPKGDIR}/sam2bed
	-cp ${APPDIR}/conversion/src/wrappers/vcf2bed ${OSXPKGDIR}/vcf2bed
	-cp ${APPDIR}/conversion/src/wrappers/wig2bed ${OSXPKGDIR}/wig2bed
	-cp ${APPDIR}/conversion/src/wrappers/bam2starch ${OSXPKGDIR}/bam2starch
	-cp ${APPDIR}/conversion/src/wrappers/gff2starch ${OSXPKGDIR}/gff2starch
	-cp ${APPDIR}/conversion/src/wrappers/gtf2starch ${OSXPKGDIR}/gtf2starch
	-cp ${APPDIR}/conversion/src/wrappers/psl2starch ${OSXPKGDIR}/psl2starch
	-cp ${APPDIR}/conversion/src/wrappers/rmsk2starch ${OSXPKGDIR}/rmsk2starch
	-cp ${APPDIR}/conversion/src/wrappers/sam2starch ${OSXPKGDIR}/sam2starch
	-cp ${APPDIR}/conversion/src/wrappers/vcf2starch ${OSXPKGDIR}/vcf2starch
	-cp ${APPDIR}/conversion/src/wrappers/wig2starch ${OSXPKGDIR}/wig2starch
	-cp ${APPDIR}/conversion/src/wrappers/bam2bed_sge ${OSXPKGDIR}/bam2bed_sge
	-cp ${APPDIR}/conversion/src/wrappers/bam2bed_gnuParallel ${OSXPKGDIR}/bam2bed_gnuParallel
	-cp ${APPDIR}/conversion/src/wrappers/bam2starch_sge ${OSXPKGDIR}/bam2starch_sge
	-cp ${APPDIR}/conversion/src/wrappers/bam2starch_gnuParallel ${OSXPKGDIR}/bam2starch_gnuParallel
	mkdir -p ${OSXLIBDIR}
