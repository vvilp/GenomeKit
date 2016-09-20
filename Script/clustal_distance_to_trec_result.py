import re
import Queue

path = "clustal_distance"
f = open(path, "r")
f_trec_w = open(path+"_trect_result", "w")
row = int(f.readline().strip())

label_arr = []
distance_str_array = []

for i in range(0, row):
    line = f.readline().strip()
    label, distance_str = [x.strip() for x in line.split(',')]
    label_indexs = re.findall(r'\d+', label)
    label = "S" + label_indexs[1] +  "/" + label_indexs[0].zfill(5)
    # label = line[0:6]
    # distance_str = line[7:]
    label_arr.append(label)
    distance_str_array.append(distance_str.strip())

print "Finish Label processing"
# print len(label_arr)
# print len(distance_str_array[0].split())
'''
output:
301	Q0	FR940202-2-00150	104	  2.129133	STANDARD
301	Q0	FR940202-2-00151	414	  1.724760	STANDARD
301	Q0	FR940202-2-00154	124	  2.104024	STANDARD
301	Q0	FR940203-1-00036	233	  1.912871	STANDARD
301	Q0	FR940203-1-00038	326	  1.800881	STANDARD
301	Q0	FR940203-1-00039	273	  1.870957	STANDARD
'''

for i in range(0, len(distance_str_array)):
    distance_arr = [float(x) for x in distance_str_array[i].split()]

    q = Queue.PriorityQueue()
    for j in range (0, len(distance_arr)):
        score = 1.0 / (1 + (distance_arr[j]))
        output_str = "%s Q0 %s 0 %.6f STANDARD\n" % (label_arr[i], label_arr[j], score)
        q.put((-score,output_str))

    #get first 2000 entry
    for k in range(0,400):
        f_trec_w.write(q.get()[1])
    print i

# for i in range(0, len(distance_str_array)):
#     # print distance_str_array[i]
#     # break;
#     distance_arr = [float(x) for x in distance_str_array[i].split()]
#     for j in range (0, len(distance_arr)):
#         f_trec_w.write(label_arr[i])
#         f_trec_w.write(" Q0 ")
#         f_trec_w.write(label_arr[j])
#         f_trec_w.write(" 0 ")
#         score = 1.0 / (1 + (distance_arr[j]))
#         f_trec_w.write("%.6f" % score)
#         f_trec_w.write(" STANDARD")
#         f_trec_w.write("\n")
#     print i
f_trec_w.close()
