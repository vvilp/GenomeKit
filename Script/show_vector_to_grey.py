from PIL import Image
import numpy as np

data = np.random.random((32,32))

#Rescale to 0-255 and convert to uint8
rescaled = (255.0 / data.max() * (data - data.min())).astype(np.uint8)
print rescaled.shape
im = Image.fromarray(rescaled)
show_size = (100,100)
im = im.resize(show_size, Image.ANTIALIAS)
im.show()
im.save('randomvec.png')

# k=5
# sig_name = "5mer_semantic_sig_1024_all_dna_new_all"
# sig_file = "../TestData2/sig/" + sig_name
# f_sig = open(sig_file, "r")
#
# sig_dict = dict()
# label = ""
# sig = []
# iterC = 0
# for line in f_sig:
#     line = line.strip()
#     if iterC % 2 == 0:
#         label = line
#         if k == 0:
#             k = len(line)
#             print "k:%d" % k
#     else:
#         sig = [float(x) for x in line.split()]
#         sig_dict[label] = np.array(sig)
#     iterC+=1
#
# # print sig_dict["AAAAA"].max()
# # print sig_dict["AAAAA"].min()
# kmer_list_to_show = ["AAAAA","AAAAC","AAAAG","AAAAT","CCCCC","GGGGG","TTTTT"]
#
# for each_kmer in kmer_list_to_show:
#     # print each_kmer
#     data = sig_dict[each_kmer]
#     rescaled = (255.0 / data.max() * (data - data.min()))
#     print rescaled.shape
#     rescaled = np.reshape(rescaled, (-1, (rescaled.shape[0])**(0.5)))
#     # print rescaled
#     im = Image.fromarray(rescaled)
#     show_size = (100,100)
#     im = im.resize(show_size, Image.ANTIALIAS)
#     im.convert('RGB').save(each_kmer+'.jpeg')


# data = sig_dict["AAAAA"]
# rescaled = (255.0 / data.max() * (data - data.min()))
# print rescaled.shape
# rescaled = np.reshape(rescaled, (-1, (rescaled.shape[0])**(0.5)))
# # print rescaled
# im = Image.fromarray(rescaled)
# show_size = (500,500)
# im = im.resize(show_size, Image.ANTIALIAS)
# im.convert('RGB').save('new.jpeg')
