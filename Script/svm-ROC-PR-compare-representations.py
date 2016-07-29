print(__doc__)

import numpy as np
import matplotlib.pyplot as plt
from sklearn import svm, datasets
from sklearn.metrics import roc_curve, auc
from sklearn.cross_validation import train_test_split
from sklearn.preprocessing import label_binarize
from sklearn.multiclass import OneVsRestClassifier
from sklearn.metrics import precision_recall_curve
from sklearn.metrics import average_precision_score
from scipy.interpolate import interp1d
from scipy import interp
import itertools

# plt.rc('text', usetex=True)
plt.rc('font', family='serif')
marker = itertools.cycle(('D', '*', '.', 'o', 'd'))

training_file_list = ["sequence-bow-for-training.csv", "sequence-tfidf-rep-for-training.csv", "sequence-512rep-for-training.csv"]
label_list = ["KCV", "TFIDF", "SNN-REP"]
# training_file_list = ["test.data", "test.data"]

plt.figure(1, figsize=(15, 10))
plt.clf()
# plt.plot(recall[0], precision[0], label='Precision-Recall curve')
plt.xlabel('Recall', fontsize=16)
plt.ylabel('Precision', fontsize=16)
plt.ylim([0.0, 1.01])
plt.xlim([0.0, 1.01])
# plt.title('Precision and Recall (average precision:{0:0.2f})'.format(average_precision["micro"]))


for index, each_file in enumerate(training_file_list):

    f = open(each_file)

    data = np.loadtxt(f)
    X = data[:, 1:]
    y = data[:, 0]

    # class_list = [0,1,3,5]
    class_list = []
    for i in range(1,51):
        class_list.append(i)
    # print class_list

    # Binarize the output
    y = label_binarize(y, classes=class_list)
    n_classes = y.shape[1]
    print n_classes

    random_state = np.random.RandomState(0)

    # shuffle and split training and test sets
    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=.25,random_state=0)

    # Learn to predict each class against the other
    classifier = OneVsRestClassifier(svm.SVC(kernel='linear', probability=True,random_state=random_state))
    y_score = classifier.fit(X_train, y_train).decision_function(X_test)

    #================================================================
    # precision-recall
    #================================================================
    # Compute Precision-Recall and plot curve
    precision = dict()
    recall = dict()
    average_precision = dict()
    for i in range(n_classes):
        precision[i], recall[i], _ = precision_recall_curve(y_test[:, i],y_score[:, i])
        average_precision[i] = average_precision_score(y_test[:, i], y_score[:, i])

    precision["micro"], recall["micro"], _ = precision_recall_curve(y_test.ravel(),y_score.ravel())
    average_precision["micro"] = average_precision_score(y_test, y_score,average="micro")

    # Plot Precision-Recall curve
    # print i
    # print label_list[i]
    plt.plot(recall["micro"], precision["micro"],label='{0} Average Precision = {1:0.2f}'.format(label_list[index], average_precision["micro"]), linewidth=2)
    plt.legend(loc="lower right", prop={'size':16}, fancybox=True)
    # plt.show()
plt.savefig('average-pr.pdf')
