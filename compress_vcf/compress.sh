#!/bin/bash

case_indir="../UKB/case/"
control_indir="../UKB/control/"
outdir="../UKB/bin/"
unique_snp_output="../UKB/unique_snps.bin"

for fname in "$case_indir"*.vcf; do
	./compress_vcf "$fname" "1"
done

mv "$case_indir"*.bin "$outdir"

for fname in "$control_indir"*.vcf; do
	./compress_vcf "$fname" "0"
done

mv "$control_indir"*.bin "$outdir"

./extract_uniq_snps "$outdir" "7750" "$unique_snp_output"
