import gensim, logging, math
import os
import re
import numpy as np

logging.basicConfig(format='%(asctime)s : %(levelname)s : %(message)s', level=logging.INFO)

gene_file = "genes_small.faa"
kmer_sig_path = "genes_small_2mer_256_gensim"
output_gene_sig_path = "protein_gene_sig"

# gene_file = "all_dna.fa"
# kmer_sig_path = "5mer_semantic_sig_512_all_dna_new_all2"
# output_gene_sig_path = "gene_sig"

f_output = open(output_gene_sig_path, "w")

k=2
genes = []
gene_labels = []
numTopics = 100

def save_sig(label, sig, file):
    file.write(label)
    file.write("\n")
    for i in range (0, sig.shape[0]):
        file.write("%.6f" % sig[i])
        if i < sig.shape[0] - 1 :
            file.write(" ")
    file.write("\n")

def GeneToKmers(gene,k):
    kmer_list = []
    for i in range(0, len(gene) - k + 1):
        kmer_list.append(gene[i:i+k])
    return kmer_list

def GetKmerSig(path,k):
    f_sig = open(path,"r")
    sig_dict = dict()
    label = ""
    sig = []
    iterC = 0
    for line in f_sig:
        line = line.strip()
        if iterC % 2 == 0:
            label = line
            if k == 0:
                k = len(line)
                print "k:%d" % k
        else:
            sig = [float(x) for x in line.split()]
            sig_dict[label] = np.array(sig)
        iterC+=1
    return sig_dict

def GeneToSentences(path):
    sentences = []
    f = open(path, "r")
    for each_line in f:
        each_line = each_line.strip()
        if each_line[0] == '>':
            # s_array = [x for x in each_line.split(',')]
            # result = re.findall(r'\d+', s_array[0]) #find integer
            # gene_label = "S" + result[1] +  "/" + result[0].zfill(5)
            # gene_labels.append(gene_label)
            # print gene_label
            gene_labels.append(each_line[1:])
        else:
            genes.append(each_line)
            sentences.append(GeneToKmers(each_line,k))
    return sentences

sentences = GeneToSentences(gene_file)

dictionary = gensim.corpora.Dictionary(sentences)
corpus = [dictionary.doc2bow(text) for text in sentences]
#print corpus
lda = gensim.models.ldamodel.LdaModel(corpus=corpus, id2word=dictionary, num_topics=numTopics, update_every=1, chunksize=10, passes=1)
# lda.print_topics(10)

# measure topic quality
topics = lda.show_topics(formatted=False, num_words=10, num_topics=numTopics)

kmer_sig_dict = GetKmerSig(kmer_sig_path,k)
topic_sig_lists = []
for i,topic in enumerate(topics):
    topicIndex = topic[0]
    topicContent = topic[1]
    topic_sig = np.zeros(kmer_sig_dict[kmer_sig_dict.keys()[0]].shape[0])
    # print "Topic:%d" % i
    for i in range(0, len(topicContent)):
        # print topicContent[i][1]
        # print kmer1
        # print kmer_sig_dict[kmer1]
        kmer1 = topicContent[i][0]
        topic_sig = topic_sig + kmer_sig_dict[kmer1] * topicContent[i][1]
    topic_sig_lists.append(topic_sig)
    # print (topic_sig


# print len(corpus)
# document_topics = lda.get_document_topics(corpus[0])
# print document_topics[0][1]

for i in range (len(corpus)):
    gene_topics=lda.get_document_topics(corpus[i])
    gene_sig = np.zeros(kmer_sig_dict[kmer_sig_dict.keys()[0]].shape[0])
    for each_topic in gene_topics:
        gene_sig = gene_sig + (topic_sig_lists[each_topic[0]] * each_topic[1])
        # gene_sig = gene_sig + (topic_sig_lists[each_topic[0]])
    save_sig(gene_labels[i], gene_sig, f_output)

def CalcuScore(numBoth, num):
    if num == 0 or numBoth == 0:
        return 0
    a = (float)(numBoth)/(float)(num)
    #print a
    return math.log(a+1.0)

avgScore = 0
for topic in topics:
    topicIndex = topic[0]
    topicContent = topic[1]
    #print topicContent
    topicScore = 0

    for i in range(0, len(topicContent)):
        # print topicContent[i][1]
        kmer1 = topicContent[i][0]
        for j in range(i+1, len(topicContent)):
            if i == j:
                break
            kmer2 = topicContent[j][0]
            numKmer1 = 0
            numKmer2 = 0
            numBoth = 0
            for gene in genes:
                if kmer1 in gene:
                    numKmer1+=1
                if kmer2 in gene:
                    numKmer2+=1
                if kmer1 in gene and kmer2 in gene:
                    numBoth+=1
            topicScore += CalcuScore(numBoth,numKmer1) + CalcuScore(numBoth,numKmer2)
    print topicScore
    avgScore += topicScore

avgScore = avgScore / len(topics)

print "%d %f" % (len(topics), avgScore)
