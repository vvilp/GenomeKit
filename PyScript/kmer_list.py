import os

path = "kmer_count_10"
file_list = os.listdir(path)
# print file_list
kmer_set = set()
for each in file_list:
    f_path = path + "/" + each
    f_size = os.path.getsize(f_path)
    # print '{0} : {1}'.format(f_path, f_size)
    if f_size < 1000000 :
        f = open(f_path,"r")
        lines = [line.rstrip('\n') for line in f]
        for line in lines:
            if line[0] != '>':
                kmer_set.add(line);

print len(kmer_set)
