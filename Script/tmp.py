import os

input_file_path_label = "../tmp/6mer_label"
input_file_path_sig = "../tmp/6mer_RandIndexing_Sig-autoencoder-256"
output_file = "../tmp/output"

input1 = open(input_file_path_label,"r")
input2 = open(input_file_path_sig,"r")
output = open(output_file, "w");

labels = []
for line in input1:
    labels.append(line)

sigs = []
for line in input2:
    sigs.append(line)

for i in range (0, len(labels)):
    output.write(labels[i])
    output.write(sigs[i])


# input_file = open(input_file_path,"r")
# output_file = open(output_file_path,"w")
#
# for line in input_file:
#     # print line
#     output_file.write(line[0:5])
#     output_file.write("\n")
#     output_file.write(line[6:])
