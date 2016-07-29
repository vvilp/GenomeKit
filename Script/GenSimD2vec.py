import gensim, logging
from collections import namedtuple
logging.basicConfig(format='%(asctime)s : %(levelname)s : %(message)s', level=logging.INFO)

k=3
sig_len = 512
gene_file = "swissprot-sequences.faa"
output_file_path = "d2v-%dmer-%d-sig" % (k,sig_len)
gene_sig_file_path = gene_file + "-d2v-%dmer-%d-sig" % (k,sig_len)



def save_sig(label, sig, file):
    file.write(label[0])
    file.write("\n")
    for i in range (0, len(sig)):
        file.write("%.3f" % sig[i])
        if i < len(sig) - 1 :
            file.write(" ")
    file.write("\n")

def GeneToKmers(gene,k):
    kmer_list = []
    for i in range(0, len(gene) - k + 1):
        kmer_list.append(gene[i:i+k])
    return kmer_list

def GeneToSentences(path):
    SentimentDocument = namedtuple('TaggedDocument', 'words tags')
    sentences = []
    f = open(path, "r")
    label = ""
    for each_line in f:
        each_line = each_line.strip()
        if each_line[0] == '>':
            label = each_line[1:]
            # print label
        else:
            # GeneToKmers(each_line,k)
            sentences.append(SentimentDocument(GeneToKmers(each_line,k), [label]))
            # sentences.append(gensim.models.doc2vec.LabeledSentence(words=GeneToKmers(each_line,k), labels=[label]))
    return sentences

sentences = GeneToSentences(gene_file)

# for i, each in enumerate(sentences):
#     print each.tags

model = gensim.models.Doc2Vec(sentences, size=sig_len)
# model.save_word2vec_format(output_file_path)

# print model.wmdistance(sentences[0].words, sentences[1].words)
# print model.wmdistance(sentences[0].words, sentences[2].words)
gene_sig_file = open(gene_sig_file_path, "w");
for i, each in enumerate(sentences):
    save_sig(each.tags, model.docvecs[i], gene_sig_file)


# print model.docvecs[0]

# sentences = [['first', 'sentence'], ['second', 'sentence']]
# # train word2vec on the two sentences
# model = gensim.models.Word2Vec(sentences, min_count=1)
# model.save("testmode")
