#
# makefile
#

TMP = /tmp
USER := $(shell whoami)
SRC = src
DATADIR := ${TMP}/${USER}/starch_regression_test/data

# 1M element file
ELEMENTS = 1000000

all: random.bed

random.bed:
	@echo "[STARCH] making ${ELEMENTS}-element, randomly-generated BED file..."
	@mkdir -p ${DATADIR}
	@test -s ${DATADIR}/$@ || ${SRC}/make_unsorted_random_bed_file.sh --elements=${ELEMENTS} | ${SRC}/jumble_duplicate_coords.pl | sort-bed - > ${DATADIR}/$@

#
# tip: one can use 
#
# 	$ awk '{key=$1":"$2":"$3; if (a[key]) { print "dup"; }}' ${DATADIR}/random.bed 
#
# to verify there are no duplicates
#

clean:
	@echo "[STARCH] removing randomly-generated BED data (if present)..."
	@rm -rf ${DATADIR}/random.bed
