import math
# sig_size = 1024
# k = 6
# kmer_alph = "ACGT"
# gene_path = "all_dna.fa"
# f_gene = open(gene_path, "r")
# output = open("tf-idf-6mer-1024", "w")

sig_size = 1024
k = 3
kmer_alph = "ABCDEFGHIKLMNPQRSTVWXYZ"
gene_path = "data/genes_small.faa"
f_gene = open(gene_path, "r")

family_num = 1024
seq_in_family_num = 20
family_protein_path = "data/512-family-swissprot-sequences.faa"
f_family_protein = open(family_protein_path, "r")

output = open("protein-tf-idf-3mer-512", "w")

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

def write_sig(kmer,sig,f):
    f.write(kmer)
    f.write("\n")
    for i,each in enumerate(sig):
        f.write("%0.6f" % each)
        if i!=len(sig)-1:
            f.write(" ")
    f.write("\n")

kmers = generate_kmers(k, kmer_alph)

gene_list = []
for i, line in enumerate(f_gene):
    if i % 2 != 0:
        gene_list.append(line)

print len(gene_list)


protein_gene_list = []
for i, line in enumerate(f_family_protein):
    if i % 2 != 0:
        protein_gene_list.append(line)

print len(protein_gene_list)


kmer_tf_dict = dict()
for kmer in kmers:
    sig = []
    tf = 0
    total_number_kmer = 0
    num_gene_with_kmer = 0
    for i in range(0, len(protein_gene_list)):
        tf += protein_gene_list[i].count(kmer)
        total_number_kmer = len(protein_gene_list[i]) - k + 1
        if kmer in protein_gene_list[i]:
            num_gene_with_kmer+=1
        if (i+1) % seq_in_family_num == 0:
            tfidf = 0
            if num_gene_with_kmer != 0:
                tf = (float)(tf) / (float)(total_number_kmer)
                idf = math.log((float)(seq_in_family_num) / (float)(num_gene_with_kmer))
                tfidf = tf * idf
                # print "tf:%f" % tf
                # print "idf:%f" % idf
                tf = 0
                total_number_kmer = 0
                num_gene_with_kmer = 0
            sig.append(tfidf)
    write_sig(kmer,sig,output)
    print kmer
