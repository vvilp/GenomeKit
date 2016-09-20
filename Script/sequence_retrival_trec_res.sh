

sigfile=$1
gene_path=$2
rel=$3
suffix="_gene_sig_trec_result"
suffix2="_output"
trec_result=$sigfile$suffix
trec_result_output=$trec_result$suffix2

echo $sigfile
echo $gene_path
echo $rel
echo $trec_result

./GeneSigDistance $sigfile $gene_path 20
./trec_eval -a $rel $trec_result > $trec_result_output
