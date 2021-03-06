import numpy as np
from scipy.spatial import distance

# gene_sig_name = "gene_sig_" + sig_name
# gene_sig_file = "../TestData3_test89/sig/" + gene_sig_name
# gene_trect_result_output = "../TestData3_test89/Precision_recall/" + gene_sig_name + "_rec_result"

gene_sig_name = "swissprot-sequences.faa-d2v-3mer-512-sig"
gene_sig_file = "./" + gene_sig_name
gene_trect_result_output = "./" + gene_sig_name + "_trec_result"

f_gene_sig = open(gene_sig_file, "r")

# f_dis_w = open(gene_distance_output, "w")
f_trec_w = open(gene_trect_result_output, "w")

sig_dict = dict()
label = ""
sig = []
for i,line in enumerate(f_gene_sig):
    line = line.strip()
    if i % 2 == 0:
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
        if score > 50:
            output(key_set[i], key_set[j], score, f_trec_w)
            if i!=j:
                output(key_set[j], key_set[i], score, f_trec_w)


f_trec_w.close()
