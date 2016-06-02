import numpy as np
# import lda
k=5
kmer_alph = "ACGT"
gene_file = "../TestData2/alfsim-data2/DB/all_dna.fa"
# gene_file = "genes500.fa"
output = gene_file+"_bow_sig"
f_gene = open(gene_file, "r")
f_output = open(output, "w")

# a = np.array([1.0,2.0])
# print a/5

def save_sig(label, sig, file):
    file.write(label)
    file.write("\n")
    for i in range (0, sig.shape[0]):
        file.write("%.6f" % sig[i])
        if i < sig.shape[0] - 1 :
            file.write(" ")
    file.write("\n")

def search_kmer(k, letters, kmer, kmers):
    if k == 0:
        kmers.append(kmer)
        return
    for i in range(0, len(letters)):
        kmer += letters[i]
        search_kmer(k-1, letters, kmer, kmers)
        kmer = kmer[0:len(kmer)-1]

def generate_kmers(k, letters):
    kmers = list()
    kmer = ""
    search_kmer(k, letters, kmer, kmers)
    return kmers

kmers = generate_kmers(k,kmer_alph)
# print kmers
# gene_bow = np.array()
gene_sig_dict = dict()
gene_list = list()
count = 0
for each_line in f_gene:
    if each_line[0] == '>':
        continue
    else:
        gene_list.append(each_line)
        each_line = each_line.strip()
        l = list()
        for each_kmer in kmers:
            l.append((float)(each_line.count(each_kmer)) / (len(each_line) - k + 1))
        gene_sig_dict[each_line] = np.array(l)
    if count % 100 == 0:
        print count
    count+=1

# kmer representation
for each_kmer in kmers:
    kmer_sig = np.zeros(gene_sig_dict[gene_sig_dict.keys()[0]].shape[0])
    count = 0
    for each_gene in gene_sig_dict:
        if each_kmer in each_gene:
            kmer_sig += gene_sig_dict[each_gene]
            count+=1
        # print kmer_sig
    kmer_sig = kmer_sig / (float)(count)
    # print kmer_sig
    # print count
    # break
    save_sig(each_kmer,kmer_sig,f_output)
