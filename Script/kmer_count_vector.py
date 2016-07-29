import numpy as np

k=2
kmer_alph = "ABCDEFGHIKLMNPQRSTVWXYZ"
# gene_file = "../TestData2/alfsim-data2/DB/all_dna.fa"
# gene_file = "../TestData2/alfsim-data2/DB/all_dna.fa"
gene_file = "swissprot-sequences.faa"
gene_sig_output = "./sequence-bow-for-training.csv"
f_w = open(gene_sig_output, "w")
f_gene = open(gene_file, "r")

def save_sig(label, sig, family, file):
    file.write("%d" % family)
    file.write(" ")
    for i in range (0, len(sig)):
        file.write("%d" % sig[i])
        if i < len(sig) - 1 :
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
gene_bow_list = list()
count = 0
for each_line in f_gene:
    if each_line[0] == '>':
        continue
    else:
        each_line = each_line.strip()
        l = list()
        for each_kmer in kmers:
            l.append(each_line.count(each_kmer))
        # print len(l)
        gene_bow_list.append(l)
    if count % 100 == 0:
        print count
    count+=1

# gene_bow_array = np.array(gene_bow_list)
print len(gene_bow_list)
family_count = 0
for i, list in enumerate(gene_bow_list):
    if i % 50 == 0:
        family_count+=1
    save_sig("", list, family_count, f_w);
