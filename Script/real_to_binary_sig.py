
input = open("protein-3mer-512", "r");
output = open("protein-3mer-512-binary", "w");

for i, line in enumerate(input):
    line = line.strip()
    if i % 2 == 0:
        output.write(line + "\n")
    else:
        list = [float(x) for x in line.split()]
        for each in list:
            if each < 10:
                output.write("0 ");
            else:
                output.write("1 ");
        output.write("\n");
