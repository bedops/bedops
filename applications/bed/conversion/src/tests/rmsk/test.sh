#!/bin/bash

bin_dir="/usr/local/bin"
rmsk2bed_bin="${bin_dir}/rmsk2bed"
rmsk2starch_bin="${bin_dir}/rmsk2starch"

echo "[rmsk2bed] testing sorted output..."
sample_rmsk_fn="sample.out"
expected_sorted_bed_fn="sample.expected.bed"
observed_sorted_bed_fn="$(mktemp /tmp/XXXXXX)"
${rmsk2bed_bin} < ${sample_rmsk_fn} > ${observed_sorted_bed_fn} 2> /dev/null
diff -q ${expected_sorted_bed_fn} ${observed_sorted_bed_fn}
rm -f ${observed_sorted_bed_fn}

echo "[rmsk2bed] testing starch (bzip2) output..."
sample_rmsk_fn="sample.out"
expected_starch_fn="sample.expected.starch"
observed_starch_fn="$(mktemp /tmp/XXXXXX)"
${rmsk2starch_bin} < ${sample_rmsk_fn} > ${observed_starch_fn} 2> /dev/null
diff -q <(unstarch ${expected_starch_fn}) <(unstarch ${observed_starch_fn})
rm -f ${observed_starch_fn}

echo "[rmsk2bed] testing starch (gzip) output..."
sample_rmsk_fn="sample.out"
expected_starch_fn="sample.expected.gzip.starch"
observed_starch_fn="$(mktemp /tmp/XXXXXX)"
${rmsk2starch_bin} --starch-gzip < ${sample_rmsk_fn} > ${observed_starch_fn} 2> /dev/null
diff -q <(unstarch ${expected_starch_fn}) <(unstarch ${observed_starch_fn})
rm -f ${observed_starch_fn}

echo "[rmsk2bed] testing sorted output (2)..."
sample_rmsk_fn="sample2.out"
expected_sorted_bed_fn="sample2.expected.bed"
observed_sorted_bed_fn="$(mktemp /tmp/XXXXXX)"
${rmsk2bed_bin} < ${sample_rmsk_fn} > ${observed_sorted_bed_fn} 2> /dev/null
diff -q ${expected_sorted_bed_fn} ${observed_sorted_bed_fn}
rm -f ${observed_sorted_bed_fn}

echo "[rmsk2bed] testing starch (bzip2) output (2)..."
sample_rmsk_fn="sample2.out"
expected_starch_fn="sample2.expected.starch"
observed_starch_fn="$(mktemp /tmp/XXXXXX)"
${rmsk2starch_bin} < ${sample_rmsk_fn} > ${observed_starch_fn} 2> /dev/null
diff -q <(unstarch ${expected_starch_fn}) <(unstarch ${observed_starch_fn})
rm -f ${observed_starch_fn}

echo "[rmsk2bed] testing starch (gzip) output (2)..."
sample_rmsk_fn="sample2.out"
expected_starch_fn="sample2.expected.gzip.starch"
observed_starch_fn="$(mktemp /tmp/XXXXXX)"
${rmsk2starch_bin} --starch-gzip < ${sample_rmsk_fn} > ${observed_starch_fn} 2> /dev/null
diff -q <(unstarch ${expected_starch_fn}) <(unstarch ${observed_starch_fn})
rm -f ${observed_starch_fn}

echo "[rmsk2bed] tests complete!"
