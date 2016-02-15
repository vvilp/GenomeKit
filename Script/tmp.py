import os

input_file_path = "/Users/vvilp/5mer_512_all_dna_new_all.fa.rep"
output_file_path = "/Users/vvilp/5mer_512_all_dna_new_all.fa.rep2"

input_file = open(input_file_path,"r")
output_file = open(output_file_path,"w")

for line in input_file:
    # print line
    output_file.write(line[0:5])
    output_file.write("\n")
    output_file.write(line[6:])
