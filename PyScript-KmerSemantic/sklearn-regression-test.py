import numpy as np
from sklearn import linear_model
from sklearn import svm

# n_samples, n_features = 10, 2
# np.random.seed(0)
# y = np.random.randn(n_samples)
# X = np.random.randn(n_samples, n_features)
#
# print y
# print X
#
# clf = linear_model.SGDRegressor()
# clf.fit(X, y)
#
# print clf.predict ([[1, 0.]])

f = open("test_data/1.csv")

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
