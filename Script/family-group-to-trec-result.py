from collections import defaultdict
input = open("swissprot-family-group", "r")
output = open("swissprot-trect-rel", "w")
for line in input:
    line = line.strip()
    list = [x for x in line.split()]
    for each_x in list:
        for each_y in list:
            output.write("%s 0 %s 1 \n" % (each_x, each_y))

# input = open("swissprot-family-group", "r")
# gene_dict = defaultdict(lambda: 0)
# for line in input:
#     line = line.strip()
#     list = [x for x in line.split()]
#     for item in list:
#         gene_dict[item] +=1
#
# for key, value in gene_dict.iteritems():
#     if value > 1:
#         print key
