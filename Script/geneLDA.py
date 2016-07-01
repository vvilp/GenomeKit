import numpy as np
import lda

# k=5
# kmer_alph = "ACGT"
k=3
kmer_alph = "ABCDEFGHIKLMNPQRSTVWXYZ"
# gene_file = "../TestData2/alfsim-data2/DB/all_dna.fa"
# gene_file = "../TestData2/alfsim-data2/DB/all_dna.fa"
gene_file = "genes_small.faa"
f_gene = open(gene_file, "r")

def search_kmer(k, letters, kmer, kmers):
    if k == 0:
        kmers.append(kmer)
        return
    for i in range(0, len(letters)):
        kmer += letters[i]
        search_kmer(k-1, letters, kmer, kmers)
        kmer = kmer[0:len(kmer)-1]

def generate_kmers(k, letters):
    kmers = list()
    kmer = ""
    search_kmer(k, letters, kmer, kmers)
    return kmers

kmers = generate_kmers(k,kmer_alph)
# print kmers
# gene_bow = np.array()
gene_bow_list = list()
count = 0
for each_line in f_gene:
    if each_line[0] == '>':
        continue
    else:
        each_line = each_line.strip()
        l = list()
        for each_kmer in kmers:
            l.append(each_line.count(each_kmer))
        gene_bow_list.append(l)
    if count % 100 == 0:
        print count
    count+=1

print len(gene_bow_list)
gene_bow_np_array = np.array(gene_bow_list)
print gene_bow_np_array.shape

model = lda.LDA(n_topics=15, n_iter=200, random_state=1)
model.fit(gene_bow_np_array)  # model.fit_transform(X) is also available
topic_word = model.topic_word_  # model.components_ also works
n_top_words = 20
for i, topic_dist in enumerate(topic_word):
    topic_words = np.array(kmers)[np.argsort(topic_dist)][:-n_top_words:-1]
    print('Topic {}: {}'.format(i, ' '.join(topic_words)))
