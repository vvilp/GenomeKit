'''Single-layer autoencoder example using MNIST digit data.
This example shows one way to train a single-layer autoencoder model using the
handwritten MNIST digits.
This example also shows the use of climate command-line arguments.
'''

import climate
import matplotlib.pyplot as plt
import theanets
import theanets.util
import numpy as np
from utils import load_mnist, plot_layers, plot_images

def save_data(data, file_path, kmerList):
    f2 = open(file_path, 'w')
    for i in range (0, data.shape[0]):
        f2.write(kmerList[i])
        f2.write("\n")
        for j in range (0, data.shape[1]):
            f2.write("%.6f" % data[i][j])
            # if data[i][j] < 0:
            #     f2.write("%.6f" % -1)
            # else:
            #     f2.write("%.6f" % data[i][j])
            if j < data.shape[1] - 1 :
                f2.write(" ")
        f2.write("\n")
    f2.close()

def main():
    f = open('../TestData2/sig/5mer_semantic_sig_512_all_dna_new2', 'r')
    # f = open('data/6mer_count_vector_small', 'r')
    # train = [[float(x) for x in line.split()] for line in f]
    train = []
    kmerList = []
    iterC = 0
    for line in f:
        line = line.strip()
        if iterC % 2 != 0:
            sig = [float(x) for x in line.split()]
            train.append(sig)
        else:
            kmerList.append(line)
        iterC+=1

    train_data = np.array(train)
    print train_data.shape
    # net = theanets.Autoencoder([512, 256, 128, 64, 128, 256, 512])
    net = theanets.Autoencoder(
        layers=(512, 1024, 512, ('tied', 1024), ('tied', 512)),
    )

    # net = theanets.Autoencoder(
    #     layers=(512, 2048 , 512 , ('tied', 2048), ('tied', 512)),
    # )
    # net = theanets.Autoencoder(
    #     layers=(512, 256, 128, ('tied', 256), ('tied', 512)),
    # )

    # net = theanets.Autoencoder(
    #     layers=(8189, 10000, 8189, ('tied', 10000), ('tied', 8189)),
    # )

    net.train(train_data,
              patience = 1,
            #   train_batches = 10,
              input_noise=0.1,
              weight_l2=0.0001,
              algo='rmsprop',
              momentum=0.9,
              min_improvement=0.1)
    print "Finish Train"
    recons_data = net.predict(train)
    save_data(recons_data, "reconstruct_data", kmerList)

    # layer_num = 5;
    # for i in range (0, layer_num):
    #     layer_output = net.get_layer_output(train, lay_index=i)
    #     print layer_output.shape
    #     save_data(layer_output, "layer" + str(i))



if __name__ == '__main__':
    main();
