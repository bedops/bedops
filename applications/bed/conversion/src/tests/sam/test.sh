#!/bin/bash

bin_dir="/usr/local/bin"
sam2bed_bin="${bin_dir}/sam2bed"
sam2starch_bin="${bin_dir}/sam2starch"

echo "[sam2bed] testing sorted output..."
sample_sam_fn="sample.sam"
expected_sorted_bed_fn="sample.expected.bed"
observed_sorted_bed_fn="$(mktemp /tmp/XXXXXX)"
${sam2bed_bin} < ${sample_sam_fn} > ${observed_sorted_bed_fn} 2> /dev/null
diff -q ${expected_sorted_bed_fn} ${observed_sorted_bed_fn}
rm -f ${observed_sorted_bed_fn}

echo "[sam2bed] testing split output..."
split_sam_fn="split.sam"
expected_split_bed_fn="split.expected.bed"
observed_split_bed_fn="$(mktemp /tmp/XXXXXX)"
${sam2bed_bin} --split < ${split_sam_fn} > ${observed_split_bed_fn} 2> /dev/null
diff -q ${expected_split_bed_fn} ${observed_split_bed_fn}
rm -f ${observed_split_bed_fn}

echo "[sam2bed] testing starch (bzip2) output..."
sample_sam_fn="sample.sam"
expected_starch_fn="sample.expected.starch"
observed_starch_fn="$(mktemp /tmp/XXXXXX)"
${sam2starch_bin} < ${sample_sam_fn} > ${observed_starch_fn} 2> /dev/null
diff -q <(unstarch ${expected_starch_fn}) <(unstarch ${observed_starch_fn})
rm -f ${observed_starch_fn}

echo "[sam2bed] testing starch (gzip) output..."
sample_sam_fn="sample.sam"
expected_starch_fn="sample.expected.starch"
observed_starch_fn="$(mktemp /tmp/XXXXXX)"
${sam2starch_bin} --starch-gzip < ${sample_sam_fn} > ${observed_starch_fn} 2> /dev/null
diff -q <(unstarch ${expected_starch_fn}) <(unstarch ${observed_starch_fn})
rm -f ${observed_starch_fn}

echo "[sam2bed] tests complete!"
