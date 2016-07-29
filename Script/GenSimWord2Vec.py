import gensim, logging
logging.basicConfig(format='%(asctime)s : %(levelname)s : %(message)s', level=logging.INFO)

k=4
sig_len = 512
gene_file = "all_dna.fa"
output_file_path = "dna-w2v-%dmer-%d-sig" % (k,sig_len)


def GeneToKmers(gene,k):
    kmer_list = []
    for i in range(0, len(gene) - k + 1):
        kmer_list.append(gene[i:i+k])
    return kmer_list

def GeneToSentences(path):
    sentences = []
    f = open(path, "r")
    for each_line in f:
        each_line = each_line.strip()
        if each_line[0] == '>':
            print each_line
        else:
            sentences.append(GeneToKmers(each_line,k))
    return sentences

sentences = GeneToSentences(gene_file)

model = gensim.models.Word2Vec(sentences, size=sig_len, sg=1)
model.save_word2vec_format(output_file_path)

# sentences = [['first', 'sentence'], ['second', 'sentence']]
# # train word2vec on the two sentences
# model = gensim.models.Word2Vec(sentences, min_count=1)
# model.save("testmode")
