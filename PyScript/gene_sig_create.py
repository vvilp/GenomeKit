'''
calculate the sig for each gene in each file
'''

import os
import re
import numpy as np


sig_label_file = "sig/6mer_label"
sig_file = "sig/6mer_RandIndexing_Sig-autoencoder-1024"
gene_file = "data_trec_eval/all_dna.fa"
gene_sig_output = "data_trec_eval/gene_sig_output.csv"
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
def gene_sig(s):
    sig = np.zeros(kmer_sig_dict[kmer_sig_dict.keys()[0]].shape[0])
    for i in range(0, len(s) - 5):
        sig = sig + kmer_sig_dict[s[i:i+6]]
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
        sig = gene_sig(each_line)
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
