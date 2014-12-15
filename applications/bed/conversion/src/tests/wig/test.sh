#!/bin/bash

bin_dir="/usr/local/bin"
wig2bed_bin="${bin_dir}/wig2bed"
wig2starch_bin="${bin_dir}/wig2starch"

for idx in $(seq 1 5)
do
    echo "[wig2bed] testing output [$idx]..." 
    sample_wig_fn="sample_$idx.wig"
    expected_sorted_bed_fn="sample_$idx.expected.bed"
    observed_sorted_bed_fn="$(mktemp /tmp/XXXXXX)"
    ${wig2bed_bin} < ${sample_wig_fn} > ${observed_sorted_bed_fn} 2> /dev/null
    diff -q ${expected_sorted_bed_fn} ${observed_sorted_bed_fn}
    rm -f ${observed_sorted_bed_fn}

    echo "[wig2bed] testing starch (bzip2) output [$idx]..."
    sample_wig_fn="sample_$idx.wig"
    expected_starch_fn="sample_$idx.expected.starch"
    observed_starch_fn="$(mktemp /tmp/XXXXXX)"
    ${wig2starch_bin} < ${sample_wig_fn} > ${observed_starch_fn} 2> /dev/null
    diff -q <(unstarch ${expected_starch_fn}) <(unstarch ${observed_starch_fn})
    rm -f ${observed_starch_fn}

    echo "[wig2bed] testing starch (gzip) output [$idx]..."
    sample_wig_fn="sample_$idx.wig"
    expected_starch_fn="sample_$idx.expected.gzip.starch"
    observed_starch_fn="$(mktemp /tmp/XXXXXX)"
    ${wig2starch_bin} --starch-gzip < ${sample_wig_fn} > ${observed_starch_fn} 2> /dev/null
    diff -q <(unstarch ${expected_starch_fn}) <(unstarch ${observed_starch_fn})
    rm -f ${observed_starch_fn}
done

idx=6

echo "[wig2bed] testing output [$idx]..." 
sample_wig_fn="sample_$idx.wig"
expected_sorted_bed_fn="sample_$idx.expected.bed"
observed_sorted_bed_fn="$(mktemp /tmp/XXXXXX)"
${wig2bed_bin} --keep-header --multisplit="foo" < ${sample_wig_fn} > ${observed_sorted_bed_fn} 2> /dev/null
diff -q ${expected_sorted_bed_fn} ${observed_sorted_bed_fn}
rm -f ${observed_sorted_bed_fn}

echo "[wig2bed] testing starch (bzip2) output [$idx]..."
sample_wig_fn="sample_$idx.wig"
expected_starch_fn="sample_$idx.expected.starch"
observed_starch_fn="$(mktemp /tmp/XXXXXX)"
${wig2starch_bin} --keep-header --multisplit="foo" < ${sample_wig_fn} > ${observed_starch_fn} 2> /dev/null
diff -q <(unstarch ${expected_starch_fn}) <(unstarch ${observed_starch_fn})
rm -f ${observed_starch_fn}

echo "[wig2bed] testing starch (gzip) output [$idx]..."
sample_wig_fn="sample_$idx.wig"
expected_starch_fn="sample_$idx.expected.gzip.starch"
observed_starch_fn="$(mktemp /tmp/XXXXXX)"
${wig2starch_bin} --starch-gzip --keep-header --multisplit="foo" < ${sample_wig_fn} > ${observed_starch_fn} 2> /dev/null
diff -q <(unstarch ${expected_starch_fn}) <(unstarch ${observed_starch_fn})
rm -f ${observed_starch_fn}

echo "[wig2bed] tests complete!"
