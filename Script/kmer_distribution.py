from matplotlib.ticker import FuncFormatter
import matplotlib.pyplot as plt
import numpy as np
import math
k=3
show_num = 500

f = open("data/swissprot-sequences.faa", "r")

kmer_dict = dict()
for each_line in f:
    if each_line[0] == '>':
        continue
    else:
        gene = each_line.strip()
        for i in range(0, len(gene) - k + 1):
            kmer = gene[i:i+k]
            if kmer in kmer_dict:
                kmer_dict[kmer] +=1
            else:
                kmer_dict[kmer] = 1

# print kmer_dict
kmer_list = []
count_list = []
rank_list = []
log_prob = []
log_rank = []
linear_prob = []
linear_rank = []
count = 0
for kmer in sorted(kmer_dict, key=kmer_dict.get, reverse=True):
    # print kmer, kmer_dict[kmer]
    count += 1
    kmer_list.append(kmer)
    count_list.append(kmer_dict[kmer])
    rank_list.append(count)

    log_prob.append(math.log((float)(kmer_dict[kmer])))
    log_rank.append(math.log(count))

    if count == 1 or count == show_num:
        linear_prob.append(math.log((float)(kmer_dict[kmer])))
        linear_rank.append(math.log(count))

    if count == show_num:
        break

print count

# print kmer_list
# print count_list
# print log_prob
# print log_rank

# kmer distribution
# x = np.arange(show_num)
# plt.figure(0, figsize=(1000, 50))
# plt.bar(x, count_list)
# plt.xticks(x + 0.5, kmer_list, rotation='vertical')
# plt.tight_layout()
# plt.savefig('test.pdf',dpi=100)

# plt.title("Kmer frequency - rank")
# plt.plot(rank_list, count_list, linewidth=1.0)
# # plt.plot(linear_rank, linear_prob, linewidth=2.0)
# # plt.tight_layout()
# plt.ylabel('Kmer frequency')
# plt.xlabel('Kmer rank')
# plt.savefig('test.pdf',dpi=100)

# log(prob(r)) - log(r)
plt.title("Kmer log frequency - log rank")
plt.plot(log_rank, log_prob, linewidth=1.0)
plt.plot(linear_rank, linear_prob, linewidth=2.0)
# plt.tight_layout()
plt.ylabel('Kmer log frequency')
plt.xlabel('Kmer log rank')
plt.savefig('kmer_log_log.pdf',dpi=100)
# # plt.show()
