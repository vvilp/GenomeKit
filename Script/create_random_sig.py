import sys
import random

# k = 3
# sig_len = 256
# kmer_label_file_path = "./" + str(k) + "mer_label"
# sig_file_path =  "./" + str(k) + "mer_rand_sig_" + str(sig_len)
# kmer_alph = "ACGT"
k = 4
sig_len = 256
kmer_label_file_path = "./" + str(k) + "mer_label"
sig_file_path =  "./" + str(k) + "mer_rand_sig_" + str(sig_len)
kmer_alph = "ABCDEFGHIKLMNPQRSTVWXYZ"

kmer_label_file = open(kmer_label_file_path, "w");
sig_file = open(sig_file_path, "w");

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

def create_kmer_sig_dict(k, sig_len):
    kmers = generate_kmers(k, kmer_alph)
    sig_dict = dict()
    for kmer in kmers:
        sig_dict[kmer] = [0] * sig_len
    return sig_dict

def rand_list(size):
    rand_list = [0] * size
    for i in range(0, size):
        rand_list[i] = random.uniform(-1.0, 1.0);
    return rand_list

kmers = generate_kmers(k, kmer_alph)

for kmer in kmers:
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
kmer_label_file.close();
