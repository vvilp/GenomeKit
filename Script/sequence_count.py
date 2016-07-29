
f = open("swissprot-sequences.faa", "r")

max_len = 0;
min_len = 9999999;
avg_len = 0;
count = 0
for line in f:
    if line[0] != '>':
        if len(line) > max_len:
            max_len = len(line)
        if len(line) < min_len:
            min_len = len(line)
        avg_len += len(line)
        count +=1

print max_len
print min_len
print avg_len / count
