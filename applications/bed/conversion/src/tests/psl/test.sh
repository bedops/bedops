#!/bin/bash

bin_dir="/usr/local/bin"
psl2bed_bin="${bin_dir}/psl2bed"
psl2starch_bin="${bin_dir}/psl2starch"

echo "[psl2bed] testing headered output..."
sample_psl_fn="sample.headered.psl"
expected_sorted_bed_fn="sample.expected.headered.bed"
observed_sorted_bed_fn="$(mktemp /tmp/XXXXXX)"
${psl2bed_bin} < ${sample_psl_fn} > ${observed_sorted_bed_fn} 2> /dev/null
diff -q ${expected_sorted_bed_fn} ${observed_sorted_bed_fn}
rm -f ${observed_sorted_bed_fn}

echo "[psl2bed] testing headerless output..."
sample_psl_fn="sample.headerless.psl"
expected_sorted_bed_fn="sample.expected.headerless.bed"
observed_sorted_bed_fn="$(mktemp /tmp/XXXXXX)"
${psl2bed_bin} < ${sample_psl_fn} > ${observed_sorted_bed_fn} 2> /dev/null
diff -q ${expected_sorted_bed_fn} ${observed_sorted_bed_fn}
rm -f ${observed_sorted_bed_fn}

echo "[psl2bed] testing headerless split output..."
sample_psl_fn="sample.headerless.psl"
expected_sorted_bed_fn="sample.expected.headerless.split.bed"
observed_sorted_bed_fn="$(mktemp /tmp/XXXXXX)"
${psl2bed_bin} --split < ${sample_psl_fn} > ${observed_sorted_bed_fn} 2> /dev/null
diff -q ${expected_sorted_bed_fn} ${observed_sorted_bed_fn}
rm -f ${observed_sorted_bed_fn}

echo "[psl2bed] testing headered starch (bzip2) output..."
sample_psl_fn="sample.headered.psl"
expected_starch_fn="sample.expected.headered.starch"
observed_starch_fn="$(mktemp /tmp/XXXXXX)"
${psl2starch_bin} < ${sample_psl_fn} > ${observed_starch_fn} 2> /dev/null
diff -q <(unstarch ${expected_starch_fn}) <(unstarch ${observed_starch_fn})
rm -f ${observed_starch_fn}

echo "[psl2bed] testing headered starch (gzip) output..."
sample_psl_fn="sample.headered.psl"
expected_starch_fn="sample.expected.headered.gzip.starch"
observed_starch_fn="$(mktemp /tmp/XXXXXX)"
${psl2starch_bin} --starch-gzip < ${sample_psl_fn} > ${observed_starch_fn} 2> /dev/null
diff -q <(unstarch ${expected_starch_fn}) <(unstarch ${observed_starch_fn})
rm -f ${observed_starch_fn}

echo "[psl2bed] testing headerless starch (bzip2) output..."
sample_psl_fn="sample.headerless.psl"
expected_starch_fn="sample.expected.headerless.starch"
observed_starch_fn="$(mktemp /tmp/XXXXXX)"
${psl2starch_bin} < ${sample_psl_fn} > ${observed_starch_fn} 2> /dev/null
diff -q <(unstarch ${expected_starch_fn}) <(unstarch ${observed_starch_fn})
rm -f ${observed_starch_fn}

echo "[psl2bed] testing headerless split starch (bzip2) output..."
sample_psl_fn="sample.headerless.psl"
expected_starch_fn="sample.expected.headerless.split.starch"
observed_starch_fn="$(mktemp /tmp/XXXXXX)"
${psl2starch_bin} --split < ${sample_psl_fn} > ${observed_starch_fn} 2> /dev/null
diff -q <(unstarch ${expected_starch_fn}) <(unstarch ${observed_starch_fn})
rm -f ${observed_starch_fn}

echo "[psl2bed] testing headerless starch (gzip) output..."
sample_psl_fn="sample.headerless.psl"
expected_starch_fn="sample.expected.headerless.gzip.starch"
observed_starch_fn="$(mktemp /tmp/XXXXXX)"
${psl2starch_bin} --starch-gzip < ${sample_psl_fn} > ${observed_starch_fn} 2> /dev/null
diff -q <(unstarch ${expected_starch_fn}) <(unstarch ${observed_starch_fn})
rm -f ${observed_starch_fn}

echo "[psl2bed] testing headerless split starch (gzip) output..."
sample_psl_fn="sample.headerless.psl"
expected_starch_fn="sample.expected.headerless.split.gzip.starch"
observed_starch_fn="$(mktemp /tmp/XXXXXX)"
${psl2starch_bin} --starch-gzip --split < ${sample_psl_fn} > ${observed_starch_fn} 2> /dev/null
diff -q <(unstarch ${expected_starch_fn}) <(unstarch ${observed_starch_fn})
rm -f ${observed_starch_fn}

echo "[psl2bed] tests complete!"
