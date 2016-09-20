import math
import sys

# arg1 kmer size, arg2 gene file, arg3 swiss prot file, arg4 output

sig_size = 512
k = int(sys.argv[1])

gene_path = sys.argv[2]
f_gene = open(gene_path, "r")

family_num = 512
seq_in_family_num = 20
family_protein_path = sys.argv[3]
f_family_protein = open(family_protein_path, "r")

output_path = sys.argv[4]
output = open(output_path, "w")



def generate_kmers(k, gene_list):
    kmers = set()
    for gene in gene_list:
        for i in range(0, len(gene) - k + 1):
            kmers.add(gene[i:i+k])
    return kmers

def write_sig(kmer,sig,f):
    f.write(kmer)
    f.write("\n")
    for i,each in enumerate(sig):
        f.write("%0.6f" % each)
        if i!=len(sig)-1:
            f.write(" ")
    f.write("\n")


gene_list = []
for i, line in enumerate(f_gene):
    if i % 2 != 0:
        line = line.strip()
        gene_list.append(line)

print len(gene_list)


protein_gene_list = []
for i, line in enumerate(f_family_protein):
    if i % 2 != 0:
        line = line.strip()
        protein_gene_list.append(line)

print len(protein_gene_list)

kmers = generate_kmers(k, protein_gene_list)

print "kmer num: %d" % len(kmers)

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
                tf = 0
                total_number_kmer = 0
                num_gene_with_kmer = 0
            sig.append(tfidf)
    write_sig(kmer,sig,output)
    # print kmer
