import gensim, logging, math
#logging.basicConfig(format='%(asctime)s : %(levelname)s : %(message)s', level=logging.INFO)

gene_file = "genes500.fa"
k=5
genes = []
numTopics = 20
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
            #print each_line
            pass
        else:
            genes.append(each_line)
            sentences.append(GeneToKmers(each_line,k))
    return sentences

def CalcuScore(numBoth, num):
    if num == 0 or numBoth == 0:
        return 0
    a = (float)(numBoth)/(float)(num)
    #print a
    return math.log(a+1.0)

sentences = GeneToSentences(gene_file)

dictionary = gensim.corpora.Dictionary(sentences)
corpus = [dictionary.doc2bow(text) for text in sentences]
#print corpus
lda = gensim.models.ldamodel.LdaModel(corpus=corpus, id2word=dictionary, num_topics=numTopics, update_every=1, chunksize=10, passes=1)
# lda.print_topics(10)

# measure topic quality
topics = lda.show_topics(formatted=False, num_words=10, num_topics=numTopics)

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
    #break


#print lda[corpus[0]]
#print lda.get_topic_terms(0,20)
#lda.save("lda.save")

#print lda.show_topics(formatted=False)[0][1][0][0] [0 topic0][0:topicIndex 1:topic word and weight][0 first topic][0:word 1:topic]

