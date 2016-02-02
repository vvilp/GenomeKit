import sys
import random
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
    kmers = generate_kmers(k, "ACGT")
    sig_dict = dict()
    for kmer in kmers:
        sig_dict[kmer] = [0] * sig_len
    return sig_dict

def kmer_count_add(k, seq_str, kmer_count_dict, index):
    for i in range(0, len(seq_str) - k + 1):
        kmer = seq_str[i:i+k]
        kmer_count_list = kmer_count_dict[kmer]
        kmer_count_list[index] += 1

def rand_index_list(size, density):
    rand_list = [0] * size
    randIndexNum = (int)(size * density)
    ternary =  [-1, 0 ,1]
    for i in range(0, randIndexNum):
        rand_list[random.randrange(0,size)] = ternary[random.randrange(0,len(ternary))]
    return rand_list

def kmer_RI_add(k, seq_str, kmer_sig_dict, size):
    ri_list = rand_index_list(size, 0.002)
    for i in range(0, len(seq_str) - k + 1):
        kmer = seq_str[i:i+k]
        kmer_sig_list = kmer_sig_dict[kmer]
        kmer_sig_dict[kmer] = [x + y for x, y in zip(ri_list, kmer_sig_list)]

k = 6
input_file = "data_trec_eval3/all_dna.fa"
input_f = open(input_file, "r")
lines = [line.strip() for line in input_f]
seq_count = len(lines) / 2

output_file = "data_trec_eval3/data/6mer_4096_RISIG"
output_f = open(output_file, "w")

output_3mer_label = "data_trec_eval3/data/6mer_label"
output_3mer_label_f = open(output_3mer_label, "w")

print seq_count
kmer_sig_dict = create_kmer_sig_dict(k,seq_count);
sorted_key = sorted(kmer_sig_dict.keys())

sig_len = 4096
index = 0
for line in lines:
    if line[0] == '>':
        continue
    # kmer_count_add(k, line, kmer_sig_dict, index)
    kmer_RI_add(k, line, kmer_sig_dict, sig_len)
    index += 1
    print index
    if index == sig_len:
        break

for key in sorted_key:
    # output_f.write(kmer_sig_dict[key])
    # print key
    # print kmer_sig_dict[key]
    kmer_count_list = kmer_sig_dict[key]
    for i in range(0,len(kmer_count_list)):
        output_f.write(str(kmer_count_list[i]))
        if i < len(kmer_count_list) - 1:
            output_f.write(" ")
    output_f.write("\n")

    output_3mer_label_f.write(key)
    output_3mer_label_f.write("\n")

output_f.close()
output_3mer_label_f.close()
