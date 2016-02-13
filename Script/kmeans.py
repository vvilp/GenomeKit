# from sklearn import cluster, datasets
# iris = datasets.load_iris()
# k_means = cluster.KMeans(n_clusters=3)
# k_means.fit(iris.data)
# print(k_means.labels_[::10])
# print(iris.target[::10])
# print(iris.data)
from sklearn import cluster

cluster_num = 10
k = 5
sig_name = "5mer_semantic_sig_512_all_dna_new2"
sig_file_path = "../TestData2/sig/" + sig_name
cluster_output_path = "../TestData2/sig/" + sig_name + "_cluster_output"

f_sig = open(sig_file_path, "r")
f_output = open(cluster_output_path, "w")
kmer_label = []
kmer_sig = []

iterC = 0
for line in f_sig:
    line = line.strip()
    if iterC % 2 == 0:
        if k == 0:
            k = len(line)
            print "kmer k:%d" % k
        kmer_label.append(line)
    else:
        kmer_sig.append([float(x) for x in line.split()])
    iterC+=1

# print kmer_label
# print kmer_sig
k_means = cluster.KMeans(n_clusters=cluster_num)
k_means.fit(kmer_sig)
print(k_means.labels_)

cluster_list = [[] for k in range(cluster_num)]
for i,each in enumerate(k_means.labels_):
    cluster_list[each].append(kmer_label[i])

for i, each_list in enumerate(cluster_list):
    f_output.write("cluster %d\t" % i )
    for each in each_list:
        f_output.write("%s\t" % each)
    f_output.write("\n\n")

f_output.close()
