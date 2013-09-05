#!/bin/tcsh -efx

set snps = GWAS_SNPs.bed
set dhss = (LNCaP_DHS.bed PrEC_DHS.bed CACO2_DHS.bed HEPG2_DHS.bed K562_DHS.bed MCF7_DHS.bed)

sort-bed $snps > $snps:r.sorted.bed


rm -f SNP_DHS_matrix.bed
set column_names = ("Names")
foreach dhs ($dhss)
  set column_names = ($column_names $dhs:r)

  # add a column of DHS overlap counts to the output matrix
  if ( ! -s SNP_DHS_matrix.bed ) then
    # first entry -> include SNP description as the first few columns of output
    bedmap --ec --delim "\t" --bp-ovr 1 --echo --count GWAS_SNPs.sorted.bed $dhs > SNP_DHS_matrix.bed
  else
    # paste on the counts
    bedmap --ec --delim "\t" --bp-ovr 1 --count GWAS_SNPs.sorted.bed $dhs \
      | paste SNP_DHS_matrix.bed - \
      > tmp.bed

    mv tmp.bed SNP_DHS_matrix.bed
  endif
end


# sort by disease trait and condense data fields into matrix2png form
sort -k5dr SNP_DHS_matrix.bed \
  | awk '{ \
           printf $1":"$2"-"$3"-"$4"-"$5"-"$6; \
           for ( i = 7; i <= NF; ++i ) { \
             printf "\t"$(i); \
           } \
           printf "\n"; \
         }' \
  > SNP_DHS_matrix.txt

# add header information
echo $column_names \
  | tr ' ' '\t' \
  | cat - SNP_DHS_matrix.txt \
  > tmp.bed

mv tmp.bed SNP_DHS_matrix.txt

# make heatmap
matrix2png -r -c -g -size 16:16 -mincolor yellow -midcolor black -maxcolor red -data SNP_DHS_matrix.txt > SNP_DHS_matrix.png

exit 0
