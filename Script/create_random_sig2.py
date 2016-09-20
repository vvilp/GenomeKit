import sys
import random

# k = 3
# sig_len = 256
# kmer_label_file_path = "./" + str(k) + "mer_label"
# sig_file_path =  "./" + str(k) + "mer_rand_sig_" + str(sig_len)
# kmer_alph = "ACGT"
k = 4
sig_len = 256
sig_file_path =  "./" + str(k) + "mer_rand_sig_" + str(sig_len)
gene_file_path = "./all_prot.fa"

sig_file = open(sig_file_path, "w");
# gene_file =  open(gene_file_path, "w");

kmers = set()

def rand_list(size):
    rand_list = [0] * size
    for i in range(0, size):
        rand_list[i] = random.uniform(-1.0, 1.0);
    return rand_list

def GeneToKmers(gene,k):
    for i in range(0, len(gene) - k + 1):
        kmers.add(gene[i:i+k])

def GeneToKmerSet(path):
    f = open(path, "r")
    for each_line in f:
        each_line = each_line.strip()
        if each_line[0] == '>':
            print each_line
        else:
            GeneToKmers(each_line,k)

GeneToKmerSet(gene_file_path)

count = 0
for kmer in kmers:
    count+=1
    percent = (int)((float)(count) / (float)(len(kmers)) * 100 )
    if percent % 10 == 0:
        print percent
    # kmer_label_file.write(kmer)
    # kmer_label_file.write("\n")
    randlist = rand_list(sig_len)
    sig_file.write(kmer)
    sig_file.write("\n")
    for r in randlist:
        sig_file.write("%.3f" % r)
        sig_file.write(" ")
    sig_file.write("\n")

sig_file.close();
# kmer_label_file.close();
