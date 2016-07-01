'''
S001/00001 Q0 S001/00001 0 1.000000 STANDARD
S001/00001 Q0 S001/00002 0 0.000058 STANDARD
S001/00001 Q0 S001/00003 0 0.000071 STANDARD
S001/00001 Q0 S001/00004 0 0.000057 STANDARD
S001/00001 Q0 S001/00005 0 0.000029 STANDARD
S001/00001 Q0 S001/00006 0 0.000052 STANDARD
'''

import re

blast_output_file = "../TestData3_test89/blast_experiment/output"
trec_output_file = "../TestData3_test89/blast_experiment/trec_result_blast"



f = open(blast_output_file, "r")
f_trec_w = open(trec_output_file, "w")

score_dict = dict()
count = 0
for line in f:
    line.strip()
    line_break_arr = [x.strip() for x in line.split()]
    label1 = line_break_arr[0]
    label2 = line_break_arr[1]
    score = float(line_break_arr[len(line_break_arr)-1])
    score_dict[label1 + " " + label2] = score
    score_dict[label2 + " " + label1] = score
    # print label1, label2, score
    count+=1
    if count % 500 == 0:
        print count

print "parse score finsh!"

def output(keyi, keyj, score, f):
    output_str = "%s Q0 %s 0 %.6f STANDARD\n" % (keyi, keyj, score)
    f.write(output_str)

count = 0
for key, value in score_dict.iteritems():
    keys = key.split();
    output(keys[0], keys[1], value, f_trec_w);
    count +=1
    if count % 500 == 0:
        print count

f_trec_w.close()
