figures:
	./reference_bedmap_make_overlap_figures.Rscript reference_bedmap_map.bed reference_bedmap_reference.bed reference_bedmap_mapref.pdf

padded_figures:
	bedops --range 100 --everything reference_bedmap_reference.bed > reference_bedmap_reference_padded.bed
	./reference_bedmap_make_overlap_figures.Rscript reference_bedmap_map.bed reference_bedmap_reference_padded.bed reference_bedmap_mapref_padded.pdf
