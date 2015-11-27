import numpy as np
from sklearn import linear_model
from sklearn import svm


f = open("temp/output.csv")
data = np.loadtxt(f)

# print data

X = data[:, 0:2]  # select columns 1 through end
y = data[:, 2]

clf = svm.SVR()
clf.fit(X, y)

print clf.predict(X)

# f_test = open("test_data/test.csv")
# data_test = np.loadtxt(f_test)
#
# print data_test
#
# print clf.predict(data_test)
