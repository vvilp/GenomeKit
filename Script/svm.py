# import numpy as np
# from sklearn import svm
# from sklearn import metrics
# from sklearn.cross_validation import train_test_split
# import logging
#
# print(__doc__)
#
# # Display progress logs on stdout
# logging.basicConfig(level=logging.INFO, format='%(asctime)s %(message)s')
#
# f = open("sequence-1024rep-for-training.csv")
# data = np.loadtxt(f)
#
# X = data[:, 1:]
# y = data[:, 0]
#
# X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.25)
# clf = svm.SVC().fit(X_train, y_train)
# y_predicted = clf.predict(X_test)
#
# print "Classification report for %s" % clf
# print
# print metrics.classification_report(y_test, y_predicted)
# # print
# # print "Confusion matrix"
# # print metrics.confusion_matrix(y_test, y_predicted)

print(__doc__)
from sklearn import metrics
import numpy as np
import matplotlib.pyplot as plt
from sklearn import svm, datasets
from sklearn.metrics import roc_curve, auc
from sklearn.cross_validation import train_test_split
from sklearn.preprocessing import label_binarize
from sklearn.multiclass import OneVsRestClassifier
from sklearn.metrics import precision_recall_curve
from sklearn.metrics import average_precision_score
from scipy import interp

# Import some data to play with
# iris = datasets.load_iris()
# X = iris.data
# y = iris.target

f = open("sequence-tfidf-rep-for-training.csv")
# f = open("test.data")

data = np.loadtxt(f)
X = data[:, 1:]
y = data[:, 0]
# n_classes = 50

# class_list = [0,1,3,5]
class_list = []


for i in range(1,51):
    class_list.append(i)
# print class_list

# Binarize the output
y = label_binarize(y, classes=class_list)
n_classes = y.shape[1]
print n_classes

# # Add noisy features to make the problem harder
random_state = np.random.RandomState(0)
# n_samples, n_features = X.shape
# X = np.c_[X, random_state.randn(n_samples, 200 * n_features)]

# shuffle and split training and test sets
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=.25,random_state=0)

# Learn to predict each class against the other
classifier = OneVsRestClassifier(svm.SVC(kernel='linear', probability=True,random_state=random_state))
y_predicted = classifier.fit(X_train, y_train).predict(X_test)
print y_predicted
print "Classification report for "
print
print metrics.classification_report(y_test, y_predicted)
