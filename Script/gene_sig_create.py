'''
calculate the sig for each gene in each file
'''

import os
import re
import numpy as np

k=5
sig_name = "5mer_semantic_sig_512_all_dna_new2_autoencoder"
gene_sig_name = "gene_sig_" + sig_name
sig_file = "../TestData2/sig/" + sig_name
gene_file = "../TestData2/alfsim-data2/DB/all_dna.fa"
gene_sig_output = "../TestData2/alfsim-data2/experiment_result/" + gene_sig_name

f_sig = open(sig_file, "r")
f_gene = open(gene_file, "r")
f_w = open(gene_sig_output, "w")

def save_sig(label, sig, file):
    file.write(label)
    file.write("\n")
    for i in range (0, sig.shape[0]):
        file.write("%.2f" % sig[i])
        if i < sig.shape[0] - 1 :
            file.write(" ")
    file.write("\n")

sig_dict = dict()
label = ""
sig = []
iterC = 0
for line in f_sig:
    line = line.strip()
    if iterC % 2 == 0:
        label = line
        if k == 0:
            k = len(line)
            print "k:%d" % k
    else:
        sig = [float(x) for x in line.split()]
        sig_dict[label] = np.array(sig)
    iterC+=1

def gene_sig(s,k):
    sig = np.zeros(sig_dict[sig_dict.keys()[0]].shape[0])
    for i in range(0, len(s) - k + 1):
        if s[i:i+k] in sig_dict:
            sig = sig + sig_dict[s[i:i+k]]
    return sig

gene_label = ""
for each_line in f_gene:
    each_line = each_line.strip()
    # print each_line
    sig = dict()
    if each_line[0] == '>':
        s_array = [x for x in each_line.split(',')]
        result = re.findall(r'\d+', s_array[0]) #find integer
        gene_label = "S" + result[1] +  "/" + result[0].zfill(5)
        print gene_label
    else:
        sig = gene_sig(each_line, k)
        save_sig(gene_label, sig, f_w)
