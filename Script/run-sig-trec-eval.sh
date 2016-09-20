
suffix="_gene_sig_trec_result"
suffix2="_output"

# listVar="3mer_rand_sig_256 3mer_rand_sig_512 3mer-RI-256 3mer-RI-512 4mer-RI-512 5mer-RI-512 6mer-RI-512 3mer-SNN-512_reformat 4mer-SNN-512_reformat 5mer-SNN-512_reformat 6mer-SNN-512_reformat 3mer-SNN-1024_reformat 3mer-SNN-2048_reformat 3mer-SNN-512-binary 3mer-SNN-1024-binary 3mer-SNN-2048-binary"
listVar="4mer_rand_sig_512 5mer_rand_sig_512 6mer_rand_sig_512"
for i in $listVar; do
    echo "$i"
    ./GeneSigDistance $i all_prot.fa 20
    trec_result=$i$suffix
    trec_result_output=$trec_result$suffix2
    ./trec_eval -a trec_eval_rel $trec_result > $trec_result_output
done
