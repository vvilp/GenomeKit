import numpy as np
from scipy.cluster.vq import kmeans
from scipy.spatial.distance import cdist,pdist
from sklearn import datasets
from sklearn.decomposition import RandomizedPCA
from matplotlib import pyplot as plt
from matplotlib import cm


# k = 5
sig_name = "dna-w2v-4mer-512-sig-2"
sig_file_path = "./" + sig_name
cluster_output_path = "./" + sig_name + "_cluster_output"

f_sig = open(sig_file_path, "r")
f_output = open(cluster_output_path, "w")
kmer_label = []
kmer_sig = []

iterC = 0
for line in f_sig:
    line = line.strip()
    if iterC % 2 == 0:
        # if k == 0:
        #     k = len(line)
            # print "kmer k:%d" % k
        kmer_label.append(line)
    else:
        kmer_sig.append([float(x) for x in line.split()])
    iterC+=1

##### data #####
# load digits dataset
data = kmer_sig #datasets.load_digits()
t = kmer_label #data['target']

# perform PCA dimensionality reduction
pca = RandomizedPCA(n_components=2).fit(data)
X = pca.transform(data)


##### cluster data into K=1..20 clusters #####
K_MAX = 20
KK = range(1,K_MAX+1)

KM = [kmeans(X,k) for k in KK]
centroids = [cent for (cent,var) in KM]
D_k = [cdist(X, cent, 'euclidean') for cent in centroids]
cIdx = [np.argmin(D,axis=1) for D in D_k]
dist = [np.min(D,axis=1) for D in D_k]

tot_withinss = [sum(d**2) for d in dist]  # Total within-cluster sum of squares
totss = sum(pdist(X)**2)/X.shape[0]       # The total sum of squares
betweenss = totss - tot_withinss          # The between-cluster sum of squares

##### plots #####
kIdx = 4        # K=10
clr = cm.spectral( np.linspace(0,1,10) ).tolist()
mrk = 'os^p<dvh8>+x.'

# elbow curve
fig = plt.figure()
# plt.figure(1, figsize=(15, 10))
ax = fig.add_subplot(111)
ax.plot(KK, betweenss/totss*100, 'b*-')
ax.plot(KK[kIdx], betweenss[kIdx]/totss*100, marker='o', markersize=12,
    markeredgewidth=2, markeredgecolor='r', markerfacecolor='None')
ax.set_ylim((0,100))
plt.grid(True)
# plt.figure(figsize=(20, 20))
plt.xlabel('Number of clusters')
plt.ylabel('Percentage of variance explained (%)')
plt.savefig("4mer-kmean-cluster-Elbow.pdf")
# plt.title('Elbow for KMeans clustering')

# show centroids for K=10 clusters
# plt.figure()
# for i in range(kIdx+1):
#     img = pca.inverse_transform(centroids[kIdx][i]).reshape(8,8)
#     ax = plt.subplot(3,4,i+1)
#     ax.set_xticks([])
#     ax.set_yticks([])
#     plt.imshow(img, cmap=cm.gray)
#     plt.title( 'Cluster %d' % i )

# compare K=10 clustering vs. actual digits (PCA projections)
fig = plt.figure()
# ax = fig.add_subplot(121)
# for i in range(10):
#     ind = (t==i)
#     ax.scatter(X[ind,0],X[ind,1], s=35, c=clr[i], marker=mrk[i], label='%d'%i)
# plt.legend()
# plt.title('Actual Digits')



print cIdx[kIdx]

print X

# plt.figure(figsize=(15, 7))
ax = fig.add_subplot(111)
for i in range(len(cIdx[kIdx])):
    cIndex = cIdx[kIdx][i]
    label = kmer_label[i]
    ax.scatter(X[i,0],X[i,1], s=35, c=clr[cIndex], marker=mrk[cIndex]);
    plt.annotate(label,
                 size=5,
                 xy=(X[i,0],X[i,1]),
                 xytext=(4, 2),
                 textcoords='offset points',
                 ha='right',
                 va='bottom')

# ax = fig.add_subplot(111)
# for i in range(kIdx+1):
#     ind = (cIdx[kIdx]==i)
#     ax.scatter(X[ind,0],X[ind,1], s=35, c=clr[i], marker=mrk[i], label='cluster ID:%d'%i)
#     for label, x, y in zip(kmer_label, X[ind,0],X[ind,1]):
#         plt.annotate(label,
#                      xy=(x, y),
#                      xytext=(5, 2),
#                      textcoords='offset points',
#                      ha='right',
#                      va='bottom')


plt.legend()
# plt.title('Clusters Number:%d'%KK[kIdx])

# plt.show()
plt.savefig("4mer-kmean-cluster.pdf")
