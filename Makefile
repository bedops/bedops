export KERNEL        := ${shell uname -a | cut -f1 -d' '}
APPDIR                = applications/bed
OTHERDIR              = applications/other
OSXPKGROOT            = packaging/os_x
OSXBUILDDIR           = ${OSXPKGROOT}/build
OSXPKGDIR             = ${OSXPKGROOT}/resources/bin
OSXLIBDIR             = ${OSXPKGROOT}/resources/lib
SELF                  = ${shell pwd}/Makefile
MASSIVE_REST_EXP      = 22
MASSIVE_ID_EXP        = 18
MASSIVE_CHROM_EXP     = 8
JPARALLEL             = 8
MEGAROW               = megarow
TYPICAL               = typical
DEFAULT_BINARY_TYPE   = ${TYPICAL} 
WRAPPERS              = $(wildcard ${APPDIR}/conversion/src/wrappers/*)
CWD                  := $(shell pwd)
BINDIR                = bin
BINDIR_MODULE         = modules
BINDIR_MODULE_TYPICAL = ${BINDIR_MODULE}/${TYPICAL}
BINDIR_MODULE_MEGAROW = ${BINDIR_MODULE}/${MEGAROW}

default:
ifeq ($(KERNEL), Darwin)
	$(MAKE) $(MAKECMDGOALS) -j $(JPARALLEL) -f system.mk/Makefile.darwin
else
	$(MAKE) $(MAKECMDGOALS) -j $(JPARALLEL) -f system.mk/Makefile.linux
endif

clean: default

support: default

debug: default

gprof: default

all:
	$(MAKE) support -f ${SELF}
	$(MAKE) typical -f ${SELF}
	$(MAKE) megarow -f ${SELF}
	$(MAKE) install_all -f ${SELF}
	$(MAKE) symlink_typical -f ${SELF}

module_all:
	$(MAKE) support -f ${SELF}
	$(MAKE) typical -f ${SELF}
	$(MAKE) megarow -f ${SELF}
	$(MAKE) install_all -f ${SELF}
	$(MAKE) module_binaries -f ${SELF}

megarow:
	$(MAKE) BINARY_TYPE=$(MEGAROW) BINARY_TYPE_NUM=1 POSTFIX=-$(MEGAROW) MEGAFLAGS="-DREST_EXPONENT=${MASSIVE_REST_EXP} -DID_EXPONENT=${MASSIVE_ID_EXP} -DCHROM_EXPONENT=${MASSIVE_CHROM_EXP}" -f ${SELF}

typical:
	$(MAKE) BINARY_TYPE=$(TYPICAL) BINARY_TYPE_NUM=0 POSTFIX=-$(TYPICAL) -f ${SELF}

symlink_typical:
	$(eval variablename=`find $(BINDIR)/ -maxdepth 1 -mindepth 1 -type f -name '*$(TYPICAL)' -print0 | xargs -L1 -0 -I{} sh -c 'basename {}'`)
	for i in ${variablename}; do \
		fooname=`echo $$i | sed 's/-$(TYPICAL)//'`; \
		echo $${fooname}; \
		ln -sf $$i $(BINDIR)/$${fooname}; \
	done

symlink_megarow:
	$(eval variablename=`find $(BINDIR)/ -maxdepth 1 -mindepth 1 -type f -name '*$(MEGAROW)' -print0 | xargs -L1 -0 -I{} sh -c 'basename {}'`)
	for i in ${variablename}; do \
		fooname=`echo $$i | sed 's/-$(MEGAROW)//'`; \
		echo $${fooname}; \
		ln -sf $$i $(BINDIR)/$${fooname}; \
	done

install: prep_c install_conversion_scripts install_starch_scripts
	-cp ${APPDIR}/sort-bed/bin/sort-bed- ${BINDIR}/sort-bed
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-slurm- ${BINDIR}/update-sort-bed-slurm
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-starch-slurm- ${BINDIR}/update-sort-bed-starch-slurm
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-migrate-candidates- ${BINDIR}/update-sort-bed-migrate-candidates
	-cp ${APPDIR}/bedops/bin/bedops- ${BINDIR}/bedops
	-cp ${APPDIR}/closestfeats/bin/closest-features- ${BINDIR}/closest-features
	-cp ${APPDIR}/bedmap/bin/bedmap- ${BINDIR}/bedmap
	-cp ${APPDIR}/bedextract/bin/bedextract- ${BINDIR}/bedextract
	-cp ${APPDIR}/starch/bin/starch- ${BINDIR}/starch
	-cp ${APPDIR}/starch/bin/unstarch- ${BINDIR}/unstarch
	-cp ${APPDIR}/starch/bin/starchcat- ${BINDIR}/starchcat
	-cp ${APPDIR}/starch/bin/starchstrip- ${BINDIR}/starchstrip
	-cp ${APPDIR}/conversion/bin/convert2bed- ${BINDIR}/convert2bed

install_all: install_conversion_scripts_with_suffix install_starch_scripts_with_suffix
	-cp ${APPDIR}/sort-bed/bin/sort-bed-$(MEGAROW) ${BINDIR}/sort-bed-$(MEGAROW)
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-slurm-$(MEGAROW) ${BINDIR}/update-sort-bed-slurm-$(MEGAROW)
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-starch-slurm-$(MEGAROW) ${BINDIR}/update-sort-bed-starch-slurm-$(MEGAROW)
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-migrate-candidates-$(MEGAROW) ${BINDIR}/update-sort-bed-migrate-candidates-$(MEGAROW)
	-cp ${APPDIR}/bedops/bin/bedops-$(MEGAROW) ${BINDIR}/bedops-$(MEGAROW)
	-cp ${APPDIR}/closestfeats/bin/closest-features-$(MEGAROW) ${BINDIR}/closest-features-$(MEGAROW)
	-cp ${APPDIR}/bedmap/bin/bedmap-$(MEGAROW) ${BINDIR}/bedmap-$(MEGAROW)
	-cp ${APPDIR}/bedextract/bin/bedextract-$(MEGAROW) ${BINDIR}/bedextract-$(MEGAROW)
	-cp ${APPDIR}/starch/bin/starch-$(MEGAROW) ${BINDIR}/starch-$(MEGAROW)
	-cp ${APPDIR}/starch/bin/unstarch-$(MEGAROW) ${BINDIR}/unstarch-$(MEGAROW)
	-cp ${APPDIR}/starch/bin/starchcat-$(MEGAROW) ${BINDIR}/starchcat-$(MEGAROW)
	-cp ${APPDIR}/starch/bin/starchstrip-$(MEGAROW) ${BINDIR}/starchstrip-$(MEGAROW)
	-cp ${APPDIR}/conversion/bin/convert2bed-$(MEGAROW) ${BINDIR}/convert2bed-$(MEGAROW)
	-cp ${APPDIR}/sort-bed/bin/sort-bed-$(TYPICAL) ${BINDIR}/sort-bed-$(TYPICAL)
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-slurm-$(TYPICAL) ${BINDIR}/update-sort-bed-slurm-$(TYPICAL)
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-starch-slurm-$(TYPICAL) ${BINDIR}/update-sort-bed-starch-slurm-$(TYPICAL)
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-migrate-candidates-$(TYPICAL) ${BINDIR}/update-sort-bed-migrate-candidates-$(TYPICAL)
	-cp ${APPDIR}/bedops/bin/bedops-$(TYPICAL) ${BINDIR}/bedops-$(TYPICAL)
	-cp ${APPDIR}/closestfeats/bin/closest-features-$(TYPICAL) ${BINDIR}/closest-features-$(TYPICAL)
	-cp ${APPDIR}/bedmap/bin/bedmap-$(TYPICAL) ${BINDIR}/bedmap-$(TYPICAL)
	-cp ${APPDIR}/bedextract/bin/bedextract-$(TYPICAL) ${BINDIR}/bedextract-$(TYPICAL)
	-cp ${APPDIR}/starch/bin/starch-$(TYPICAL) ${BINDIR}/starch-$(TYPICAL)
	-cp ${APPDIR}/starch/bin/unstarch-$(TYPICAL) ${BINDIR}/unstarch-$(TYPICAL)
	-cp ${APPDIR}/starch/bin/starchcat-$(TYPICAL) ${BINDIR}/starchcat-$(TYPICAL)
	-cp ${APPDIR}/starch/bin/starchstrip-$(TYPICAL) ${BINDIR}/starchstrip-$(TYPICAL)
	-cp ${APPDIR}/conversion/bin/convert2bed-$(TYPICAL) ${BINDIR}/convert2bed-$(TYPICAL)
	-cp ${OTHERDIR}/switch-BEDOPS-binary-type ${BINDIR}/switch-BEDOPS-binary-type

module_binaries:
	mkdir -p ${BINDIR_MODULE_TYPICAL}
	-cp ${BINDIR}/sort-bed-$(TYPICAL) ${BINDIR_MODULE_TYPICAL}/sort-bed
	-cp ${BINDIR}/update-sort-bed-slurm-$(TYPICAL) ${BINDIR_MODULE_TYPICAL}/update-sort-bed-slurm
	-cp ${BINDIR}/update-sort-bed-starch-slurm-$(TYPICAL) ${BINDIR_MODULE_TYPICAL}/update-sort-bed-starch-slurm
	-cp ${BINDIR}/update-sort-bed-migrate-candidates-$(TYPICAL) ${BINDIR_MODULE_TYPICAL}/update-sort-bed-migrate-candidates
	-cp ${BINDIR}/bedops-$(TYPICAL) ${BINDIR_MODULE_TYPICAL}/bedops
	-cp ${BINDIR}/closest-features-$(TYPICAL) ${BINDIR_MODULE_TYPICAL}/closest-features
	-cp ${BINDIR}/bedmap-$(TYPICAL) ${BINDIR_MODULE_TYPICAL}/bedmap
	-cp ${BINDIR}/bedextract-$(TYPICAL) ${BINDIR_MODULE_TYPICAL}/bedextract
	-cp ${BINDIR}/starch-$(TYPICAL) ${BINDIR_MODULE_TYPICAL}/starch
	-cp ${BINDIR}/unstarch-$(TYPICAL) ${BINDIR_MODULE_TYPICAL}/unstarch
	-cp ${BINDIR}/starchcat-$(TYPICAL) ${BINDIR_MODULE_TYPICAL}/starchcat
	-cp ${BINDIR}/starchstrip-$(TYPICAL) ${BINDIR_MODULE_TYPICAL}/starchstrip
	-cp ${BINDIR}/starch-diff-$(TYPICAL) ${BINDIR_MODULE_TYPICAL}/starch-diff
	-cp ${BINDIR}/convert2bed-$(TYPICAL) ${BINDIR_MODULE_TYPICAL}/convert2bed
	-cp ${BINDIR}/bam2bed-$(TYPICAL) ${BINDIR_MODULE_TYPICAL}/bam2bed
	-cp ${BINDIR}/gff2bed-$(TYPICAL) ${BINDIR_MODULE_TYPICAL}/gff2bed
	-cp ${BINDIR}/gtf2bed-$(TYPICAL) ${BINDIR_MODULE_TYPICAL}/gtf2bed
	-cp ${BINDIR}/gvf2bed-$(TYPICAL) ${BINDIR_MODULE_TYPICAL}/gvf2bed
	-cp ${BINDIR}/psl2bed-$(TYPICAL) ${BINDIR_MODULE_TYPICAL}/psl2bed
	-cp ${BINDIR}/rmsk2bed-$(TYPICAL) ${BINDIR_MODULE_TYPICAL}/rmsk2bed
	-cp ${BINDIR}/sam2bed-$(TYPICAL) ${BINDIR_MODULE_TYPICAL}/sam2bed
	-cp ${BINDIR}/vcf2bed-$(TYPICAL) ${BINDIR_MODULE_TYPICAL}/vcf2bed
	-cp ${BINDIR}/wig2bed-$(TYPICAL) ${BINDIR_MODULE_TYPICAL}/wig2bed
	-cp ${BINDIR}/bam2starch-$(TYPICAL) ${BINDIR_MODULE_TYPICAL}/bam2starch
	-cp ${BINDIR}/gff2starch-$(TYPICAL) ${BINDIR_MODULE_TYPICAL}/gff2starch
	-cp ${BINDIR}/gtf2starch-$(TYPICAL) ${BINDIR_MODULE_TYPICAL}/gtf2starch
	-cp ${BINDIR}/gvf2starch-$(TYPICAL) ${BINDIR_MODULE_TYPICAL}/gvf2starch
	-cp ${BINDIR}/psl2starch-$(TYPICAL) ${BINDIR_MODULE_TYPICAL}/psl2starch
	-cp ${BINDIR}/rmsk2starch-$(TYPICAL) ${BINDIR_MODULE_TYPICAL}/rmsk2starch
	-cp ${BINDIR}/sam2starch-$(TYPICAL) ${BINDIR_MODULE_TYPICAL}/sam2starch
	-cp ${BINDIR}/vcf2starch-$(TYPICAL) ${BINDIR_MODULE_TYPICAL}/vcf2starch
	-cp ${BINDIR}/wig2starch-$(TYPICAL) ${BINDIR_MODULE_TYPICAL}/wig2starch
	-cp ${BINDIR}/bam2bed_sge-$(TYPICAL) ${BINDIR_MODULE_TYPICAL}/bam2bed_sge
	-cp ${BINDIR}/bam2bed_slurm-$(TYPICAL) ${BINDIR_MODULE_TYPICAL}/bam2bed_slurm
	-cp ${BINDIR}/bam2bed_gnuParallel-$(TYPICAL) ${BINDIR_MODULE_TYPICAL}/bam2bed_gnuParallel
	-cp ${BINDIR}/bam2starch_sge-$(TYPICAL) ${BINDIR_MODULE_TYPICAL}/bam2starch_sge
	-cp ${BINDIR}/bam2starch_slurm-$(TYPICAL) ${BINDIR_MODULE_TYPICAL}/bam2starch_slurm
	-cp ${BINDIR}/bam2starch_gnuParallel-$(TYPICAL) ${BINDIR_MODULE_TYPICAL}/bam2starch_gnuParallel
	-cp ${BINDIR}/starchcluster_sge-$(TYPICAL) ${BINDIR_MODULE_TYPICAL}/starchcluster_sge
	-cp ${BINDIR}/starchcluster_gnuParallel-$(TYPICAL) ${BINDIR_MODULE_TYPICAL}/starchcluster_gnuParallel
	-cp ${BINDIR}/starchcluster_slurm-$(TYPICAL) ${BINDIR_MODULE_TYPICAL}/starchcluster_slurm
	-cp ${BINDIR}/starch-diff-$(TYPICAL) ${BINDIR_MODULE_TYPICAL}/starch-diff
	mkdir -p ${BINDIR_MODULE_MEGAROW}
	-cp ${BINDIR}/sort-bed-$(MEGAROW) ${BINDIR_MODULE_MEGAROW}/sort-bed
	-cp ${BINDIR}/update-sort-bed-slurm-$(MEGAROW) ${BINDIR_MODULE_MEGAROW}/update-sort-bed-slurm
	-cp ${BINDIR}/update-sort-bed-starch-slurm-$(MEGAROW) ${BINDIR_MODULE_MEGAROW}/update-sort-bed-starch-slurm
	-cp ${BINDIR}/update-sort-bed-migrate-candidates-$(MEGAROW) ${BINDIR_MODULE_MEGAROW}/update-sort-bed-migrate-candidates
	-cp ${BINDIR}/bedops-$(MEGAROW) ${BINDIR_MODULE_MEGAROW}/bedops
	-cp ${BINDIR}/closest-features-$(MEGAROW) ${BINDIR_MODULE_MEGAROW}/closest-features
	-cp ${BINDIR}/bedmap-$(MEGAROW) ${BINDIR_MODULE_MEGAROW}/bedmap
	-cp ${BINDIR}/bedextract-$(MEGAROW) ${BINDIR_MODULE_MEGAROW}/bedextract
	-cp ${BINDIR}/starch-$(MEGAROW) ${BINDIR_MODULE_MEGAROW}/starch
	-cp ${BINDIR}/unstarch-$(MEGAROW) ${BINDIR_MODULE_MEGAROW}/unstarch
	-cp ${BINDIR}/starchcat-$(MEGAROW) ${BINDIR_MODULE_MEGAROW}/starchcat
	-cp ${BINDIR}/starchstrip-$(MEGAROW) ${BINDIR_MODULE_MEGAROW}/starchstrip
	-cp ${BINDIR}/starch-diff-$(MEGAROW) ${BINDIR_MODULE_MEGAROW}/starch-diff
	-cp ${BINDIR}/convert2bed-$(MEGAROW) ${BINDIR_MODULE_MEGAROW}/convert2bed
	-cp ${BINDIR}/bam2bed-$(MEGAROW) ${BINDIR_MODULE_MEGAROW}/bam2bed
	-cp ${BINDIR}/gff2bed-$(MEGAROW) ${BINDIR_MODULE_MEGAROW}/gff2bed
	-cp ${BINDIR}/gtf2bed-$(MEGAROW) ${BINDIR_MODULE_MEGAROW}/gtf2bed
	-cp ${BINDIR}/gvf2bed-$(MEGAROW) ${BINDIR_MODULE_MEGAROW}/gvf2bed
	-cp ${BINDIR}/psl2bed-$(MEGAROW) ${BINDIR_MODULE_MEGAROW}/psl2bed
	-cp ${BINDIR}/rmsk2bed-$(MEGAROW) ${BINDIR_MODULE_MEGAROW}/rmsk2bed
	-cp ${BINDIR}/sam2bed-$(MEGAROW) ${BINDIR_MODULE_MEGAROW}/sam2bed
	-cp ${BINDIR}/vcf2bed-$(MEGAROW) ${BINDIR_MODULE_MEGAROW}/vcf2bed
	-cp ${BINDIR}/wig2bed-$(MEGAROW) ${BINDIR_MODULE_MEGAROW}/wig2bed
	-cp ${BINDIR}/bam2starch-$(MEGAROW) ${BINDIR_MODULE_MEGAROW}/bam2starch
	-cp ${BINDIR}/gff2starch-$(MEGAROW) ${BINDIR_MODULE_MEGAROW}/gff2starch
	-cp ${BINDIR}/gtf2starch-$(MEGAROW) ${BINDIR_MODULE_MEGAROW}/gtf2starch
	-cp ${BINDIR}/gvf2starch-$(MEGAROW) ${BINDIR_MODULE_MEGAROW}/gvf2starch
	-cp ${BINDIR}/psl2starch-$(MEGAROW) ${BINDIR_MODULE_MEGAROW}/psl2starch
	-cp ${BINDIR}/rmsk2starch-$(MEGAROW) ${BINDIR_MODULE_MEGAROW}/rmsk2starch
	-cp ${BINDIR}/sam2starch-$(MEGAROW) ${BINDIR_MODULE_MEGAROW}/sam2starch
	-cp ${BINDIR}/vcf2starch-$(MEGAROW) ${BINDIR_MODULE_MEGAROW}/vcf2starch
	-cp ${BINDIR}/wig2starch-$(MEGAROW) ${BINDIR_MODULE_MEGAROW}/wig2starch
	-cp ${BINDIR}/bam2bed_sge-$(MEGAROW) ${BINDIR_MODULE_MEGAROW}/bam2bed_sge
	-cp ${BINDIR}/bam2bed_slurm-$(MEGAROW) ${BINDIR_MODULE_MEGAROW}/bam2bed_slurm
	-cp ${BINDIR}/bam2bed_gnuParallel-$(MEGAROW) ${BINDIR_MODULE_MEGAROW}/bam2bed_gnuParallel
	-cp ${BINDIR}/bam2starch_sge-$(MEGAROW) ${BINDIR_MODULE_MEGAROW}/bam2starch_sge
	-cp ${BINDIR}/bam2starch_slurm-$(MEGAROW) ${BINDIR_MODULE_MEGAROW}/bam2starch_slurm
	-cp ${BINDIR}/bam2starch_gnuParallel-$(MEGAROW) ${BINDIR_MODULE_MEGAROW}/bam2starch_gnuParallel
	-cp ${BINDIR}/starchcluster_sge-$(MEGAROW) ${BINDIR_MODULE_MEGAROW}/starchcluster_sge
	-cp ${BINDIR}/starchcluster_gnuParallel-$(MEGAROW) ${BINDIR_MODULE_MEGAROW}/starchcluster_gnuParallel
	-cp ${BINDIR}/starchcluster_slurm-$(MEGAROW) ${BINDIR_MODULE_MEGAROW}/starchcluster_slurm
	-cp ${BINDIR}/starch-diff-$(MEGAROW) ${BINDIR_MODULE_MEGAROW}/starch-diff

#######################
# install details

prep_c:
	mkdir -p ${BINDIR}

install_debug: prep_c install_conversion_scripts install_starch_scripts
	-cp ${APPDIR}/sort-bed/bin/debug.sort-bed- ${BINDIR}/debug.sort-bed
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-slurm- ${BINDIR}/update-sort-bed-slurm
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-starch-slurm- ${BINDIR}/update-sort-bed-starch-slurm
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-migrate-candidates- ${BINDIR}/update-sort-bed-migrate-candidates
	-cp ${APPDIR}/bedops/bin/debug.bedops- ${BINDIR}/debug.bedops
	-cp ${APPDIR}/closestfeats/bin/debug.closest-features- ${BINDIR}/debug.closest-features
	-cp ${APPDIR}/bedmap/bin/debug.bedmap- ${BINDIR}/debug.bedmap
	-cp ${APPDIR}/bedextract/bin/debug.bedextract- ${BINDIR}/debug.bedextract
	-cp ${APPDIR}/starch/bin/debug.starch- ${BINDIR}/debug.starch
	-cp ${APPDIR}/starch/bin/debug.unstarch- ${BINDIR}/debug.unstarch
	-cp ${APPDIR}/starch/bin/debug.starchcat- ${BINDIR}/debug.starchcat
	-cp ${APPDIR}/starch/bin/debug.starchstrip- ${BINDIR}/debug.starchstrip
	-cp ${APPDIR}/conversion/bin/debug.convert2bed- ${BINDIR}/debug.convert2bed

install_gprof: prep_c install_conversion_scripts install_starch_scripts
	-cp ${APPDIR}/sort-bed/bin/gprof.sort-bed- ${BINDIR}/gprof.sort-bed
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-slurm- ${BINDIR}/update-sort-bed-slurm
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-starch-slurm- ${BINDIR}/update-sort-bed-starch-slurm
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-migrate-candidates- ${BINDIR}/update-sort-bed-migrate-candidates
	-cp ${APPDIR}/bedops/bin/gprof.bedops- ${BINDIR}/gprof.bedops
	-cp ${APPDIR}/closestfeats/bin/gprof.closest-features- ${BINDIR}/gprof.closest-features
	-cp ${APPDIR}/bedmap/bin/gprof.bedmap- ${BINDIR}/gprof.bedmap
	-cp ${APPDIR}/bedextract/bin/gprof.bedextract- ${BINDIR}/gprof.bedextract
	-cp ${APPDIR}/starch/bin/gprof.starch- ${BINDIR}/gprof.starch
	-cp ${APPDIR}/starch/bin/gprof.unstarch- ${BINDIR}/gprof.unstarch
	-cp ${APPDIR}/starch/bin/gprof.starchcat- ${BINDIR}/gprof.starchcat
	-cp ${APPDIR}/starch/bin/gprof.starchstrip- ${BINDIR}/gprof.starchstrip
	-cp ${APPDIR}/conversion/bin/gprof.convert2bed- ${BINDIR}/gprof.convert2bed

install_starch_scripts: prep_c
	-cp ${APPDIR}/starch/bin/starchcluster_sge- ${BINDIR}/starchcluster_sge
	-cp ${APPDIR}/starch/bin/starchcluster_gnuParallel- ${BINDIR}/starchcluster_gnuParallel
	-cp ${APPDIR}/starch/bin/starchcluster_slurm- ${BINDIR}/starchcluster_slurm
	-cp ${APPDIR}/starch/bin/starch-diff- ${BINDIR}/starch-diff

install_starch_scripts_with_suffix: prep_c
	-cp ${APPDIR}/starch/bin/starchcluster_sge-$(TYPICAL) ${BINDIR}/starchcluster_sge-$(TYPICAL)
	-cp ${APPDIR}/starch/bin/starchcluster_gnuParallel-$(TYPICAL) ${BINDIR}/starchcluster_gnuParallel-$(TYPICAL)
	-cp ${APPDIR}/starch/bin/starchcluster_slurm-$(TYPICAL) ${BINDIR}/starchcluster_slurm-$(TYPICAL)
	-cp ${APPDIR}/starch/bin/starch-diff-$(TYPICAL) ${BINDIR}/starch-diff-$(TYPICAL)
	-cp ${APPDIR}/starch/bin/starchcluster_sge-$(MEGAROW) ${BINDIR}/starchcluster_sge-$(MEGAROW)
	-cp ${APPDIR}/starch/bin/starchcluster_gnuParallel-$(MEGAROW) ${BINDIR}/starchcluster_gnuParallel-$(MEGAROW)
	-cp ${APPDIR}/starch/bin/starchcluster_slurm-$(MEGAROW) ${BINDIR}/starchcluster_slurm-$(MEGAROW)
	-cp ${APPDIR}/starch/bin/starch-diff-$(MEGAROW) ${BINDIR}/starch-diff-$(MEGAROW)

install_conversion_scripts: prep_c
	-cp ${APPDIR}/conversion/src/wrappers/bam2bed ${BINDIR}/bam2bed
	-cp ${APPDIR}/conversion/src/wrappers/gff2bed ${BINDIR}/gff2bed
	-cp ${APPDIR}/conversion/src/wrappers/gtf2bed ${BINDIR}/gtf2bed
	-cp ${APPDIR}/conversion/src/wrappers/gvf2bed ${BINDIR}/gvf2bed
	-cp ${APPDIR}/conversion/src/wrappers/psl2bed ${BINDIR}/psl2bed
	-cp ${APPDIR}/conversion/src/wrappers/rmsk2bed ${BINDIR}/rmsk2bed
	-cp ${APPDIR}/conversion/src/wrappers/sam2bed ${BINDIR}/sam2bed
	-cp ${APPDIR}/conversion/src/wrappers/vcf2bed ${BINDIR}/vcf2bed
	-cp ${APPDIR}/conversion/src/wrappers/wig2bed ${BINDIR}/wig2bed
	-cp ${APPDIR}/conversion/src/wrappers/bam2starch ${BINDIR}/bam2starch
	-cp ${APPDIR}/conversion/src/wrappers/gff2starch ${BINDIR}/gff2starch
	-cp ${APPDIR}/conversion/src/wrappers/gtf2starch ${BINDIR}/gtf2starch
	-cp ${APPDIR}/conversion/src/wrappers/gvf2starch ${BINDIR}/gvf2starch
	-cp ${APPDIR}/conversion/src/wrappers/psl2starch ${BINDIR}/psl2starch
	-cp ${APPDIR}/conversion/src/wrappers/rmsk2starch ${BINDIR}/rmsk2starch
	-cp ${APPDIR}/conversion/src/wrappers/sam2starch ${BINDIR}/sam2starch
	-cp ${APPDIR}/conversion/src/wrappers/vcf2starch ${BINDIR}/vcf2starch
	-cp ${APPDIR}/conversion/src/wrappers/wig2starch ${BINDIR}/wig2starch
	-cp ${APPDIR}/conversion/src/wrappers/bam2bed_sge ${BINDIR}/bam2bed_sge
	-cp ${APPDIR}/conversion/src/wrappers/bam2bed_slurm ${BINDIR}/bam2bed_slurm
	-cp ${APPDIR}/conversion/src/wrappers/bam2bed_gnuParallel ${BINDIR}/bam2bed_gnuParallel
	-cp ${APPDIR}/conversion/src/wrappers/bam2starch_sge ${BINDIR}/bam2starch_sge
	-cp ${APPDIR}/conversion/src/wrappers/bam2starch_slurm ${BINDIR}/bam2starch_slurm
	-cp ${APPDIR}/conversion/src/wrappers/bam2starch_gnuParallel ${BINDIR}/bam2starch_gnuParallel

.PHONY: $(WRAPPERS)

install_conversion_scripts_with_suffix: $(WRAPPERS)

$(WRAPPERS): prep_c
	cp $@ $(patsubst %,$(BINDIR)/%-$(TYPICAL), $(notdir $@))
	cp $@ $(patsubst %,$(BINDIR)/%-$(MEGAROW), $(notdir $@))

install_osx_packaging_bins: prep_c all
	mkdir -p ${OSXPKGDIR}
	-cp ${APPDIR}/sort-bed/bin/sort-bed-typical ${OSXPKGDIR}/sort-bed-typical
	-cp ${APPDIR}/sort-bed/bin/sort-bed-megarow ${OSXPKGDIR}/sort-bed-megarow
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-slurm-typical ${OSXPKGDIR}/update-sort-bed-slurm-typical
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-slurm-megarow ${OSXPKGDIR}/update-sort-bed-slurm-megarow
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-starch-slurm-typical ${OSXPKGDIR}/update-sort-bed-starch-slurm-typical
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-starch-slurm-megarow ${OSXPKGDIR}/update-sort-bed-starch-slurm-megarow
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-migrate-candidates-typical ${OSXPKGDIR}/update-sort-bed-migrate-candidates-typical
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-migrate-candidates-megarow ${OSXPKGDIR}/update-sort-bed-migrate-candidates-megarow
	-cp ${APPDIR}/bedops/bin/bedops-typical ${OSXPKGDIR}/bedops-typical
	-cp ${APPDIR}/bedops/bin/bedops-megarow ${OSXPKGDIR}/bedops-megarow
	-cp ${APPDIR}/closestfeats/bin/closest-features-typical ${OSXPKGDIR}/closest-features-typical
	-cp ${APPDIR}/closestfeats/bin/closest-features-megarow ${OSXPKGDIR}/closest-features-megarow
	-cp ${APPDIR}/bedmap/bin/bedmap-typical ${OSXPKGDIR}/bedmap-typical
	-cp ${APPDIR}/bedmap/bin/bedmap-megarow ${OSXPKGDIR}/bedmap-megarow
	-cp ${APPDIR}/bedextract/bin/bedextract-typical ${OSXPKGDIR}/bedextract-typical
	-cp ${APPDIR}/bedextract/bin/bedextract-megarow ${OSXPKGDIR}/bedextract-megarow
	-cp ${APPDIR}/starch/bin/starch-typical ${OSXPKGDIR}/starch-typical
	-cp ${APPDIR}/starch/bin/starch-megarow ${OSXPKGDIR}/starch-megarow
	-cp ${APPDIR}/starch/bin/unstarch-typical ${OSXPKGDIR}/unstarch-typical
	-cp ${APPDIR}/starch/bin/unstarch-megarow ${OSXPKGDIR}/unstarch-megarow
	-cp ${APPDIR}/starch/bin/starchcat-typical ${OSXPKGDIR}/starchcat-typical
	-cp ${APPDIR}/starch/bin/starchcat-megarow ${OSXPKGDIR}/starchcat-megarow
	-cp ${APPDIR}/starch/bin/starchstrip-typical ${OSXPKGDIR}/starchstrip-typical
	-cp ${APPDIR}/starch/bin/starchstrip-megarow ${OSXPKGDIR}/starchstrip-megarow
	-cp ${APPDIR}/starch/bin/starchcluster_sge-typical ${OSXPKGDIR}/starchcluster_sge-typical
	-cp ${APPDIR}/starch/bin/starchcluster_sge-megarow ${OSXPKGDIR}/starchcluster_sge-megarow
	-cp ${APPDIR}/starch/bin/starchcluster_gnuParallel-typical ${OSXPKGDIR}/starchcluster_gnuParallel-typical
	-cp ${APPDIR}/starch/bin/starchcluster_gnuParallel-megarow ${OSXPKGDIR}/starchcluster_gnuParallel-megarow
	-cp ${APPDIR}/starch/bin/starchcluster_slurm-typical ${OSXPKGDIR}/starchcluster_slurm-typical
	-cp ${APPDIR}/starch/bin/starchcluster_slurm-megarow ${OSXPKGDIR}/starchcluster_slurm-megarow
	-cp ${APPDIR}/starch/bin/starch-diff-typical ${OSXPKGDIR}/starch-diff-typical
	-cp ${APPDIR}/starch/bin/starch-diff-megarow ${OSXPKGDIR}/starch-diff-megarow
	-cp ${APPDIR}/conversion/bin/convert2bed-typical ${OSXPKGDIR}/convert2bed-typical
	-cp ${APPDIR}/conversion/bin/convert2bed-megarow ${OSXPKGDIR}/convert2bed-megarow
	-cp ${APPDIR}/conversion/src/wrappers/bam2bed ${OSXPKGDIR}/bam2bed
	-cp ${APPDIR}/conversion/src/wrappers/gff2bed ${OSXPKGDIR}/gff2bed
	-cp ${APPDIR}/conversion/src/wrappers/gtf2bed ${OSXPKGDIR}/gtf2bed
	-cp ${APPDIR}/conversion/src/wrappers/gvf2bed ${OSXPKGDIR}/gvf2bed
	-cp ${APPDIR}/conversion/src/wrappers/psl2bed ${OSXPKGDIR}/psl2bed
	-cp ${APPDIR}/conversion/src/wrappers/rmsk2bed ${OSXPKGDIR}/rmsk2bed
	-cp ${APPDIR}/conversion/src/wrappers/sam2bed ${OSXPKGDIR}/sam2bed
	-cp ${APPDIR}/conversion/src/wrappers/vcf2bed ${OSXPKGDIR}/vcf2bed
	-cp ${APPDIR}/conversion/src/wrappers/wig2bed ${OSXPKGDIR}/wig2bed
	-cp ${APPDIR}/conversion/src/wrappers/bam2starch ${OSXPKGDIR}/bam2starch
	-cp ${APPDIR}/conversion/src/wrappers/gff2starch ${OSXPKGDIR}/gff2starch
	-cp ${APPDIR}/conversion/src/wrappers/gtf2starch ${OSXPKGDIR}/gtf2starch
	-cp ${APPDIR}/conversion/src/wrappers/gvf2starch ${OSXPKGDIR}/gvf2starch
	-cp ${APPDIR}/conversion/src/wrappers/psl2starch ${OSXPKGDIR}/psl2starch
	-cp ${APPDIR}/conversion/src/wrappers/rmsk2starch ${OSXPKGDIR}/rmsk2starch
	-cp ${APPDIR}/conversion/src/wrappers/sam2starch ${OSXPKGDIR}/sam2starch
	-cp ${APPDIR}/conversion/src/wrappers/vcf2starch ${OSXPKGDIR}/vcf2starch
	-cp ${APPDIR}/conversion/src/wrappers/wig2starch ${OSXPKGDIR}/wig2starch
	-cp ${APPDIR}/conversion/src/wrappers/bam2bed_sge ${OSXPKGDIR}/bam2bed_sge
	-cp ${APPDIR}/conversion/src/wrappers/bam2bed_slurm ${OSXPKGDIR}/bam2bed_slurm
	-cp ${APPDIR}/conversion/src/wrappers/bam2bed_gnuParallel ${OSXPKGDIR}/bam2bed_gnuParallel
	-cp ${APPDIR}/conversion/src/wrappers/bam2starch_sge ${OSXPKGDIR}/bam2starch_sge
	-cp ${APPDIR}/conversion/src/wrappers/bam2starch_slurm ${OSXPKGDIR}/bam2starch_slurm
	-cp ${APPDIR}/conversion/src/wrappers/bam2starch_gnuParallel ${OSXPKGDIR}/bam2starch_gnuParallel
	-cp ${OTHERDIR}/switch-BEDOPS-binary-type ${OSXPKGDIR}
	-cd ${OSXPKGDIR}; \
	ln -sf ./sort-bed-typical ./sort-bed; \
	ln -sf ./update-sort-bed-slurm-typical ./update-sort-bed-slurm; \
	ln -sf ./update-sort-bed-starch-slurm-typical ./update-sort-bed-starch-slurm; \
	ln -sf ./update-sort-bed-migrate-candidates-typical ./update-sort-bed-migrate-candidates; \
	ln -sf ./bedops-typical ./bedops; \
	ln -sf ./closest-features-typical ./closest-features; \
	ln -sf ./bedmap-typical ./bedmap; \
	ln -sf ./bedextract-typical ./bedextract; \
	ln -sf ./starch-typical ./starch; \
	ln -sf ./unstarch-typical ./unstarch; \
	ln -sf ./starchcat-typical ./starchcat; \
	ln -sf ./starchstrip-typical ./starchstrip; \
	ln -sf ./starchcluster_sge-typical ./starchcluster_sge; \
	ln -sf ./starchcluster_gnuParallel-typical ./starchcluster_gnuParallel; \
	ln -sf ./starchcluster_slurm-typical ./starchcluster_slurm; \
	ln -sf ./starch-diff-typical ./starch-diff; \
	ln -sf ./convert2bed-typical ./convert2bed; \
	cd ${CWD}; \
	mkdir -p ${OSXLIBDIR}; \

update_bedops_version:
ifndef OLD_VER
	$(error Old version variable OLD_VER is undefined (e.g., 2.4.26))
endif
ifndef NEW_VER
	$(error New version variable NEW_VER is undefined (e.g., 2.4.27))
endif
ifeq ($(KERNEL), Darwin)
	sed -i "" -e "s/"$$OLD_VER"/"$$NEW_VER"/g" README.md
	sed -i "" -e "s/"$$OLD_VER"/"$$NEW_VER"/g" packaging/rpm/bedops.spec
	sed -i "" -e "s/"$$OLD_VER"/"$$NEW_VER"/g" packaging/rpm/Dockerfile
	sed -i "" -e "s/"$$OLD_VER"/"$$NEW_VER"/g" packaging/deb/control
	sed -i "" -e "s/"$$OLD_VER"/"$$NEW_VER"/g" packaging/deb/Dockerfile
	sed -i "" -e "s/"$$OLD_VER"/"$$NEW_VER"/g" interfaces/general-headers/suite/BEDOPS.Version.hpp
	sed -i "" -e "s/"$$OLD_VER"/"$$NEW_VER"/g" packaging/os_x/BEDOPS.pkgproj
	sed -i "" -e "s/"$$OLD_VER"/"$$NEW_VER"/g" docs/index.rst
	sed -i "" -e "s/"$$OLD_VER"/"$$NEW_VER"/g" docs/conf.py
	sed -i "" -e "s/"$$OLD_VER"/"$$NEW_VER"/g" applications/bed/conversion/src/convert2bed.h
	sed -i "" -e "s/"$$OLD_VER"/"$$NEW_VER"/g" applications/bed/starch/src/starchcluster_gnuParallel.tcsh
	sed -i "" -e "s/"$$OLD_VER"/"$$NEW_VER"/g" applications/bed/starch/src/starchcluster_sge.tcsh
	sed -i "" -e "s/"$$OLD_VER"/"$$NEW_VER"/g" applications/bed/starch/src/starchcluster_slurm.tcsh
	sed -i "" -e "s/"$$OLD_VER"/"$$NEW_VER"/g" applications/bed/starch/src/starch-diff.py
	sed -i "" -e "s/"$$OLD_VER"/"$$NEW_VER"/g" applications/bed/starch/src/Makefile.darwin
	sed -i "" -e "s/"$$OLD_VER"/"$$NEW_VER"/g" applications/bed/sort-bed/src/update-sort-bed-slurm.py
	sed -i "" -e "s/"$$OLD_VER"/"$$NEW_VER"/g" applications/bed/sort-bed/src/update-sort-bed-starch-slurm.py
	sed -i "" -e "s/"$$OLD_VER"/"$$NEW_VER"/g" applications/bed/sort-bed/src/update-sort-bed-migrate-candidates.py
	find docs/content -type f -exec sed -i "" -e "s/"$$OLD_VER"/"$$NEW_VER"/g" {} +
else
	sed -i "s/"$$OLD_VER"/"$$NEW_VER"/g" README.md
	sed -i "s/"$$OLD_VER"/"$$NEW_VER"/g" packaging/rpm/bedops.spec
	sed -i "s/"$$OLD_VER"/"$$NEW_VER"/g" packaging/rpm/Dockerfile
	sed -i "s/"$$OLD_VER"/"$$NEW_VER"/g" packaging/deb/control
	sed -i "s/"$$OLD_VER"/"$$NEW_VER"/g" packaging/deb/Dockerfile
	sed -i "s/"$$OLD_VER"/"$$NEW_VER"/g" interfaces/general-headers/suite/BEDOPS.Version.hpp
	sed -i "s/"$$OLD_VER"/"$$NEW_VER"/g" packaging/os_x/BEDOPS.pkgproj
	sed -i "s/"$$OLD_VER"/"$$NEW_VER"/g" docs/index.rst
	sed -i "s/"$$OLD_VER"/"$$NEW_VER"/g" docs/conf.py
	sed -i "s/"$$OLD_VER"/"$$NEW_VER"/g" applications/bed/conversion/src/convert2bed.h
	sed -i "s/"$$OLD_VER"/"$$NEW_VER"/g" applications/bed/starch/src/starchcluster_gnuParallel.tcsh
	sed -i "s/"$$OLD_VER"/"$$NEW_VER"/g" applications/bed/starch/src/starchcluster_sge.tcsh
	sed -i "s/"$$OLD_VER"/"$$NEW_VER"/g" applications/bed/starch/src/starchcluster_slurm.tcsh
	sed -i "s/"$$OLD_VER"/"$$NEW_VER"/g" applications/bed/starch/src/starch-diff.py
	sed -i "s/"$$OLD_VER"/"$$NEW_VER"/g" applications/bed/starch/src/Makefile.darwin
	sed -i "s/"$$OLD_VER"/"$$NEW_VER"/g" applications/bed/sort-bed/src/update-sort-bed-slurm.py
	sed -i "s/"$$OLD_VER"/"$$NEW_VER"/g" applications/bed/sort-bed/src/update-sort-bed-starch-slurm.py
	sed -i "s/"$$OLD_VER"/"$$NEW_VER"/g" applications/bed/sort-bed/src/update-sort-bed-migrate-candidates.py
	find docs/content -type f -exec sed -i "s/"$$OLD_VER"/"$$NEW_VER"/g" {} +
endif

docker: packaging/docker/Dockerfile
	docker build -t bedops -f packaging/docker/Dockerfile  .

rpm: packaging/rpm/Dockerfile
	docker build -t bedops:rpm -f packaging/rpm/Dockerfile .

deb: packaging/deb/Dockerfile
	docker build -t bedops:deb -f packaging/deb/Dockerfile .
