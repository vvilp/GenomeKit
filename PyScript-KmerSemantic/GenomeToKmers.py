
f = open("Data/NC_017340_Staphylococcus_aureus_04_02981_uid161969.ffn","r")
output_f = open("Data/S_aureus_kmer_output","w")


for line in f:
    if line[0] == '>':
        continue;

    # print line

    for i in range(0,len(line) - 6):
        # print line[i:i+6] + " "
        output_f.write(line[i:i+6] + " ")

    # break;
output_f.close()
