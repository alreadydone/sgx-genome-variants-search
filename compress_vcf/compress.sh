#!/bin/bash

case_indir="../iDASH/case/"
control_indir="../iDASH/control/"
outdir="../iDASH/bin/"
unique_snp_output="../iDASH/unique_snps.bin"

#case_indir="../test_data/case/"
#control_indir="../test_data/control/"
#outdir="../test_data/bin/"
#unique_snp_output="../test_data/unique_snps.bin"

./extract_uniq_snps "$outdir" "2000" "$unique_snp_output"
exit 0

for fname in "$case_indir"*.vcf; do
	./compress_vcf "$fname" "1"
done

mv "$case_indir"*.bin "$outdir"

for fname in "$control_indir"*.vcf; do
	./compress_vcf "$fname" "0"
done

mv "$control_indir"*.bin "$outdir"

./extract_uniq_snps "$outdir" "1000" "$unique_snp_output"
