
sudo apt-get -y install g++
sudo apt-get -y install git

git clone https://github.com/vvilp/GenomeKit.git

cd GenomeKit
git checkout -b Dev
git pull origin Dev

g++ SemanticNeuralNetwork.cpp -o SemanticNeuralNetwork -std=c++11 -pthread
g++ Distance.cpp -o Distance -std=c++11 -pthread
