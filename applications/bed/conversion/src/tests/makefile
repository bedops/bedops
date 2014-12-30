formats := bam gff gtf psl sam vcf wig

.PHONY: $(formats)

all: $(formats)
	@echo "[convert2bed] tests complete!"

$(formats):
	@$(MAKE) -C $@