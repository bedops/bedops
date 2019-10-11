#!/bin/bash -e

boundsFn=$1
samples=$2
maxLength=$3

# rand=$(od -N 4 -t uL -An /dev/urandom | tr -d " ") && rand=$(($rand % 1234))
# rand=$(shuf -i 0-${upperBound} -n ${samples})
# rand=$(python -S -c "import sys; import random; sys.stdout.write(str(random.randrange(0,${upperBound})))")

while IFS='' read -r line || [[ -n "$line" ]]; do
    chr=`printf "$line" | awk '{ print $1 }'`
    chrN=`printf "$chr" | awk '{ sub(/chr/, "", $1); print $1; }'`
    bound=`printf "$line" | awk '{ print $3 }'`
    adjustedBound=$((${bound} - ${maxLength} - 1))
    for sample in `seq 1 ${samples}`; do
        # start=$(od -N 4 -t uL -An /dev/urandom | tr -d " ") && start=$(($start % $adjustedBound))
        # length=$(od -N 4 -t uL -An /dev/urandom | tr -d " ") && length=$(($length % $maxLength))
        # start=$(shuf -i 0-${adjustedBound} -n 1)
        # length=$(shuf -i 0-${maxLength} -n 1)
        # start=$(python -S -c "import sys; import random; sys.stdout.write(str(random.randrange(0,${adjustedBound})))")
        # length=$(python -S -c "import sys; import random; sys.stdout.write(str(random.randrange(0,${maxLength})))")
        # stop=$(($start + $length + 1))
        # echo -e "$chr\t$start\t$stop\tid-$chrN-$sample"
        echo $(python -S -c "import sys; import random; start=random.randrange(0,${adjustedBound}); length=random.randrange(0,${maxLength}); stop=start+length+1; sys.stdout.write('${chr}\t%s\t%s\tid-${chrN}-${sample}\n' % (start, stop))") | tr " " "\t"
    done
done < "${boundsFn}"