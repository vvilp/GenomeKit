import os

# f = open("kmer_count_vector","r")
# fw = open("label","w")
# lines = [line.rstrip('\n') for line in f]
#
# for line in lines:
#     if len(line) == 10:
#         fw.write(line)
#         fw.write("\n")
#
# fw.close()



f = open("data/6mer_RandIndexing_Sig","r")
fw = open("data/6mer_RandIndexing_Sig_triple","w")

# f = open("data/6mer_count_vector","r")
# fw = open("data/6mer_count_vector_triple","w")

lines = [line.rstrip('\n') for line in f]

for line in lines:
    arr = [float(a) for a in line.split()]
    for i in range(0, len(arr)):
        if arr[i] > 0:
            fw.write("1")
        elif arr[i] == 0:
            fw.write("0")
        else:
            fw.write("-1")

        if i < len(arr)-1:
            fw.write(" ")
    fw.write("\n")

fw.close()
