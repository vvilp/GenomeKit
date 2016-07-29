import math
# sig_size = 1024
# k = 6
# kmer_alph = "ACGT"
# gene_path = "all_dna.fa"
# f_gene = open(gene_path, "r")
# output = open("tf-idf-6mer-1024", "w")

sig_size = 256
k = 3
kmer_alph = "ABCDEFGHIKLMNPQRSTVWXYZ"
gene_path = "genes_small.faa"
f_gene = open(gene_path, "r")
output = open("protein-tf-idf-3mer-256", "w")

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

kmers = generate_kmers(k, kmer_alph)

gene_list = []
for i, line in enumerate(f_gene):
    if i % 2 != 0:
        gene_list.append(line)

print len(gene_list)

kmeridf_dict = dict()
for kmer in kmers:
    gene_num = 0
    for i in range(sig_size):
        if kmer in gene_list[i]:
            gene_num+=1
    idf = 0
    if gene_num != 0:
        idf = math.log((float)(sig_size) / (float)(gene_num))
    if idf == 0.0:
        kmeridf_dict[kmer] = 1.0
    else:
        kmeridf_dict[kmer] = idf
# print kmeridf_dict
kmer_sig_dict = dict()
for kmer in kmers:
    output.write(kmer)
    output.write("\n")
    for i in range(sig_size):
        gene = gene_list[i]
        total_num_kmer = len(gene) - k + 1
        num_kmer = gene.count(kmer)
        tf=0
        if num_kmer != 0:
            tf = (float)(total_num_kmer) / (float)(num_kmer)
        tfidf = tf * kmeridf_dict[kmer]
        output.write("%.6f" % (tfidf/100.0))
        if i != sig_size-1:
            output.write(" ")
    output.write("\n")
