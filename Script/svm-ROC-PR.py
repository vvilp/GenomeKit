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

f = open("sequence-bow-for-training.csv")
# f = open("sequence-512rep-for-training.csv")
# f = open("test.data")

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

# Compute ROC curve and ROC area for each class
fpr = dict()
tpr = dict()
roc_auc = dict()
for i in range(n_classes):
    fpr[i], tpr[i], _ = roc_curve(y_test[:, i], y_score[:, i])
    roc_auc[i] = auc(fpr[i], tpr[i])

# Compute micro-average ROC curve and ROC area
fpr["micro"], tpr["micro"], _ = roc_curve(y_test.ravel(), y_score.ravel())
roc_auc["micro"] = auc(fpr["micro"], tpr["micro"])


##############################################################################
# Plot of a ROC curve for a specific class
# plt.figure()
# plt.plot(fpr[2], tpr[2], label='ROC curve (area = %0.2f)' % roc_auc[2])
# # plt.plot([0, 1], [0, 1], 'k--')
# plt.xlim([0.0, 1.0])
# plt.ylim([0.0, 1.05])
# plt.xlabel('False Positive Rate')
# plt.ylabel('True Positive Rate')
# plt.title('Receiver operating characteristic example')
# plt.legend(loc="lower right")
# plt.show()


##############################################################################
# Plot ROC curves for the multiclass problem

# Compute macro-average ROC curve and ROC area

# First aggregate all false positive rates
all_fpr = np.unique(np.concatenate([fpr[i] for i in range(n_classes)]))

# Then interpolate all ROC curves at this points
mean_tpr = np.zeros_like(all_fpr)
for i in range(n_classes):
    mean_tpr += interp(all_fpr, fpr[i], tpr[i])

# Finally average it and compute AUC
mean_tpr /= n_classes

fpr["macro"] = all_fpr
tpr["macro"] = mean_tpr
roc_auc["macro"] = auc(fpr["macro"], tpr["macro"])

# Plot all ROC curves
plt.figure(0, figsize=(15, 10))
plt.clf()
plt.plot(fpr["micro"], tpr["micro"],label='Average Sensitivity (area = {0:0.2f})' ''.format(roc_auc["micro"]), linewidth=2)
# plt.plot(fpr["macro"], tpr["macro"],label='macro-average (area = {0:0.2f})' ''.format(roc_auc["macro"]), linewidth=2)
plt.xlim([0.0, 1.01])
plt.ylim([0.0, 1.01])
plt.xlabel('Fall-out', fontsize=16)
plt.ylabel('Sensitivity', fontsize=16)
# plt.title('SV')
plt.legend(loc="lower right", prop={'size':16}, fancybox=True, framealpha=0.5)
# plt.show()
plt.savefig('average-roc.pdf')

plt.figure(1, figsize=(15, 10))
plt.clf()
for i in range(n_classes):
    plt.plot(fpr[i], tpr[i], label='Family class {0} (area = {1:0.2f})' ''.format(i, roc_auc[i]))

# plt.plot([0, 1], [0, 1], 'k--')
plt.xlim([0.0, 1.01])
plt.ylim([0.0, 1.01])
plt.xlabel('Fall-out')
plt.ylabel('Sensitivity')
# plt.title('SV')
plt.legend(loc="lower right", prop={'size':7}, fancybox=True, framealpha=0.5)
# plt.show()
plt.savefig('each-roc.pdf')

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
plt.figure(2, figsize=(15, 10))
plt.clf()
# plt.plot(recall[0], precision[0], label='Precision-Recall curve')
plt.plot(recall["micro"], precision["micro"],label='Average precision-recall curve (area = {0:0.2f})'''.format(average_precision["micro"]), linewidth=2)
plt.xlabel('Recall', fontsize=16)
plt.ylabel('Precision', fontsize=16)
plt.ylim([0.0, 1.01])
plt.xlim([0.0, 1.01])
# plt.title('Precision and Recall (average precision:{0:0.2f})'.format(average_precision["micro"]))
plt.legend(loc="lower right", prop={'size':16}, fancybox=True)
# plt.show()
plt.savefig('average-pr.pdf')


# Plot Precision-Recall curve for each class
plt.figure(3, figsize=(15, 10))
plt.clf()
# plt.plot(recall["micro"], precision["micro"],label='Average precision-recall curve (area = {0:0.2f})' ''.format(average_precision["micro"]))

for i in range(n_classes):
    plt.plot(recall[i], precision[i],label='Family class {0} (area = {1:0.2f})' ''.format(i, average_precision[i]))

plt.xlim([0.0, 1.01])
plt.ylim([0.0, 1.01])
plt.xlabel('Recall')
plt.ylabel('Precision')
# plt.title('Precision-Recall Curve')
plt.legend(loc="lower left", prop={'size':7}, fancybox=True)

# plt.show()
plt.savefig('each-pr.pdf')
