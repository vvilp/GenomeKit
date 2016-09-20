# $1 swiss-prot-file $2: output sig file $3: sequence retrieval file


k=$1
siglen=512
gene_path=$2
sigoutput=$gene_path"RIsig-512-sig-$k"
sigoutput+="mer"
suffix="_gene_sig_trec_result"
suffix2="_output"
trec_result=$sigoutput$suffix
trec_result_output=$trec_result$suffix2
echo kmerLen:$k
echo sigoutput:$sigoutput
echo trec_result:$trec_result
echo trec_result_output:$trec_result_output

./RIGenes $k $siglen $gene_path $sigoutput
./GeneSigDistance $sigoutput $gene_path 8
./trec_eval -a data/swissprot-trec-rel $trec_result > $trec_result_output
