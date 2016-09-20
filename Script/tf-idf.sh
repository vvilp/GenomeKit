# $1 swiss-prot-file $2: output sig file $3: sequence retrieval file


k=$1
sigoutput="tfidf-512-sig-$k"
sigoutput+="mer"
suffix="_gene_sig_trec_result"
suffix2="_output"
trec_result=$sigoutput$suffix
trec_result_output=$trec_result$suffix2
echo kmerLen:$k
echo sigoutput:$sigoutput
echo trec_result:$trec_result
echo trec_result_output:$trec_result_output


#python tfidf-sig3.py $k data/genes_small.faa data/swissprot-sequences.faa data/$sigoutput
./GeneSigDistance data/$sigoutput data/swissprot-sequences.faa 8
./trec_eval -a data/swissprot-trec-rel data/$trec_result > data/$trec_result_output
