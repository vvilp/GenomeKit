import numpy as np
from scipy.spatial import distance

gene_sig_file = "data_trec_eval/gene_sig_output.csv"
gene_distance_output = "data_trec_eval/distance.csv"
gene_trect_result_output = "data_trec_eval/trec_result"
f_gene_sig = open(gene_sig_file, "r")

f_dis_w = open(gene_distance_output, "w")
f_trec_w = open(gene_trect_result_output, "w")

sig_dict = dict()
label = ""
sig = []
for line in f_gene_sig:
    line = line.strip()
    if line[0] == 'S':
        label = line
    else:
        sig = [float(x) for x in line.split()]
        sig_dict[label] = np.array(sig)
f_gene_sig.close()

'''
output:
301	Q0	FR940202-2-00150	104	  2.129133	STANDARD
301	Q0	FR940202-2-00151	414	  1.724760	STANDARD
301	Q0	FR940202-2-00154	124	  2.104024	STANDARD
301	Q0	FR940203-1-00036	233	  1.912871	STANDARD
301	Q0	FR940203-1-00038	326	  1.800881	STANDARD
301	Q0	FR940203-1-00039	273	  1.870957	STANDARD
'''

def output(keyi, keyj, score, f):
    output_str = "%s Q0 %s 0 %d STANDARD\n" % (keyi, keyj, score)
    f.write(output_str)


key_set = sorted(sig_dict.keys())

for i in range(0, len(key_set)):
    print key_set[i]
    for j in range(i, len(key_set)):
        dis = distance.euclidean(sig_dict[key_set[i]], sig_dict[key_set[j]])
        score = (1.0 / (1 + (dis))) * 1000000
        if score > 500:
            output(key_set[i], key_set[j], score, f_trec_w)
            if i!=j:
                output(key_set[j], key_set[i], score, f_trec_w)


f_trec_w.close()
