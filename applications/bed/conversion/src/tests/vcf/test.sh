#!/bin/bash

bin_dir="/usr/local/bin"
vcf2bed_bin="${bin_dir}/vcf2bed"
vcf2starch_bin="${bin_dir}/vcf2starch"

echo "[vcf2bed] testing sorted and split output..."
sample_split_vcf_fn="sample.vcf"
expected_split_sorted_bed_fn="sample.expected.split.bed"
observed_split_sorted_bed_fn="$(mktemp /tmp/XXXXXX)"
${vcf2bed_bin} < ${sample_split_vcf_fn} > ${observed_split_sorted_bed_fn} 2> /dev/null
diff -q ${expected_split_sorted_bed_fn} ${observed_split_sorted_bed_fn}
rm -f ${observed_split_sorted_bed_fn}

echo "[vcf2bed] testing sorted and no-split output..."
sample_nosplit_vcf_fn="sample.vcf"
expected_nosplit_sorted_bed_fn="sample.expected.nosplit.bed"
observed_nosplit_sorted_bed_fn="$(mktemp /tmp/XXXXXX)"
${vcf2bed_bin} --do-not-split < ${sample_nosplit_vcf_fn} > ${observed_nosplit_sorted_bed_fn} 2> /dev/null
diff -q ${expected_nosplit_sorted_bed_fn} ${observed_nosplit_sorted_bed_fn}
rm -f ${observed_nosplit_sorted_bed_fn}

echo "[vcf2bed] testing starch split (bzip2) output..."
sample_vcf_fn="sample.vcf"
expected_split_starch_fn="sample.expected.split.bzip2.starch"
observed_split_starch_fn="$(mktemp /tmp/XXXXXX)"
${vcf2starch_bin} < ${sample_vcf_fn} > ${observed_split_starch_fn} 2> /dev/null
diff -q <(unstarch ${expected_split_starch_fn}) <(unstarch ${observed_split_starch_fn})
rm -f ${observed_split_starch_fn}

echo "[vcf2bed] testing starch (gzip) output..."
sample_vcf_fn="sample.vcf"
expected_split_starch_fn="sample.expected.split.gzip.starch"
observed_split_starch_fn="$(mktemp /tmp/XXXXXX)"
${vcf2starch_bin} --starch-gzip < ${sample_vcf_fn} > ${observed_split_starch_fn} 2> /dev/null
diff -q <(unstarch ${expected_split_starch_fn}) <(unstarch ${observed_split_starch_fn})
rm -f ${observed_split_starch_fn}

echo "[vcf2bed] testing starch no-split (bzip2) output..."
sample_vcf_fn="sample.vcf"
expected_split_starch_fn="sample.expected.nosplit.bzip2.starch"
observed_split_starch_fn="$(mktemp /tmp/XXXXXX)"
${vcf2starch_bin} --do-not-split < ${sample_vcf_fn} > ${observed_split_starch_fn} 2> /dev/null
diff -q <(unstarch ${expected_split_starch_fn}) <(unstarch ${observed_split_starch_fn})
rm -f ${observed_split_starch_fn}

echo "[vcf2bed] testing starch (gzip) output..."
sample_vcf_fn="sample.vcf"
expected_split_starch_fn="sample.expected.nosplit.gzip.starch"
observed_split_starch_fn="$(mktemp /tmp/XXXXXX)"
${vcf2starch_bin} --do-not-split --starch-gzip < ${sample_vcf_fn} > ${observed_split_starch_fn} 2> /dev/null
diff -q <(unstarch ${expected_split_starch_fn}) <(unstarch ${observed_split_starch_fn})
rm -f ${observed_split_starch_fn}

echo "[vcf2bed] tests complete!"
