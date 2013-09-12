THIS_DIR     = ${shell pwd}

all: input_data tests
	@echo "Congratulations, all Starch tests passed!"

input_data: random_bed_file 

tests: compression_and_extraction_tests concatenation_tests updating_tests metadata_integrity_tests

#
# setup of original (random) BED data
#

ORIGINAL_DIR = ${THIS_DIR}/original_data
RANDOM_BED   = ${ORIGINAL_DIR}/data/random.bed

random_bed_file:
	@make -C ${ORIGINAL_DIR} -f makefile random.bed

#
# compression and extraction
#

COMPEXT_DIR  = ${THIS_DIR}/compression_and_extraction

compression_and_extraction_tests: random_bed_file
	@make -C ${COMPEXT_DIR} -f makefile all

#
# concatenation
#

CONCAT_DIR   = ${THIS_DIR}/concatenation

concatenation_tests: random_bed_file 
	@make -C ${CONCAT_DIR} -f makefile all

#
# updating
#

UPDATING_DIR = ${THIS_DIR}/updating

updating_tests: random_bed_file compression_and_extraction_tests
	@make -C ${UPDATING_DIR} -f makefile all

#
# metadata integrity
#

MD_INTEG_DIR = ${THIS_DIR}/metadata_integrity

metadata_integrity_tests: random_bed_file compression_and_extraction_tests
	@make -C ${MD_INTEG_DIR} -f makefile all

#
# clean-up
#

clean:
	@echo "[STARCH] cleaning up files..."
	@make -C ${ORIGINAL_DIR} -f makefile clean
	@make -C ${COMPEXT_DIR} -f makefile clean
	@make -C ${CONCAT_DIR} -f makefile clean
	@make -C ${UPDATING_DIR} -f makefile clean
	@make -C ${MD_INTEG_DIR} -f makefile clean
