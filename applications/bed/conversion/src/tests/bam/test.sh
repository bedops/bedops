#!/bin/bash

bin_dir="/usr/local/bin"
bam2bed_bin="${bin_dir}/bam2bed"
bam2starch_bin="${bin_dir}/bam2starch"

echo "[bam2bed] testing sorted output..."
sample_bam_fn="sample.bam"
expected_sorted_bed_fn="sample.expected.bed"
observed_sorted_bed_fn="$(mktemp /tmp/XXXXXX)"
${bam2bed_bin} < ${sample_bam_fn} > ${observed_sorted_bed_fn} 2> /dev/null
diff -q ${expected_sorted_bed_fn} ${observed_sorted_bed_fn}
rm -f ${observed_sorted_bed_fn}

echo "[bam2bed] testing split output..."
split_bam_fn="split.bam"
expected_split_bed_fn="split.expected.bed"
observed_split_bed_fn="$(mktemp /tmp/XXXXXX)"
${bam2bed_bin} --split < ${split_bam_fn} > ${observed_split_bed_fn} 2> /dev/null
diff -q ${expected_split_bed_fn} ${observed_split_bed_fn}
rm -f ${observed_split_bed_fn}

echo "[bam2bed] testing starch (bzip2) output..."
sample_bam_fn="sample.bam"
expected_starch_fn="sample.expected.starch"
observed_starch_fn="$(mktemp /tmp/XXXXXX)"
${bam2starch_bin} < ${sample_bam_fn} > ${observed_starch_fn} 2> /dev/null
diff -q <(unstarch ${expected_starch_fn}) <(unstarch ${observed_starch_fn})
rm -f ${observed_starch_fn}

echo "[bam2bed] testing starch (gzip) output..."
sample_bam_fn="sample.bam"
expected_starch_fn="sample.expected.starch"
observed_starch_fn="$(mktemp /tmp/XXXXXX)"
${bam2starch_bin} --starch-gzip < ${sample_bam_fn} > ${observed_starch_fn} 2> /dev/null
diff -q <(unstarch ${expected_starch_fn}) <(unstarch ${observed_starch_fn})
rm -f ${observed_starch_fn}

echo "[bam2bed] tests complete!"
