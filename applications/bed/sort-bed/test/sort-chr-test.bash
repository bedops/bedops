#!/bin/bash

for i in {1..1024}
do
    for j in {1..1024}
    do
        if [ ${i} -ge ${j} ]
        then
            echo -e "chr${j}\t1\t2" >> chr${i}.bed
        fi
    done
    sort-bed --max-mem 2G chr${i}.bed > /dev/null
    if [ $? -ne 0 ]
    then
        echo "failed on chr count [ ${i} ]"
        exit -1
    else
        rm chr${i}.bed
    fi
done
