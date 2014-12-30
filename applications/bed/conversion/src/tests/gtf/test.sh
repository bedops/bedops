#!/bin/bash

bin_dir="/usr/local/bin"
gtf2bed_bin="${bin_dir}/gtf2bed"
gtf2starch_bin="${bin_dir}/gtf2starch"

echo "[gtf2bed] testing sorted output..."
sample_gtf_fn="sample.gtf"
expected_sorted_bed_fn="sample.expected.bed"
observed_sorted_bed_fn="$(mktemp /tmp/XXXXXX)"
${gtf2bed_bin} < ${sample_gtf_fn} > ${observed_sorted_bed_fn} 2> /dev/null
diff -q ${expected_sorted_bed_fn} ${observed_sorted_bed_fn}
rm -f ${observed_sorted_bed_fn}

echo "[gtf2bed] testing starch (bzip2) output..."
sample_gtf_fn="sample.gtf"
expected_starch_fn="sample.expected.starch"
observed_starch_fn="$(mktemp /tmp/XXXXXX)"
${gtf2starch_bin} < ${sample_gtf_fn} > ${observed_starch_fn} 2> /dev/null
diff -q <(unstarch ${expected_starch_fn}) <(unstarch ${observed_starch_fn})
rm -f ${observed_starch_fn}

echo "[gtf2bed] testing starch (gzip) output..."
sample_gtf_fn="sample.gtf"
expected_starch_fn="sample.expected.gzip.starch"
observed_starch_fn="$(mktemp /tmp/XXXXXX)"
${gtf2starch_bin} --starch-gzip < ${sample_gtf_fn} > ${observed_starch_fn} 2> /dev/null
diff -q <(unstarch ${expected_starch_fn}) <(unstarch ${observed_starch_fn})
rm -f ${observed_starch_fn}

echo "[gtf2bed] tests complete!"
