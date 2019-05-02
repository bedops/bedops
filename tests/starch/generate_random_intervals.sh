#!/bin/bash -e

boundsFn=$1
samples=$2
maxLength=$3

# rand=$(od -N 4 -t uL -An /dev/urandom | tr -d " ") && rand=$(($rand % 1234))

while IFS='' read -r line || [[ -n "$line" ]]; do
    chr=`printf "$line" | awk '{ print $1 }'`
    chrN=`printf "$chr" | awk '{ sub(/chr/, "", $1); print $1; }'`
    bound=`printf "$line" | awk '{ print $3 }'`
    adjustedBound=$((${bound} - ${maxLength} - 1))
    for sample in `seq 1 ${samples}`; do
        start=$(od -N 4 -t uL -An /dev/urandom | tr -d " ") && start=$(($start % $adjustedBound))
        length=$(od -N 4 -t uL -An /dev/urandom | tr -d " ") && length=$(($length % $maxLength))
        stop=$(($start + $length + 1))
        echo -e "$chr\t$start\t$stop\tid-$chrN-$sample"
    done
done < "${boundsFn}"