'''
S001/00001 Q0 S001/00001 0 1.000000 STANDARD
S001/00001 Q0 S001/00002 0 0.000058 STANDARD
S001/00001 Q0 S001/00003 0 0.000071 STANDARD
S001/00001 Q0 S001/00004 0 0.000057 STANDARD
S001/00001 Q0 S001/00005 0 0.000029 STANDARD
S001/00001 Q0 S001/00006 0 0.000052 STANDARD
'''

import re

label_input_file = "../TestData2/alfsim-data2/MSA/MSA_all_dna.phy"
blast_output_file = "../TestData2/alfsim-data2/blast-experiment/output"
trec_output_file = "../TestData2/alfsim-data2/experiment_result/trec_result_blast"

f_dna = open(label_input_file, "r")
label_array = []
for line in f_dna:
    if line[0] == 'S':
        label, seq = line.split()
        label_array.append(label)
f_dna.close()
# print label_array

f = open(blast_output_file, "r")
f_trec_w = open(trec_output_file, "w")

score_dict = dict()
count = 0
for line in f:
    # print line
    line_break_arr = [x.strip() for x in line.split()]
    label1 = line_break_arr[0].strip(',')
    label2 = line_break_arr[1].strip(',')
    label_indexs = re.findall(r'\d+', label1)
    label1 = "S" + label_indexs[1] +  "/" + label_indexs[0].zfill(5)
    label_indexs = re.findall(r'\d+', label2)
    label2 = "S" + label_indexs[1] +  "/" + label_indexs[0].zfill(5)
    # print label1, label2
    score = float(line_break_arr[len(line_break_arr)-1])


    # if (label1,label2) in score_dict.keys() or (label2,label1) in score_dict.keys():
    #     if score > score_dict[(label1,label2)] :
    #         score_dict[(label1,label2)] = score
    #         score_dict[(label2,label1)] = score
    # else:
    score_dict[label1 + " " + label2] = score
    score_dict[label2 + " " + label1] = score

print "parse score finsh!"


for labeli in label_array:
    for labelj in label_array:
        score = 0.0
        score = score_dict.get(labeli + " " + labelj, 0.0)
        if score > 0.0:
            f_trec_w.write(labeli)
            f_trec_w.write(" Q0 ")
            f_trec_w.write(labelj)
            f_trec_w.write(" 0 ")
            f_trec_w.write("%.6f" % score)
            f_trec_w.write(" STANDARD")
            f_trec_w.write("\n")
    count+=1
    if count % 100 == 0:
        print count
f_trec_w.close()
