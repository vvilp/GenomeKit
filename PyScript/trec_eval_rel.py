# input MSA_ALL_DNA.phy
'''
output format
301 0 CR93E-10279 0
301 0 CR93E-10505 0
301 0 CR93E-1282 1
301 0 CR93E-1850 0
301 0 CR93E-1860 0
'''

def write_output(labeli, labelj, fw):
    output_str = "%s 0 %s 1\n" % (labeli, labelj)
    fw.write(output_str)

readfile = "data_trec_eval4/MSA_all_dna.phy"
writefile = "data_trec_eval4/tec_eval_rel"
f = open(readfile,"r")
fw = open(writefile,"w")

relate_dict = dict()
sum_row = 0
while True:
    line = f.readline()
    if line == '':
        break;
    if line == '\n':
        continue;
    row, size = [int(x) for x in line.split()]
    print row
    sum_row += row
    label_set = set()
    for i in range(0,row):
        seq_line = f.readline()
        label, seq = [x for x in seq_line.split()]
        label_set.add(label)
        # print label
    for label in label_set:
        if label in relate_dict.keys():
            relate_dict[label] = relate_dict[label] | label_set
        else:
            relate_dict[label] = label_set

keys_set = sorted(relate_dict.keys())
count = 0
for each_k_i in keys_set:
    for each_k_j in keys_set:

        if each_k_j in relate_dict[each_k_i]:
            write_output(each_k_i, each_k_j, fw)

    count += 1
    if count % 100 == 0:
        print count

# FULL RESULT!
# relate_dict = dict()
# sum_row = 0
# while True:
#     line = f.readline()
#     if line == '':
#         break;
#     if line == '\n':
#         continue;
#     row, size = [int(x) for x in line.split()]
#     print row
#     sum_row += row
#     label_set = set()
#     for i in range(0,row):
#         seq_line = f.readline()
#         label, seq = [x for x in seq_line.split()]
#         label_set.add(label)
#         # print label
#     for label in label_set:
#         if label in relate_dict.keys():
#             relate_dict[label] = relate_dict[label] | label_set
#         else:
#             relate_dict[label] = label_set
#
# print "relate dict finish!"
#
# keys_set = sorted(relate_dict.keys())
# count = 0
# for each_k_i in keys_set:
#     for each_k_j in keys_set:
#         output = each_k_i + " 0 "
#         output += each_k_j + " "
#
#         if each_k_j in relate_dict[each_k_i]:
#             output += " 1"
#         else:
#             output += " 0"
#         # print output
#         fw.write(output)
#         fw.write("\n")
#     count += 1
#     if count % 100 == 0:
#         print count
fw.close();

print sum_row
