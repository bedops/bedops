#!/bin/bash

bin_dir="/usr/local/bin"
gvf2bed_bin="${bin_dir}/gvf2bed"
gvf2starch_bin="${bin_dir}/gvf2starch"

echo "[gvf2bed] testing sorted output..."
sample_gvf_fn="sample.gvf"
expected_sorted_bed_fn="sample.expected.bed"
observed_sorted_bed_fn="$(mktemp /tmp/XXXXXX)"
${gvf2bed_bin} < ${sample_gvf_fn} > ${observed_sorted_bed_fn} 2> /dev/null
diff -q ${expected_sorted_bed_fn} ${observed_sorted_bed_fn}
rm -f ${observed_sorted_bed_fn}

echo "[gvf2bed] testing starch (bzip2) output..."
sample_gvf_fn="sample.gvf"
expected_starch_fn="sample.expected.starch"
observed_starch_fn="$(mktemp /tmp/XXXXXX)"
${gvf2starch_bin} < ${sample_gvf_fn} > ${observed_starch_fn} 2> /dev/null
diff -q <(unstarch ${expected_starch_fn}) <(unstarch ${observed_starch_fn})
rm -f ${observed_starch_fn}

echo "[gvf2bed] testing starch (gzip) output..."
sample_gvf_fn="sample.gvf"
expected_starch_fn="sample.expected.gzip.starch"
observed_starch_fn="$(mktemp /tmp/XXXXXX)"
${gvf2starch_bin} --starch-gzip < ${sample_gvf_fn} > ${observed_starch_fn} 2> /dev/null
diff -q <(unstarch ${expected_starch_fn}) <(unstarch ${observed_starch_fn})
rm -f ${observed_starch_fn}

echo "[gvf2bed] tests complete!"
