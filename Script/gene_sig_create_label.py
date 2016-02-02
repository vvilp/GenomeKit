'''
calculate the sig for each gene in each file
'''

import os
import re
import numpy as np


k=8
sig_name = "8mer_semantic_sig_512_all_dna_new1"
sig_label_file = "../TestData2/sig/8mer_label"
gene_sig_name = "gene_sig_" + sig_name
sig_file = "../TestData2/sig/" + sig_name
gene_file = "../TestData2/alfsim-data2/DB/all_dna.fa"
gene_sig_output = "../TestData2/alfsim-data2/experiment_result/" + gene_sig_name
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
    # file.write(sig)

def get_kmer_sig_dict(kmer_lable_path, kmer_sig_path):
    kmer_sig_dict = dict()
    f_kmer = open(kmer_lable_path,"r")
    kmer_array = [line.rstrip('\n') for line in f_kmer]
    f_kmer_sig = open(kmer_sig_path,"r")
    kmer_sig_array = array = [[float(x) for x in line.split()] for line in f_kmer_sig]
    for i in range(0, len(kmer_array)):
        kmer_sig_dict[kmer_array[i]] = np.array(kmer_sig_array[i])
    return kmer_sig_dict

kmer_sig_dict = get_kmer_sig_dict(sig_label_file, sig_file)
def gene_sig(s,k):
    sig = np.zeros(kmer_sig_dict[kmer_sig_dict.keys()[0]].shape[0])
    for i in range(0, len(s) - k + 1):
        sig = sig + kmer_sig_dict[s[i:i+k]]
    return sig


# dir_path = "trec_eval_data/gene_fasta"
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

# for file in os.listdir(dir_path):
#     if file.endswith(".fa"):
#         file_path = dir_path + "/" + file;
#         f = open(file_path, "r")
#         gene_label = ""
#         for each_line in f:
#             each_line = each_line.strip()
#             # print each_line
#             sig = dict()
#             if each_line[0] == '>':
#                 s_array = [x for x in each_line.split(',')]
#                 result = re.findall(r'\d+', s_array[0]) #find integer
#                 gene_label = "S" + result[1] +  "/" + result[0].zfill(5)
#                 print gene_label
#             else:
#                 sig = gene_sig(each_line)
#                 save_sig(gene_label, sig, f_w)

f_w.close();
