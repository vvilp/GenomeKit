import os
import numpy as np
from scipy.spatial import distance

def save_data_binary(data):
    f1 = open('output_binary.csv', 'w')
    for i in range (0, data.shape[0]):
        for j in range (0, data.shape[1]):
            if data[i][j] >= 0.0 :
                f1.write("1")
            else :
                f1.write("0")
            if j < data.shape[1] - 1 :
                f1.write(" ")
        f1.write("\n")
    f1.close()

def save_data(data):
    f2 = open('genome_sig_output.csv', 'w')
    for i in range (0, data.shape[0]):
        for j in range (0, data.shape[1]):
            f2.write("%.2f" % data[i][j])
            if j < data.shape[1] - 1 :
                f2.write(" ")
        f2.write("\n")
    f2.close()

def get_kmer_sig_dict(kmer_lable_path, kmer_sig_path):
    kmer_sig_dict = dict()
    f_kmer = open(kmer_lable_path,"r")
    kmer_array = [line.rstrip('\n') for line in f_kmer]
    # print len(kmer_array)
    f_kmer_sig = open(kmer_sig_path,"r")
    # f_kmer_sig = open("data/6mer_count_vector_triple","r")
    kmer_sig_array = array = [[float(x) for x in line.split()] for line in f_kmer_sig]
    for i in range(0, len(kmer_array)):
        kmer_sig_dict[kmer_array[i]] = np.array(kmer_sig_array[i])
    return kmer_sig_dict

def output_distance(pair_distance, file_list, file_name):
    ouput_dist = open(file_name, "w");
    ouput_dist.write(" ,")
    for each in file_list:
        ouput_dist.write("%s," % each)
    ouput_dist.write("\n")

    for i in range(0, len(pair_distance)):
        ouput_dist.write("%s," % file_list[i])
        for each in pair_distance[i]:
            ouput_dist.write("%.3f," % each)
        ouput_dist.write("\n")

def hamming_distance(arr1, arr2):
    distance = 0
    for i in range(0,len(arr1)):
        if (arr1[i] > 0 and arr2[i]> 0) or (arr1[i] == 0 and arr2[i] == 0) or (arr1[i] < 0 and arr2[i] < 0):
            distance=distance
        else:
            distance+=1
    return distance

def pair_distance(genome_sig_list, file_list):
    H_dist = [[0 for x in range(genome_sig_list.shape[0])] for x in range(genome_sig_list.shape[0])]
    E_dist = [[0 for x in range(genome_sig_list.shape[0])] for x in range(genome_sig_list.shape[0])]
    C_dist = [[0 for x in range(genome_sig_list.shape[0])] for x in range(genome_sig_list.shape[0])]

    for i in range(0, genome_sig_list.shape[0]):
        for j in range(0, genome_sig_list.shape[0]):
            H_dist[i][j] = distance.hamming(genome_sig_list[i],genome_sig_list[j])
            E_dist[i][j] = distance.euclidean(genome_sig_list[i],genome_sig_list[j])
            C_dist[i][j] = distance.cosine(genome_sig_list[i],genome_sig_list[j])

    output_distance(H_dist, file_list, "hamming_distance.csv")
    output_distance(E_dist, file_list, "euclidean_distance.csv")
    output_distance(C_dist, file_list, "cosine_distance.csv")

def main():
    kmer_sig_dict = get_kmer_sig_dict("data/6mer_label", "data/6mer_sig.csv");

    path = "data/6mer_count"
    file_list = os.listdir(path)
    genome_sig_list = [];
    for each in file_list:
        f_path = path + "/" + each
        genome_sig = np.zeros(kmer_sig_dict[kmer_sig_dict.keys()[0]].shape[0])

        lines = [line.rstrip('\n') for line in open(f_path)]
        # print lines
        kmer_count = 0
        for line in lines:
            if line[0] == '>':
                kmer_count = int(line[1:])
            else:
                genome_sig = genome_sig + (kmer_sig_dict[line] * kmer_count)
        genome_sig_list.append(genome_sig)
    genome_sig_list = np.array(genome_sig_list)
    print genome_sig_list
    save_data(genome_sig_list);
    pair_distance(genome_sig_list, file_list)


main()

# array1 = np.array([-2,1,1,1,1,10.0,1,1,1,1])
# array2 = np.array([-1,1,1,1,1,10.1,1,1,1,1])
# # print array1 + array2 * 2
# print distance.cosine(array1, array2);
# print distance.hamming(array1, array2);
