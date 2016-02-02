import os

jf_command = "jellyfish count -m 6 -s 100M -t 10 -C {0} -o {1}"
jf_dump_command = "jellyfish dump {0} > {1}"



file_path = "data/c23oTests.fa"

f = open(file_path, "r")

fw = None
prev_line = None
file_name = None
for line in f:
    # line = line.strip()
    # print line
    # if line
    if line[0] == '>':
        file_name = line[1:line.find(' ')]
        prev_line = line
        fw = open(file_name, "w")
    else:
        fw.write(prev_line)
        fw.write(line.upper())
        fw.close();
        command = jf_command.format(file_name, file_name+".jf")
        os.system(command)
        command = jf_dump_command.format(file_name+".jf", file_name+".fa")
        os.system(command)
