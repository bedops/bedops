export KERNEL      := ${shell uname -a | cut -f1 -d' '}
APPDIR              = applications/bed
OTHERDIR            = applications/other
BINDIR              = bin
MEGABINDIR          = bin/megarow
OSXPKGROOT          = packaging/os_x
OSXBUILDDIR         = ${OSXPKGROOT}/build
OSXPKGDIR           = ${OSXPKGROOT}/resources/bin
OSXLIBDIR           = ${OSXPKGROOT}/resources/lib
SELF                = ${shell pwd}/Makefile
MASSIVE_REST_EXP    = 22
MASSIVE_ID_EXP      = 14
MASSIVE_CHROM_EXP   = 8
JPARALLEL           = 8
MEGA                = megarow
TYPICAL             = typical
DEFAULT_BINARY_TYPE = ${TYPICAL} 
WRAPPERS            = $(wildcard ${APPDIR}/conversion/src/wrappers/*)

default:
ifeq ($(KERNEL), Darwin)
ifeq ($(BINARY_TYPE),)
	$(MAKE) BINARY_TYPE=$(DEFAULT_BINARY_TYPE) $(MAKECMDGOALS) -j $(JPARALLEL) -f system.mk/Makefile.darwin
else
	$(MAKE) $(MAKECMDGOALS) -j $(JPARALLEL) -f system.mk/Makefile.darwin
endif
else
ifeq ($(BINARY_TYPE),)
	$(MAKE) BINARY_TYPE=$(DEFAULT_BINARY_TYPE) $(MAKECMDGOALS) -j $(JPARALLEL) -f system.mk/Makefile.linux
else
	$(MAKE) $(MAKECMDGOALS) -j $(JPARALLEL) -f system.mk/Makefile.linux
endif
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

megarow:
	$(MAKE) BINARY_TYPE=$(MEGA) MEGAFLAGS="-DREST_EXPONENT=${MASSIVE_REST_EXP} -DID_EXPONENT=${MASSIVE_ID_EXP} -DCHROM_EXPONENT=${MASSIVE_CHROM_EXP}" -f ${SELF}

typical:
	$(MAKE) BINARY_TYPE=$(TYPICAL) -f ${SELF}

symlink_typical:
	$(eval variablename=`find $(BINDIR)/ -maxdepth 1 -mindepth 1 -type f -name '*$(TYPICAL)' -print0 | xargs -L1 -0 -I{} sh -c 'basename {}'`)
	for i in ${variablename}; do \
		fooname=`echo $$i | sed 's/-$(TYPICAL)//'`; \
		echo $${fooname}; \
		ln -sf $$i $(BINDIR)/$${fooname}; \
	done

symlink_megarow:
	$(eval variablename=`find $(BINDIR)/ -maxdepth 1 -mindepth 1 -type f -name '*$(MEGA)' -print0 | xargs -L1 -0 -I{} sh -c 'basename {}'`)
	for i in ${variablename}; do \
		fooname=`echo $$i | sed 's/-$(MEGA)//'`; \
		echo $${fooname}; \
		ln -sf $$i $(BINDIR)/$${fooname}; \
	done

install: prep_c install_conversion_scripts install_starch_scripts
	-cp ${APPDIR}/sort-bed/bin/sort-bed-$(DEFAULT_BINARY_TYPE) ${BINDIR}/sort-bed
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-slurm-$(DEFAULT_BINARY_TYPE) ${BINDIR}/update-sort-bed-slurm
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-starch-slurm-$(DEFAULT_BINARY_TYPE) ${BINDIR}/update-sort-bed-starch-slurm
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-migrate-candidates-$(DEFAULT_BINARY_TYPE) ${BINDIR}/update-sort-bed-migrate-candidates
	-cp ${APPDIR}/bedops/bin/bedops-$(DEFAULT_BINARY_TYPE) ${BINDIR}/bedops
	-cp ${APPDIR}/closestfeats/bin/closest-features-$(DEFAULT_BINARY_TYPE) ${BINDIR}/closest-features
	-cp ${APPDIR}/bedmap/bin/bedmap-$(DEFAULT_BINARY_TYPE) ${BINDIR}/bedmap
	-cp ${APPDIR}/bedextract/bin/bedextract-$(DEFAULT_BINARY_TYPE) ${BINDIR}/bedextract
	-cp ${APPDIR}/starch/bin/starch-$(DEFAULT_BINARY_TYPE) ${BINDIR}/starch
	-cp ${APPDIR}/starch/bin/unstarch-$(DEFAULT_BINARY_TYPE) ${BINDIR}/unstarch
	-cp ${APPDIR}/starch/bin/starchcat-$(DEFAULT_BINARY_TYPE) ${BINDIR}/starchcat
	-cp ${APPDIR}/starch/bin/starchstrip-$(DEFAULT_BINARY_TYPE) ${BINDIR}/starchstrip
	-cp ${APPDIR}/conversion/bin/convert2bed-$(DEFAULT_BINARY_TYPE) ${BINDIR}/convert2bed

install_all: install_conversion_scripts_with_suffix install_starch_scripts_with_suffix
	-cp ${APPDIR}/sort-bed/bin/sort-bed-$(MEGA) ${BINDIR}/sort-bed-$(MEGA)
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-slurm-$(MEGA) ${BINDIR}/update-sort-bed-slurm-$(MEGA)
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-starch-slurm-$(MEGA) ${BINDIR}/update-sort-bed-starch-slurm-$(MEGA)
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-migrate-candidates-$(MEGA) ${BINDIR}/update-sort-bed-migrate-candidates-$(MEGA)
	-cp ${APPDIR}/bedops/bin/bedops-$(MEGA) ${BINDIR}/bedops-$(MEGA)
	-cp ${APPDIR}/closestfeats/bin/closest-features-$(MEGA) ${BINDIR}/closest-features-$(MEGA)
	-cp ${APPDIR}/bedmap/bin/bedmap-$(MEGA) ${BINDIR}/bedmap-$(MEGA)
	-cp ${APPDIR}/bedextract/bin/bedextract-$(MEGA) ${BINDIR}/bedextract-$(MEGA)
	-cp ${APPDIR}/starch/bin/starch-$(MEGA) ${BINDIR}/starch-$(MEGA)
	-cp ${APPDIR}/starch/bin/unstarch-$(MEGA) ${BINDIR}/unstarch-$(MEGA)
	-cp ${APPDIR}/starch/bin/starchcat-$(MEGA) ${BINDIR}/starchcat-$(MEGA)
	-cp ${APPDIR}/starch/bin/starchstrip-$(MEGA) ${BINDIR}/starchstrip-$(MEGA)
	-cp ${APPDIR}/conversion/bin/convert2bed-$(MEGA) ${BINDIR}/convert2bed-$(MEGA)
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


#######################
# install details

prep_c:
	mkdir -p ${BINDIR}

install_debug: prep_c install_conversion_scripts install_starch_scripts
	-cp ${APPDIR}/sort-bed/bin/debug.sort-bed-$(DEFAULT_BINARY_TYPE) ${BINDIR}/debug.sort-bed
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-slurm-$(DEFAULT_BINARY_TYPE) ${BINDIR}/update-sort-bed-slurm
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-starch-slurm-$(DEFAULT_BINARY_TYPE) ${BINDIR}/update-sort-bed-starch-slurm
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-migrate-candidates-$(DEFAULT_BINARY_TYPE) ${BINDIR}/update-sort-bed-migrate-candidates
	-cp ${APPDIR}/bedops/bin/debug.bedops-$(DEFAULT_BINARY_TYPE) ${BINDIR}/debug.bedops
	-cp ${APPDIR}/closestfeats/bin/debug.closest-features-$(DEFAULT_BINARY_TYPE) ${BINDIR}/debug.closest-features
	-cp ${APPDIR}/bedmap/bin/debug.bedmap-$(DEFAULT_BINARY_TYPE) ${BINDIR}/debug.bedmap
	-cp ${APPDIR}/bedextract/bin/debug.bedextract-$(DEFAULT_BINARY_TYPE) ${BINDIR}/debug.bedextract
	-cp ${APPDIR}/starch/bin/debug.starch-$(DEFAULT_BINARY_TYPE) ${BINDIR}/debug.starch
	-cp ${APPDIR}/starch/bin/debug.unstarch-$(DEFAULT_BINARY_TYPE) ${BINDIR}/debug.unstarch
	-cp ${APPDIR}/starch/bin/debug.starchcat-$(DEFAULT_BINARY_TYPE) ${BINDIR}/debug.starchcat
	-cp ${APPDIR}/starch/bin/debug.starchstrip-$(DEFAULT_BINARY_TYPE) ${BINDIR}/debug.starchstrip
	-cp ${APPDIR}/conversion/bin/debug.convert2bed-$(DEFAULT_BINARY_TYPE) ${BINDIR}/debug.convert2bed

install_gprof: prep_c install_conversion_scripts install_starch_scripts
	-cp ${APPDIR}/sort-bed/bin/gprof.sort-bed-$(DEFAULT_BINARY_TYPE) ${BINDIR}/gprof.sort-bed
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-slurm-$(DEFAULT_BINARY_TYPE) ${BINDIR}/update-sort-bed-slurm
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-starch-slurm-$(DEFAULT_BINARY_TYPE) ${BINDIR}/update-sort-bed-starch-slurm
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-migrate-candidates-$(DEFAULT_BINARY_TYPE) ${BINDIR}/update-sort-bed-migrate-candidates
	-cp ${APPDIR}/bedops/bin/gprof.bedops-$(DEFAULT_BINARY_TYPE) ${BINDIR}/gprof.bedops
	-cp ${APPDIR}/closestfeats/bin/gprof.closest-features-$(DEFAULT_BINARY_TYPE) ${BINDIR}/gprof.closest-features
	-cp ${APPDIR}/bedmap/bin/gprof.bedmap-$(DEFAULT_BINARY_TYPE) ${BINDIR}/gprof.bedmap
	-cp ${APPDIR}/bedextract/bin/gprof.bedextract-$(DEFAULT_BINARY_TYPE) ${BINDIR}/gprof.bedextract
	-cp ${APPDIR}/starch/bin/gprof.starch-$(DEFAULT_BINARY_TYPE) ${BINDIR}/gprof.starch
	-cp ${APPDIR}/starch/bin/gprof.unstarch-$(DEFAULT_BINARY_TYPE) ${BINDIR}/gprof.unstarch
	-cp ${APPDIR}/starch/bin/gprof.starchcat-$(DEFAULT_BINARY_TYPE) ${BINDIR}/gprof.starchcat
	-cp ${APPDIR}/starch/bin/gprof.starchstrip-$(DEFAULT_BINARY_TYPE) ${BINDIR}/gprof.starchstrip
	-cp ${APPDIR}/conversion/bin/gprof.convert2bed-$(DEFAULT_BINARY_TYPE) ${BINDIR}/gprof.convert2bed

install_starch_scripts: prep_c
	-cp ${APPDIR}/starch/bin/starchcluster_sge-$(DEFAULT_BINARY_TYPE) ${BINDIR}/starchcluster_sge
	-cp ${APPDIR}/starch/bin/starchcluster_gnuParallel-$(DEFAULT_BINARY_TYPE) ${BINDIR}/starchcluster_gnuParallel
	-cp ${APPDIR}/starch/bin/starchcluster_slurm-$(DEFAULT_BINARY_TYPE) ${BINDIR}/starchcluster_slurm
	-cp ${APPDIR}/starch/bin/starch-diff-$(DEFAULT_BINARY_TYPE) ${BINDIR}/starch-diff

install_starch_scripts_with_suffix: prep_c
	-cp ${APPDIR}/starch/bin/starchcluster_sge-$(TYPICAL) ${BINDIR}/starchcluster_sge-$(TYPICAL)
	-cp ${APPDIR}/starch/bin/starchcluster_gnuParallel-$(TYPICAL) ${BINDIR}/starchcluster_gnuParallel-$(TYPICAL)
	-cp ${APPDIR}/starch/bin/starchcluster_slurm-$(TYPICAL) ${BINDIR}/starchcluster_slurm-$(TYPICAL)
	-cp ${APPDIR}/starch/bin/starch-diff-$(TYPICAL) ${BINDIR}/starch-diff-$(TYPICAL)
	-cp ${APPDIR}/starch/bin/starchcluster_sge-$(MEGA) ${BINDIR}/starchcluster_sge-$(MEGA)
	-cp ${APPDIR}/starch/bin/starchcluster_gnuParallel-$(MEGA) ${BINDIR}/starchcluster_gnuParallel-$(MEGA)
	-cp ${APPDIR}/starch/bin/starchcluster_slurm-$(MEGA) ${BINDIR}/starchcluster_slurm-$(MEGA)
	-cp ${APPDIR}/starch/bin/starch-diff-$(MEGA) ${BINDIR}/starch-diff-$(MEGA)

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
	cp $@ $(patsubst %,$(BINDIR)/%-$(MEGA), $(notdir $@))

install_osx_packaging_bins: prep_c
	mkdir -p ${OSXPKGDIR}
	-cp ${APPDIR}/sort-bed/bin/sort-bed ${OSXPKGDIR}/
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-slurm ${OSXPKGDIR}/
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-starch-slurm ${OSXPKGDIR}/
	-cp ${APPDIR}/sort-bed/bin/update-sort-bed-migrate-candidates ${OSXPKGDIR}/
	-cp ${APPDIR}/bedops/bin/bedops ${OSXPKGDIR}/
	-cp ${APPDIR}/closestfeats/bin/closest-features ${OSXPKGDIR}/
	-cp ${APPDIR}/bedmap/bin/bedmap ${OSXPKGDIR}/
	-cp ${APPDIR}/bedextract/bin/bedextract ${OSXPKGDIR}/
	-cp ${APPDIR}/starch/bin/starch ${OSXPKGDIR}/
	-cp ${APPDIR}/starch/bin/unstarch ${OSXPKGDIR}/
	-cp ${APPDIR}/starch/bin/starchcat ${OSXPKGDIR}/
	-cp ${APPDIR}/starch/bin/starchstrip ${OSXPKGDIR}/
	-cp ${APPDIR}/starch/bin/starchcluster_sge ${OSXPKGDIR}/starchcluster_sge
	-cp ${APPDIR}/starch/bin/starchcluster_gnuParallel ${OSXPKGDIR}/starchcluster_gnuParallel
	-cp ${APPDIR}/starch/bin/starchcluster_slurm ${OSXPKGDIR}/starchcluster_slurm
	-cp ${APPDIR}/starch/bin/starch-diff ${OSXPKGDIR}/starch-diff
	-cp ${APPDIR}/conversion/bin/convert2bed ${OSXPKGDIR}/
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
	-cp ${OTHERDIR}/switch-BEDOPS-binary-type ${OSXPKGDIR}/switch-BEDOPS-binary-type
	mkdir -p ${OSXLIBDIR}

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
