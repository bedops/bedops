#!/usr/bin/env bash

for i in $*
do
case $i in
    -e=*|--elements=*)
    ELEMENTS=`echo $i | sed 's/[-a-zA-Z0-9]*=//'`
    ;;
    *)
            # unknown option
    ;;
esac
done

set -e
if [ -z "${ELEMENTS}" ]; then echo "ERROR: Set the --elements=n variable"; exit; fi

mysql -N --user=genome --host=genome-mysql.cse.ucsc.edu -A -D hg19 << EOF
	SET @rank:=0;
	SELECT DISTINCT chrom as chromcol,
		@start:=ROUND(RAND()*(size-100)) as startcol,
		@start+ROUND(RAND()*100)+1 as stopcol,
		CONCAT("id-",@rank:=@rank+1) as idcol,
		ROUND(RAND()*1000) as scorecol,
		IF(RAND()<0.5,"+","-") asstrandcol
	FROM
		chromInfo, kgXref 
	LIMIT ${ELEMENTS}

EOF
