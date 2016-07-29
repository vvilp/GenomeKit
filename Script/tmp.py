import os

# input_file_path_label = "../tmp/6mer_label"
# input_file_path_sig = "../tmp/6mer_RandIndexing_Sig-autoencoder-256"
# output_file = "../tmp/output"
#
# input1 = open(input_file_path_label,"r")
# input2 = open(input_file_path_sig,"r")
# output = open(output_file, "w");
#
# labels = []
# for line in input1:
#     labels.append(line)
#
# sigs = []
# for line in input2:
#     sigs.append(line)
#
# for i in range (0, len(labels)):
#     output.write(labels[i])
#     output.write(sigs[i])

k=4
input_file = open("dna-w2v-4mer-512-sig","r")
output_file = open("dna-w2v-4mer-512-sig-2","w")

for i, line in enumerate(input_file):
    # print line
    if i ==0:
        continue
    output_file.write(line[0:k])
    output_file.write("\n")
    output_file.write(line[k+1:])
