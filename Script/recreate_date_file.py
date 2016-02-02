import os

f = open("data/512_6mer_count_vector","r")
fw_label = open("6mer_label","w")
fw_count = open("6mer_count_vector","w")
lines = [line.rstrip('\n') for line in f]

for line in lines:
    if len(line) == 6:
        fw_label.write(line)
        fw_label.write("\n")
    else:
        fw_count.write(line.strip())
        fw_count.write("\n")
