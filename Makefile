export KERNEL         := ${shell uname -a | cut -f1 -d' '}
APPDIR                 = applications/bed
OTHERDIR               = applications/other
OSXPKGROOT             = packaging/os_x
OSXBUILDDIR            = ${OSXPKGROOT}/build
OSXPKGDIR              = ${OSXPKGROOT}/resources/bin
OSXLIBDIR              = ${OSXPKGROOT}/resources/lib
SELF                   = ${shell pwd}/Makefile
MASSIVE_REST_EXP       = 22
MASSIVE_ID_EXP         = 18
MASSIVE_CHROM_EXP      = 8
MEASURE128BIT          = 'long double'
JPARALLEL              = 8
MEGAROW                = megarow
TYPICAL                = typical
FLOAT128               = float128
ALL_BINARY_TYPES       = ${TYPICAL} ${MEGAROW} ${FLOAT128}
DEFAULT_BINARY_TYPE    = ${TYPICAL}
export BINARY_TYPE     = ${DEFAULT_BINARY_TYPE}
DEBUG_TYPE             = ${TYPICAL}
WRAPPERS               = $(wildcard ${APPDIR}/conversion/src/wrappers/*)
CWD                   := $(shell pwd)
BINDIR                 = bin
BINDIR_MODULE          = modules
export DESTBINDIR      = .

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
	$(MAKE) float128 -f ${SELF}
	$(MAKE) install_all -f ${SELF}
	$(MAKE) symlink_post_install_all -f ${SELF}

module_all:
	$(MAKE) support -f ${SELF}
	$(MAKE) typical -f ${SELF}
	$(MAKE) megarow -f ${SELF}
	$(MAKE) float128 -f ${SELF}
	$(MAKE) install_all -f ${SELF}
	$(MAKE) module_binaries -f ${SELF}

float128:
	$(MAKE) BINARY_TYPE=$(FLOAT128) MEGAFLAGS="-DMEASURE_TYPE=${MEASURE128BIT}" -f ${SELF}

megarow:
	$(MAKE) BINARY_TYPE=$(MEGAROW) MEGAFLAGS="-DREST_EXPONENT=${MASSIVE_REST_EXP} -DID_EXPONENT=${MASSIVE_ID_EXP} -DCHROM_EXPONENT=${MASSIVE_CHROM_EXP}" -f ${SELF}

typical:
	$(MAKE) BINARY_TYPE=$(TYPICAL) -f ${SELF}

symlink_post_install_all:
	cd ${BINDIR} && ./switch-BEDOPS-binary-type --typical . && cd ${CWD}

symlink_typical:
	$(eval SRCNAMES=`find $(BINDIR)/ -maxdepth 1 -mindepth 1 -type f -name '*$(TYPICAL)' -print0 | xargs -L1 -0 -I{} sh -c 'basename {}'`)
	for SRCNAME in ${SRCNAMES}; do \
		DESTNAME=`echo $$SRCNAME | sed 's/-$(TYPICAL)//'`; \
		ln -sf $$SRCNAME $(BINDIR)/$${DESTNAME}; \
	done

symlink_megarow:
	$(eval SRCNAMES=`find $(BINDIR)/ -maxdepth 1 -mindepth 1 -type f -name '*$(MEGAROW)' -print0 | xargs -L1 -0 -I{} sh -c 'basename {}'`)
	for SRCNAME in ${SRCNAMES}; do \
		DESTNAME=`echo $$SRCNAME | sed 's/-$(MEGAROW)//'`; \
		ln -sf $$SRCNAME $(BINDIR)/$${DESTNAME}; \
	done

symlink_float128:
	$(eval SRCNAMES=`find $(BINDIR)/ -maxdepth 1 -mindepth 1 -type f -name '*$(FLOAT128)' -print0 | xargs -L1 -0 -I{} sh -c 'basename {}'`)
	for SRCNAME in ${SRCNAMES}; do \
		DESTNAME=`echo $$SRCNAME | sed 's/-$(FLOAT128)//'`; \
		ln -sf $$SRCNAME $(BINDIR)/$${DESTNAME}; \
	done

install: prep_c install_conversion_scripts install_starch_scripts
	-cp ${APPDIR}/sort-bed/bin/sort-bed-${TYPICAL} ${BINDIR}/sort-bed
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-slurm-${TYPICAL} ${BINDIR}/update-sort-bed-slurm
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-starch-slurm-${TYPICAL} ${BINDIR}/update-sort-bed-starch-slurm
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-migrate-candidates-${TYPICAL} ${BINDIR}/update-sort-bed-migrate-candidates
	-cp ${APPDIR}/bedops/bin/bedops-${TYPICAL} ${BINDIR}/bedops
	-cp ${APPDIR}/closestfeats/bin/closest-features-${TYPICAL} ${BINDIR}/closest-features
	-cp ${APPDIR}/bedmap/bin/bedmap-${TYPICAL} ${BINDIR}/bedmap
	-cp ${APPDIR}/bedextract/bin/bedextract-${TYPICAL} ${BINDIR}/bedextract
	-cp ${APPDIR}/starch/bin/starch-${TYPICAL} ${BINDIR}/starch
	-cp ${APPDIR}/starch/bin/unstarch-${TYPICAL} ${BINDIR}/unstarch
	-cp ${APPDIR}/starch/bin/starchcat-${TYPICAL} ${BINDIR}/starchcat
	-cp ${APPDIR}/starch/bin/starchstrip-${TYPICAL} ${BINDIR}/starchstrip
	-cp ${APPDIR}/conversion/bin/convert2bed-${TYPICAL} ${BINDIR}/convert2bed

install_float128: prep_c install_conversion_scripts
	-cp ${APPDIR}/sort-bed/bin/sort-bed-${FLOAT128} ${BINDIR}/sort-bed
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-slurm-${FLOAT128} ${BINDIR}/update-sort-bed-slurm
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-starch-slurm-${FLOAT128} ${BINDIR}/update-sort-bed-starch-slurm
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-migrate-candidates-${FLOAT128} ${BINDIR}/update-sort-bed-migrate-candidates
	-cp ${APPDIR}/bedops/bin/bedops-${FLOAT128} ${BINDIR}/bedops
	-cp ${APPDIR}/closestfeats/bin/closest-features-${FLOAT128} ${BINDIR}/closest-features
	-cp ${APPDIR}/bedmap/bin/bedmap-${FLOAT128} ${BINDIR}/bedmap
	-cp ${APPDIR}/bedextract/bin/bedextract-${FLOAT128} ${BINDIR}/bedextract
	-cp ${APPDIR}/starch/bin/starch-${FLOAT128} ${BINDIR}/starch
	-cp ${APPDIR}/starch/bin/unstarch-${FLOAT128} ${BINDIR}/unstarch
	-cp ${APPDIR}/starch/bin/starchcat-${FLOAT128} ${BINDIR}/starchcat
	-cp ${APPDIR}/starch/bin/starchstrip-${FLOAT128} ${BINDIR}/starchstrip
	-cp ${APPDIR}/conversion/bin/convert2bed-${FLOAT128} ${BINDIR}/convert2bed

install_megarow: prep_c install_conversion_scripts
	-cp ${APPDIR}/sort-bed/bin/sort-bed-${MEGAROW} ${BINDIR}/sort-bed
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-slurm-${MEGAROW} ${BINDIR}/update-sort-bed-slurm
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-starch-slurm-${MEGAROW} ${BINDIR}/update-sort-bed-starch-slurm
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-migrate-candidates-${MEGAROW} ${BINDIR}/update-sort-bed-migrate-candidates
	-cp ${APPDIR}/bedops/bin/bedops-${MEGAROW} ${BINDIR}/bedops
	-cp ${APPDIR}/closestfeats/bin/closest-features-${MEGAROW} ${BINDIR}/closest-features
	-cp ${APPDIR}/bedmap/bin/bedmap-${MEGAROW} ${BINDIR}/bedmap
	-cp ${APPDIR}/bedextract/bin/bedextract-${MEGAROW} ${BINDIR}/bedextract
	-cp ${APPDIR}/starch/bin/starch-${MEGAROW} ${BINDIR}/starch
	-cp ${APPDIR}/starch/bin/unstarch-${MEGAROW} ${BINDIR}/unstarch
	-cp ${APPDIR}/starch/bin/starchcat-${MEGAROW} ${BINDIR}/starchcat
	-cp ${APPDIR}/starch/bin/starchstrip-${MEGAROW} ${BINDIR}/starchstrip
	-cp ${APPDIR}/conversion/bin/convert2bed-${MEGAROW} ${BINDIR}/convert2bed

install_all: prep_c install_conversion_scripts_all install_starch_scripts_all
	for btype in ${ALL_BINARY_TYPES}; do \
		cp ${APPDIR}/sort-bed/bin/sort-bed-$$btype ${BINDIR}/sort-bed-$$btype; \
		cp ${APPDIR}/sort-bed/bin/update-sort-bed-slurm-$$btype ${BINDIR}/update-sort-bed-slurm-$$btype; \
		cp ${APPDIR}/sort-bed/bin/update-sort-bed-starch-slurm-$$btype ${BINDIR}/update-sort-bed-starch-slurm-$$btype; \
		cp ${APPDIR}/sort-bed/bin/update-sort-bed-migrate-candidates-$$btype ${BINDIR}/update-sort-bed-migrate-candidates-$$btype; \
		cp ${APPDIR}/bedops/bin/bedops-$$btype ${BINDIR}/bedops-$$btype; \
		cp ${APPDIR}/closestfeats/bin/closest-features-$$btype ${BINDIR}/closest-features-$$btype; \
		cp ${APPDIR}/bedmap/bin/bedmap-$$btype ${BINDIR}/bedmap-$$btype; \
		cp ${APPDIR}/bedextract/bin/bedextract-$$btype ${BINDIR}/bedextract-$$btype; \
		cp ${APPDIR}/starch/bin/starch-$$btype ${BINDIR}/starch-$$btype; \
		cp ${APPDIR}/starch/bin/unstarch-$$btype ${BINDIR}/unstarch-$$btype; \
		cp ${APPDIR}/starch/bin/starchcat-$$btype ${BINDIR}/starchcat-$$btype; \
		cp ${APPDIR}/starch/bin/starchstrip-$$btype ${BINDIR}/starchstrip-$$btype; \
		cp ${APPDIR}/conversion/bin/convert2bed-$$btype ${BINDIR}/convert2bed-$$btype; \
	done
	-cp ${OTHERDIR}/switch-BEDOPS-binary-type ${BINDIR}

module_binaries:
	for btype in ${ALL_BINARY_TYPES}; do \
		mkdir -p ${BINDIR_MODULE}/$$btype; \
		cp ${BINDIR}/sort-bed-$$btype ${BINDIR_MODULE}/$$btype/sort-bed; \
		cp ${BINDIR}/update-sort-bed-slurm-$$btype ${BINDIR_MODULE}/$$btype/update-sort-bed-slurm; \
		cp ${BINDIR}/update-sort-bed-starch-slurm-$$btype ${BINDIR_MODULE}/$$btype/update-sort-bed-starch-slurm; \
		cp ${BINDIR}/update-sort-bed-migrate-candidates-$$btype ${BINDIR_MODULE}/$$btype/update-sort-bed-migrate-candidates; \
		cp ${BINDIR}/bedops-$$btype ${BINDIR_MODULE}/$$btype/bedops; \
		cp ${BINDIR}/closest-features-$$btype ${BINDIR_MODULE}/$$btype/closest-features; \
		cp ${BINDIR}/bedmap-$$btype ${BINDIR_MODULE}/$$btype/bedmap; \
		cp ${BINDIR}/bedextract-$$btype ${BINDIR_MODULE}/$$btype/bedextract; \
		cp ${BINDIR}/starch-$$btype ${BINDIR_MODULE}/$$btype/starch; \
		cp ${BINDIR}/unstarch-$$btype ${BINDIR_MODULE}/$$btype/unstarch; \
		cp ${BINDIR}/starchcat-$$btype ${BINDIR_MODULE}/$$btype/starchcat; \
		cp ${BINDIR}/starchstrip-$$btype ${BINDIR_MODULE}/$$btype/starchstrip; \
		cp ${BINDIR}/starch-diff-$$btype ${BINDIR_MODULE}/$$btype/starch-diff; \
		cp ${BINDIR}/convert2bed-$$btype ${BINDIR_MODULE}/$$btype/convert2bed; \
		cp ${BINDIR}/bam2bed-$$btype ${BINDIR_MODULE}/$$btype/bam2bed; \
		cp ${BINDIR}/gff2bed-$$btype ${BINDIR_MODULE}/$$btype/gff2bed; \
		cp ${BINDIR}/gtf2bed-$$btype ${BINDIR_MODULE}/$$btype/gtf2bed; \
		cp ${BINDIR}/gvf2bed-$$btype ${BINDIR_MODULE}/$$btype/gvf2bed; \
		cp ${BINDIR}/psl2bed-$$btype ${BINDIR_MODULE}/$$btype/psl2bed; \
		cp ${BINDIR}/rmsk2bed-$$btype ${BINDIR_MODULE}/$$btype/rmsk2bed; \
		cp ${BINDIR}/sam2bed-$$btype ${BINDIR_MODULE}/$$btype/sam2bed; \
		cp ${BINDIR}/vcf2bed-$$btype ${BINDIR_MODULE}/$$btype/vcf2bed; \
		cp ${BINDIR}/wig2bed-$$btype ${BINDIR_MODULE}/$$btype/wig2bed; \
		cp ${BINDIR}/bam2starch-$$btype ${BINDIR_MODULE}/$$btype/bam2starch; \
		cp ${BINDIR}/gff2starch-$$btype ${BINDIR_MODULE}/$$btype/gff2starch; \
		cp ${BINDIR}/gtf2starch-$$btype ${BINDIR_MODULE}/$$btype/gtf2starch; \
		cp ${BINDIR}/gvf2starch-$$btype ${BINDIR_MODULE}/$$btype/gvf2starch; \
		cp ${BINDIR}/psl2starch-$$btype ${BINDIR_MODULE}/$$btype/psl2starch; \
		cp ${BINDIR}/rmsk2starch-$$btype ${BINDIR_MODULE}/$$btype/rmsk2starch; \
		cp ${BINDIR}/sam2starch-$$btype ${BINDIR_MODULE}/$$btype/sam2starch; \
		cp ${BINDIR}/vcf2starch-$$btype ${BINDIR_MODULE}/$$btype/vcf2starch; \
		cp ${BINDIR}/wig2starch-$$btype ${BINDIR_MODULE}/$$btype/wig2starch; \
		cp ${BINDIR}/bam2bed_sge-$$btype ${BINDIR_MODULE}/$$btype/bam2bed_sge; \
		cp ${BINDIR}/bam2bed_slurm-$$btype ${BINDIR_MODULE}/$$btype/bam2bed_slurm; \
		cp ${BINDIR}/bam2bed_gnuParallel-$$btype ${BINDIR_MODULE}/$$btype/bam2bed_gnuParallel; \
		cp ${BINDIR}/bam2starch_sge-$$btype ${BINDIR_MODULE}/$$btype/bam2starch_sge; \
		cp ${BINDIR}/bam2starch_slurm-$$btype ${BINDIR_MODULE}/$$btype/bam2starch_slurm; \
		cp ${BINDIR}/bam2starch_gnuParallel-$$btype ${BINDIR_MODULE}/$$btype/bam2starch_gnuParallel; \
		cp ${BINDIR}/starchcluster_sge-$$btype ${BINDIR_MODULE}/$$btype/starchcluster_sge; \
		cp ${BINDIR}/starchcluster_gnuParallel-$$btype ${BINDIR_MODULE}/$$btype/starchcluster_gnuParallel; \
		cp ${BINDIR}/starchcluster_slurm-$$btype ${BINDIR_MODULE}/$$btype/starchcluster_slurm; \
		cp ${BINDIR}/starch-diff-$$btype ${BINDIR_MODULE}/$$btype/starch-diff; \
	done

#######################
# install details

prep_c:
	mkdir -p ${BINDIR}

install_debug: prep_c install_conversion_scripts install_starch_scripts
	find ${APPDIR}/ -maxdepth 4 -mindepth 1 -type f -name 'debug.*${DEBUG_TYPE}' -exec cp {} ${BINDIR} \;

install_gprof: prep_c install_conversion_scripts install_starch_scripts
	-cp ${APPDIR}/sort-bed/bin/gprof.sort-bed ${BINDIR}/gprof.sort-bed
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-slurm ${BINDIR}/update-sort-bed-slurm
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-starch-slurm ${BINDIR}/update-sort-bed-starch-slurm
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-migrate-candidates ${BINDIR}/update-sort-bed-migrate-candidates
	-cp ${APPDIR}/bedops/bin/gprof.bedops ${BINDIR}/gprof.bedops
	-cp ${APPDIR}/closestfeats/bin/gprof.closest-features ${BINDIR}/gprof.closest-features
	-cp ${APPDIR}/bedmap/bin/gprof.bedmap ${BINDIR}/gprof.bedmap
	-cp ${APPDIR}/bedextract/bin/gprof.bedextract ${BINDIR}/gprof.bedextract
	-cp ${APPDIR}/starch/bin/gprof.starch ${BINDIR}/gprof.starch
	-cp ${APPDIR}/starch/bin/gprof.unstarch ${BINDIR}/gprof.unstarch
	-cp ${APPDIR}/starch/bin/gprof.starchcat ${BINDIR}/gprof.starchcat
	-cp ${APPDIR}/starch/bin/gprof.starchstrip ${BINDIR}/gprof.starchstrip
	-cp ${APPDIR}/conversion/bin/gprof.convert2bed ${BINDIR}/gprof.convert2bed

install_starch_scripts: prep_c
	-cp ${APPDIR}/starch/bin/starchcluster_sge-$(TYPICAL) ${BINDIR}/starchcluster_sge
	-cp ${APPDIR}/starch/bin/starchcluster_gnuParallel-$(TYPICAL) ${BINDIR}/starchcluster_gnuParallel
	-cp ${APPDIR}/starch/bin/starchcluster_slurm-$(TYPICAL) ${BINDIR}/starchcluster_slurm
	-cp ${APPDIR}/starch/bin/starch-diff-$(TYPICAL) ${BINDIR}/starch-diff

install_starch_scripts_all: prep_c
	-cp ${APPDIR}/starch/bin/starchcluster_sge-$(TYPICAL) ${BINDIR}/starchcluster_sge-$(TYPICAL)
	-cp ${APPDIR}/starch/bin/starchcluster_gnuParallel-$(TYPICAL) ${BINDIR}/starchcluster_gnuParallel-$(TYPICAL)
	-cp ${APPDIR}/starch/bin/starchcluster_slurm-$(TYPICAL) ${BINDIR}/starchcluster_slurm-$(TYPICAL)
	-cp ${APPDIR}/starch/bin/starch-diff-$(TYPICAL) ${BINDIR}/starch-diff-$(TYPICAL)
	-cp ${APPDIR}/starch/bin/starchcluster_sge-$(MEGAROW) ${BINDIR}/starchcluster_sge-$(MEGAROW)
	-cp ${APPDIR}/starch/bin/starchcluster_gnuParallel-$(MEGAROW) ${BINDIR}/starchcluster_gnuParallel-$(MEGAROW)
	-cp ${APPDIR}/starch/bin/starchcluster_slurm-$(MEGAROW) ${BINDIR}/starchcluster_slurm-$(MEGAROW)
	-cp ${APPDIR}/starch/bin/starch-diff-$(MEGAROW) ${BINDIR}/starch-diff-$(MEGAROW)
	-cp ${APPDIR}/starch/bin/starchcluster_sge-$(MEGAROW) ${BINDIR}/starchcluster_sge-$(FLOAT128)
	-cp ${APPDIR}/starch/bin/starchcluster_gnuParallel-$(MEGAROW) ${BINDIR}/starchcluster_gnuParallel-$(FLOAT128)
	-cp ${APPDIR}/starch/bin/starchcluster_slurm-$(MEGAROW) ${BINDIR}/starchcluster_slurm-$(FLOAT128)
	-cp ${APPDIR}/starch/bin/starch-diff-$(MEGAROW) ${BINDIR}/starch-diff-$(FLOAT128)

install_conversion_scripts: prep_c
	cp $(WRAPPERS) ${BINDIR}

.PHONY: $(WRAPPERS) tests

install_conversion_scripts_all: $(WRAPPERS)

$(WRAPPERS): prep_c
	cp $@ $(patsubst %,$(BINDIR)/%-$(TYPICAL), $(notdir $@))
	cp $@ $(patsubst %,$(BINDIR)/%-$(MEGAROW), $(notdir $@))
	cp $@ $(patsubst %,$(BINDIR)/%-$(FLOAT128), $(notdir $@))

install_osx_packaging_bins: prep_c all
	mkdir -p ${OSXPKGDIR}
	-cp ${APPDIR}/sort-bed/bin/sort-bed-typical ${OSXPKGDIR}/sort-bed-typical
	-cp ${APPDIR}/sort-bed/bin/sort-bed-megarow ${OSXPKGDIR}/sort-bed-megarow
	-cp ${APPDIR}/sort-bed/bin/sort-bed-float128 ${OSXPKGDIR}/sort-bed-float128
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-slurm-typical ${OSXPKGDIR}/update-sort-bed-slurm-typical
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-slurm-megarow ${OSXPKGDIR}/update-sort-bed-slurm-megarow
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-slurm-float128 ${OSXPKGDIR}/update-sort-bed-slurm-float128
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-starch-slurm-typical ${OSXPKGDIR}/update-sort-bed-starch-slurm-typical
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-starch-slurm-megarow ${OSXPKGDIR}/update-sort-bed-starch-slurm-megarow
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-starch-slurm-float128 ${OSXPKGDIR}/update-sort-bed-starch-slurm-float128
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-migrate-candidates-typical ${OSXPKGDIR}/update-sort-bed-migrate-candidates-typical
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-migrate-candidates-megarow ${OSXPKGDIR}/update-sort-bed-migrate-candidates-megarow
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-migrate-candidates-float128 ${OSXPKGDIR}/update-sort-bed-migrate-candidates-float128
	-cp ${APPDIR}/bedops/bin/bedops-typical ${OSXPKGDIR}/bedops-typical
	-cp ${APPDIR}/bedops/bin/bedops-megarow ${OSXPKGDIR}/bedops-megarow
	-cp ${APPDIR}/bedops/bin/bedops-float128 ${OSXPKGDIR}/bedops-float128
	-cp ${APPDIR}/closestfeats/bin/closest-features-typical ${OSXPKGDIR}/closest-features-typical
	-cp ${APPDIR}/closestfeats/bin/closest-features-megarow ${OSXPKGDIR}/closest-features-megarow
	-cp ${APPDIR}/closestfeats/bin/closest-features-float128 ${OSXPKGDIR}/closest-features-float128
	-cp ${APPDIR}/bedmap/bin/bedmap-typical ${OSXPKGDIR}/bedmap-typical
	-cp ${APPDIR}/bedmap/bin/bedmap-megarow ${OSXPKGDIR}/bedmap-megarow
	-cp ${APPDIR}/bedmap/bin/bedmap-float128 ${OSXPKGDIR}/bedmap-float128
	-cp ${APPDIR}/bedextract/bin/bedextract-typical ${OSXPKGDIR}/bedextract-typical
	-cp ${APPDIR}/bedextract/bin/bedextract-megarow ${OSXPKGDIR}/bedextract-megarow
	-cp ${APPDIR}/bedextract/bin/bedextract-float128 ${OSXPKGDIR}/bedextract-float128
	-cp ${APPDIR}/starch/bin/starch-typical ${OSXPKGDIR}/starch-typical
	-cp ${APPDIR}/starch/bin/starch-megarow ${OSXPKGDIR}/starch-megarow
	-cp ${APPDIR}/starch/bin/starch-float128 ${OSXPKGDIR}/starch-float128
	-cp ${APPDIR}/starch/bin/unstarch-typical ${OSXPKGDIR}/unstarch-typical
	-cp ${APPDIR}/starch/bin/unstarch-megarow ${OSXPKGDIR}/unstarch-megarow
	-cp ${APPDIR}/starch/bin/unstarch-float128 ${OSXPKGDIR}/unstarch-float128
	-cp ${APPDIR}/starch/bin/starchcat-typical ${OSXPKGDIR}/starchcat-typical
	-cp ${APPDIR}/starch/bin/starchcat-megarow ${OSXPKGDIR}/starchcat-megarow
	-cp ${APPDIR}/starch/bin/starchcat-float128 ${OSXPKGDIR}/starchcat-float128
	-cp ${APPDIR}/starch/bin/starchstrip-typical ${OSXPKGDIR}/starchstrip-typical
	-cp ${APPDIR}/starch/bin/starchstrip-megarow ${OSXPKGDIR}/starchstrip-megarow
	-cp ${APPDIR}/starch/bin/starchstrip-float128 ${OSXPKGDIR}/starchstrip-float128
	-cp ${APPDIR}/starch/bin/starchcluster_sge-typical ${OSXPKGDIR}/starchcluster_sge-typical
	-cp ${APPDIR}/starch/bin/starchcluster_sge-megarow ${OSXPKGDIR}/starchcluster_sge-megarow
	-cp ${APPDIR}/starch/bin/starchcluster_sge-float128 ${OSXPKGDIR}/starchcluster_sge-float128
	-cp ${APPDIR}/starch/bin/starchcluster_gnuParallel-typical ${OSXPKGDIR}/starchcluster_gnuParallel-typical
	-cp ${APPDIR}/starch/bin/starchcluster_gnuParallel-megarow ${OSXPKGDIR}/starchcluster_gnuParallel-megarow
	-cp ${APPDIR}/starch/bin/starchcluster_gnuParallel-float128 ${OSXPKGDIR}/starchcluster_gnuParallel-float128
	-cp ${APPDIR}/starch/bin/starchcluster_slurm-typical ${OSXPKGDIR}/starchcluster_slurm-typical
	-cp ${APPDIR}/starch/bin/starchcluster_slurm-megarow ${OSXPKGDIR}/starchcluster_slurm-megarow
	-cp ${APPDIR}/starch/bin/starchcluster_slurm-float128 ${OSXPKGDIR}/starchcluster_slurm-float128
	-cp ${APPDIR}/starch/bin/starch-diff-typical ${OSXPKGDIR}/starch-diff-typical
	-cp ${APPDIR}/starch/bin/starch-diff-megarow ${OSXPKGDIR}/starch-diff-megarow
	-cp ${APPDIR}/starch/bin/starch-diff-float128 ${OSXPKGDIR}/starch-diff-float128
	-cp ${APPDIR}/conversion/bin/convert2bed-typical ${OSXPKGDIR}/convert2bed-typical
	-cp ${APPDIR}/conversion/bin/convert2bed-megarow ${OSXPKGDIR}/convert2bed-megarow
	-cp ${APPDIR}/conversion/bin/convert2bed-float128 ${OSXPKGDIR}/convert2bed-float128
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
	./switch-BEDOPS-binary-type --typical .
	-cd ${CWD}; \
	mkdir -p ${OSXLIBDIR}; \
	
#######################
# for users with OS X: 
# "brew install findutils" and "brew install gnu-sed" 
# will install "gfind" and "gsed" binaries

update_bedops_version:
ifndef OLD_VER
	$(error Old version variable OLD_VER is undefined (e.g., 2.4.37))
endif
ifndef NEW_VER
	$(error New version variable NEW_VER is undefined (e.g., 2.4.38))
endif
ifeq ($(KERNEL), Darwin)
	gsed -i "s/"$$OLD_VER"/"$$NEW_VER"/g" README.md
	gsed -i "s/"$$OLD_VER"/"$$NEW_VER"/g" packaging/rpm/bedops.spec
	gsed -i "s/"$$OLD_VER"/"$$NEW_VER"/g" packaging/rpm/Dockerfile
	gsed -i "s/"$$OLD_VER"/"$$NEW_VER"/g" packaging/deb/control
	gsed -i "s/"$$OLD_VER"/"$$NEW_VER"/g" packaging/deb/Dockerfile
	gsed -i "s/"$$OLD_VER"/"$$NEW_VER"/g" interfaces/general-headers/suite/BEDOPS.Version.hpp
	gsed -i "s/"$$OLD_VER"/"$$NEW_VER"/g" packaging/os_x/BEDOPS.pkgproj
	gsed -i "s/"$$OLD_VER"/"$$NEW_VER"/g" docs/index.rst
	gsed -i "s/"$$OLD_VER"/"$$NEW_VER"/g" docs/conf.py
	gsed -i "s/"$$OLD_VER"/"$$NEW_VER"/g" applications/bed/conversion/src/convert2bed.h
	gsed -i "s/"$$OLD_VER"/"$$NEW_VER"/g" applications/bed/starch/src/starchcluster_gnuParallel.tcsh
	gsed -i "s/"$$OLD_VER"/"$$NEW_VER"/g" applications/bed/starch/src/starchcluster_sge.tcsh
	gsed -i "s/"$$OLD_VER"/"$$NEW_VER"/g" applications/bed/starch/src/starchcluster_slurm.tcsh
	gsed -i "s/"$$OLD_VER"/"$$NEW_VER"/g" applications/bed/starch/src/starch-diff.py
	gsed -i "s/"$$OLD_VER"/"$$NEW_VER"/g" applications/bed/starch/src/Makefile.darwin
	gsed -i "s/"$$OLD_VER"/"$$NEW_VER"/g" applications/bed/sort-bed/src/update-sort-bed-slurm.py
	gsed -i "s/"$$OLD_VER"/"$$NEW_VER"/g" applications/bed/sort-bed/src/update-sort-bed-starch-slurm.py
	gsed -i "s/"$$OLD_VER"/"$$NEW_VER"/g" applications/bed/sort-bed/src/update-sort-bed-migrate-candidates.py
	gfind docs/content -type f -exec gsed -i "s/"$$OLD_VER"/"$$NEW_VER"/g" {} +
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

#######################
# for users with OS X: 
# "brew install findutils" and "brew install gnu-sed" 
# will install "gfind" and "gsed" binaries

update_bedops_copyright_date:
ifndef OLD_CPD
	$(error Old copyright date variable OLD_CPD is undefined (e.g., 2011-2017))
endif
ifndef NEW_CPD
	$(error New copyright date variable NEW_CPD is undefined (e.g., 2011-2020))
endif
ifeq ($(KERNEL), Darwin)
	gfind . -exec gsed -i "s/"$$OLD_CPD"/"$$NEW_CPD"/g" {} \;
else
	find . -exec sed -i "s/"$$OLD_CPD"/"$$NEW_CPD"/g" {} \;
endif

#######################
# packaging

docker: packaging/docker/Dockerfile
	docker build -t bedops -f packaging/docker/Dockerfile  .

rpm: packaging/rpm/Dockerfile
	docker build -t bedops:rpm -f packaging/rpm/Dockerfile .

deb: packaging/deb/Dockerfile
	docker build -t bedops:deb -f packaging/deb/Dockerfile .

#######################
# tests

tests:
	$(MAKE) all -f tests/Makefile
