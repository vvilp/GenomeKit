#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <random>
#include <sstream>
#include <thread>
#include <utility>
#include <vector>
#include "Util.hpp"
#include "SemanticNeuralNetwork.hpp"

using namespace std;

bool IsBreakChar(char a) {
	if (a < 'a' || a > 'z') {
		if (a != ' ' && a != '.' && a != '-') {
			return true;
		}
	}
	return false;
}

void TrainTextData(string path) {
	std::ifstream t(path);
	std::stringstream buffer;
	buffer << t.rdbuf();
	string wholeContent = buffer.str();
	transform(wholeContent.begin(), wholeContent.end(), wholeContent.begin(), ::tolower);
	for (int i = 0; i < wholeContent.size(); i++) {
		if (wholeContent[i] == '\n' || wholeContent[i] == ',') {
			wholeContent[i] = ' ';
		}
		if (IsBreakChar(wholeContent[i])) {
			wholeContent[i] = '.';
		}
	}
	vector<string> sentenceArray;
	UT_String::split(wholeContent, ".", sentenceArray);

	cout << "Sentence count: " << sentenceArray.size() << endl;

	SemanticNeuralNetwork snn;
	snn.InitTextTraining();
	snn.InitTrainingData(sentenceArray);
	snn.Train();
	snn.Save(path + ".rep");
}

vector<string> GetKmers(string gene, int kmerSize) {
	vector<string> kmers;
	for (size_t i = 0; i < gene.size() - kmerSize + 1; i++) {
		string kmer = gene.substr(i, kmerSize);
		kmers.push_back(kmer);
	}
	return kmers;
}

void TrainGenome(string path) {
	int kmerSize = 3;
	int hidlayerSize = 256;
	std::ifstream t(path);
	string line = "";

	vector<vector<string>> kmerGenes;
	string gene = "";
	while (getline(t, line)) {
		if (line[0] == '>') {
			if (gene != "") {
				vector<string> kmers = GetKmers(gene, kmerSize);
				kmerGenes.push_back(kmers);
				gene = "";
			}
		} else {
			gene.append(line);
		}
	}

	cout << "Genes num: " << kmerGenes.size() << endl;
	SemanticNeuralNetwork snn;
	snn.InitGenomeTraining(kmerSize,hidlayerSize);
	snn.InitTrainingData(kmerGenes);
	snn.Train();
	snn.Save(path + "_" + to_string(kmerSize) + "mer_" + to_string(hidlayerSize) + "_sig");
}

int main(int arg, char *argvs[]) {
	srand(time(0));
	string path(argvs[1]);
	cout << path << endl;
	TrainGenome(path);
}
