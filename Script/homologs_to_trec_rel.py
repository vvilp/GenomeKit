import os

input_file_path = "../TestData3_test89/homologs.csv"
input_file = open(input_file_path, "r")

output_file_path = "../TestData3_test89/homologs_trec_rel"
output_file = open(output_file_path, "w")

def write_output(labeli, labelj, fw):
    output_str = "g%05d 0 g%05d 1\n" % (labeli, labelj)
    fw.write(output_str)

count = 0
for line in input_file:
    line = line.strip()
    s_array = [int(x) for x in line.split(',')]
    if s_array[0] >= 4000:
        break
    for i in range(1, len(s_array)):
        if s_array[i] >= 4000:
            break
        write_output(s_array[0], s_array[i], output_file)
    count+=1
    print count

output_file.close()

# print "%05d" % 1

# smaller homologs
# output_file_path = "../TestData3_test89/homologs_4000.csv"
# output_file = open(output_file_path, "w")
# for line in input_file:
#     line = line.strip()
#     s_array = [int(x) for x in line.split(',')]
#     for i in range(0, len(s_array)):
#         if s_array[i] < 2000:
#             output_file.write("%d" % s_array[i])
#         else:
#             output_file.write("\n")
#             break
#         output_file.write(",")
#
# output_file.close()
