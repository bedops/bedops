#!/bin/bash

bin_dir="/usr/local/bin"
gff2bed_bin="${bin_dir}/gff2bed"
gff2starch_bin="${bin_dir}/gff2starch"

echo "[gff2bed] testing sorted output..."
sample_gff_fn="sample.gff"
expected_sorted_bed_fn="sample.expected.bed"
observed_sorted_bed_fn="$(mktemp /tmp/XXXXXX)"
${gff2bed_bin} < ${sample_gff_fn} > ${observed_sorted_bed_fn} 2> /dev/null
diff -q ${expected_sorted_bed_fn} ${observed_sorted_bed_fn}
rm -f ${observed_sorted_bed_fn}

echo "[gff2bed] testing starch (bzip2) output..."
sample_gff_fn="sample.gff"
expected_starch_fn="sample.expected.starch"
observed_starch_fn="$(mktemp /tmp/XXXXXX)"
${gff2starch_bin} < ${sample_gff_fn} > ${observed_starch_fn} 2> /dev/null
diff -q <(unstarch ${expected_starch_fn}) <(unstarch ${observed_starch_fn})
rm -f ${observed_starch_fn}

echo "[gff2bed] testing starch (gzip) output..."
sample_gff_fn="sample.gff"
expected_starch_fn="sample.expected.gzip.starch"
observed_starch_fn="$(mktemp /tmp/XXXXXX)"
${gff2starch_bin} --starch-gzip < ${sample_gff_fn} > ${observed_starch_fn} 2> /dev/null
diff -q <(unstarch ${expected_starch_fn}) <(unstarch ${observed_starch_fn})
rm -f ${observed_starch_fn}

echo "[gff2bed] tests complete!"
