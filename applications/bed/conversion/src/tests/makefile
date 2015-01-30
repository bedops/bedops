formats := bam gff gtf gvf psl rmsk sam vcf wig

.PHONY: $(formats)

all: $(formats)
	@echo "[convert2bed] tests complete!"

$(formats):
	@$(MAKE) -C $@
