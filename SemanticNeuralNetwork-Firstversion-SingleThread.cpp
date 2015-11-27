#include <iostream>
#include <map>
#include <vector>
#include <random>
#include <utility>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <ctime>

#include "Util.hpp"
using namespace std;

class SemanticNeuralNetwork {
public:
	const int iter = 20;
	const int hlsize = 100;
	//hidden layer size
	const int shiftSize = 3;
	const float alpha = 0.1;

	map<string, pair<int, int>> vocabMap;
	// vocabulary map, word -> index,count
	int vocabSize = 0;

	vector<int> contextsIndexVec;

	float **WIH;
	//input -> hidden weight  | vocabSize ＊ hlsize
	//WIH[k][i] : weight of kth input word to ith hidden unit

	float **WHO;
	//hidden -> output weight | hlsize * vocabSize
	//WHO[i][j] weight of ith hidden unit to jth output unit

	//float *hidden;
	//float *WIHe;

	void Init(const vector<string> & contexts) {
		vocabSize = 0;
		for (size_t i = 0; i < contexts.size(); i++) {
			string word = contexts[i];
			if (vocabMap.find(word) == vocabMap.end()) {
				pair<int, int> p(vocabSize, 1);
				vocabMap[word] = p;
				vocabSize++;
			} else {
				vocabMap[word].second++;
			}
			contextsIndexVec.push_back(vocabMap[word].first);
		}

		cout << "vocabSize:" << vocabSize << endl;

		WIH = new float*[vocabSize];
		for (size_t i = 0; i < vocabSize; i++) {
			WIH[i] = new float[hlsize];
			for (size_t j = 0; j < hlsize; j++) {
                WIH[i][j] = UT_Math::RandFloat(-0.05, 0.05);
			}
		}
		WHO = new float*[hlsize];
		for (size_t i = 0; i < hlsize; i++) {
			WHO[i] = new float[vocabSize];
			for (size_t j = 0; j < vocabSize; j++) {
                WHO[i][j] = UT_Math::RandFloat(-0.05, 0.05);
			}
		}

		//hidden = new float[hlsize];
		//WIHe = new float[hlsize];
		cout << "Init Complete" << endl;
	}

	void TrainEach(const vector<int> & inputWordsIndex, int outputWordIndex) {

		int inputSize = inputWordsIndex.size();
		int localIter = iter;
		float *hidden = new float[hlsize];
		float *WIHe = new float[hlsize];
		while (localIter-- != 0) {
			memset(hidden, 0, sizeof(float) * hlsize);
			memset(WIHe, 0, sizeof(float) * hlsize);

			// input -> hidden
			for (int k = 0; k < inputSize; k++) {
				for (int i = 0; i < hlsize; i++) {
					hidden[i] = hidden[i] + WIH[inputWordsIndex[k]][i];
				}
			}

			for (int j = 0; j < vocabSize; j++) {
				float e = 0;
				float outputJ = 0;
				float target = (j == outputWordIndex) ? 1.0 : 0;

				for (int i = 0; i < hlsize; i++) {
					outputJ += hidden[i] * WHO[i][j];
				}

                outputJ = UT_Math::sigmoid(outputJ);
				e = alpha * (target - outputJ) * outputJ * (1 - outputJ);

				// for learning weight of input to hidden
				for (int i = 0; i < hlsize; i++) {
					WIHe[i] = WIHe[i] + e * WHO[i][j];
				}

				// learn weight of hidden to output layer
				for (int i = 0; i < hlsize; i++) {
					WHO[i][j] = WHO[i][j] + e * hidden[i];
				}
			}

			//learn weight of input to hidden
			for (int k = 0; k < inputSize; k++) {
				for (int i = 0; i < hlsize; i++) {
					WIH[inputWordsIndex[k]][i] = WIH[inputWordsIndex[k]][i] + WIHe[i];
				}
			}


		}

		delete[] hidden;
		delete[] WIHe;
	}

	void Train() {
		int count = 0;
		int size = contextsIndexVec.size();

		for (int i = 0; i < size; i++) {
			//cout << "training: " << i << "| All: " << contextsIndexVec.size() << endl;
			int outputIndex = contextsIndexVec[i];
			vector<int> inputWordsIndex;
			inputWordsIndex.clear();
			for (int j = 1; j < shiftSize; j++) {
				if (i - j >= 0)
					inputWordsIndex.push_back(contextsIndexVec[i - j]);
				if (i + j < size)
					inputWordsIndex.push_back(contextsIndexVec[i + j]);
			}

			TrainEach(inputWordsIndex, outputIndex);
			count++;
			printf("\rFinish: %.2f%%", (float)count / (float)size * 100.0);
		}
	}

	void Save() {
		ofstream ofile("C:\\Users\\u374220\\Desktop\\WordRepresentation");
		map<string, pair<int, int>>::iterator iter;

		for (iter = vocabMap.begin(); iter != vocabMap.end(); iter++) {
			ofile << iter->first << " ";
			int wordIndex = iter->second.first;
			for (int i = 0; i < hlsize; i++) {
				ofile << WIH[wordIndex][i] << " ";
			}
			ofile << endl;
		}

		ofile.close();
		cout << "Save completed" << endl;
	}

	void Test() {
	}
};

void ReformatString(string & line)
{
	transform(line.begin(), line.end(), line.begin(), ::tolower);
	for (int i = 0; i < line.size(); i++)
	{
		if (line[i] <'a' || line[i] > 'z') {
			line[i] = ' ';
		}
	}
}

void TestSNN() {
	srand(time(0));

	ifstream infile("C:\\Users\\u374220\\Desktop\\testData.txt");
	vector<string> contexts;
	for (string line; getline(infile, line);) {
		ReformatString(line);
		if (!line.empty())
            UT_String::split(line, " ", contexts);
	}

	cout << "word size:" << contexts.size() << endl;

	SemanticNeuralNetwork snn;
	snn.Init(contexts);
	snn.Train();
	snn.Save();
}

int main() {
	TestSNN();

	getchar();
}
