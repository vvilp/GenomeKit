from collections import defaultdict

f_sequence = open("family_classification_sequences.csv", "r")
f_meta = open("family_classification_metadata_csv.csv", "r")

output_sequence = open("swissprot-sequences.faa", "w")
output_family = open("swissprot-family-group", "w")

gene_family_dict = defaultdict(list)

# num_lines = sum(1 for line in f_meta)
# print num_lines
seq_list = f_sequence.read().splitlines()

for i, line in enumerate(f_meta):
    if i == 0:
        continue
    line = line.strip()
    list = [x for x in line.split(",")]
    sequence_id = list[0]
    sequence_family = list[3]
    gene_family_dict[sequence_family].append((i,sequence_id))
    # if i == 4001:
    #     break

each_count = 100
total_count = 5000
count = 0
sequence_set = set()
for key, value in gene_family_dict.iteritems():
    if len(value) < each_count:
        continue
    each_family_count = 0
    for i, item in enumerate(value):
        if each_family_count == each_count:
            break
        if item[1] not in sequence_set:
            output_sequence.write(">" + item[1] + "\n")
            output_sequence.write(seq_list[item[0]] + "\n")
            output_family.write(item[1] + " ")
            sequence_set.add(item[1])
            each_family_count+=1
            count+=1
    output_family.write("\n")
    if total_count == count:
        break


# print gene_family_dict["v110"]
