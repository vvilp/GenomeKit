
import os

input_file_path = "../TestData3_test89/homologs.csv"
input_file = open(input_file_path, "r")

maxNum = 0

for line in input_file:
    line = line.strip()
    # s_array = [x for x in line.split()]
    num = len(line.split(','))
    if maxNum < num:
        maxNum = num

print maxNum
