#
# makefile
#

SRC = src
DATADIR = data

# 1M element file
ELEMENTS = 1000000

all: random.bed

random.bed:
	@echo "[STARCH] making ${ELEMENTS}-element, randomly-generated BED file..."
	@test -s ${DATADIR}/$@ || ${SRC}/make_unsorted_random_bed_file.sh --elements=${ELEMENTS} | sort-bed - > ${DATADIR}/$@

clean:
	@echo "[STARCH] removing randomly-generated BED data (if present)..."
	@rm -rf ${DATADIR}/random.bed
