import os

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

def generate_kmer_dict():
    kmer_dict = dict()
    kmer_list = generate_kmers(6, "ACGT")
    for i in range(0,len(kmer_list)):
        kmer_dict[kmer_list[i]] = i
    return kmer_dict



kmer_count_dir = "temp"
kmer_pair_score_output = "temp/output.csv"
kmer_index_dict = generate_kmer_dict();

f_w = open("temp/output.csv", "w")

for file_name in os.listdir("temp"):
    filename, file_extension = os.path.splitext(file_name)
    if file_extension != ".fa" :
        continue

    file_path = kmer_count_dir + "/" + file_name
    print file_path
    f = open(file_path)
    count = 0
    kmer_count_dict = dict()
    for line in f:
        if line[0]=='>':
            count = int(line[1:])
            continue
        else:
            kmer_count_dict[line.strip()] = count
        # break
    # print kmer_count_dict

    for kmer1 in kmer_count_dict.keys():
        for kmer2 in kmer_count_dict.keys():
            if kmer1 != kmer2:
                # print kmer1, " ", kmer1, " ", kmer_count_dict[kmer1] + kmer_count_dict[kmer2]
                output_str = "%s %s %d\n" % (kmer_index_dict[kmer1], kmer_index_dict[kmer2], kmer_count_dict[kmer1] + kmer_count_dict[kmer2])
                f_w.write(output_str)

f_w.close()
