#include <iostream>
#include <map>
#include <vector>
#include <random>
#include <utility>
#include <sstream>
#include <fstream>
#include <thread>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <queue>

#include "Util.hpp"
using namespace std;

struct VocabNode {

	string word;
	int index;
	int count;

	int nodeCode;
	vector<int> codeArray;
	VocabNode *parent;

	VocabNode(){}
	VocabNode(string word, int index, int count) {
		this->word = word;
		this->index = index;
		this->count = count;
		nodeCode = 0;
		parent = NULL;
	}

	VocabNode(int count) {
		this->word = "";
		this->index = -1;
		this->count = count;
		nodeCode = 0;
		parent = NULL;
	}

};

struct VocabNodeComparator {
	bool operator() (VocabNode *arg1, VocabNode *arg2) {
		return arg1->count > arg2->count;
	}
};


class SemanticNeuralNetwork {
public:
	const int maxIter = 500;
	const int hlsize = 200;//hidden layer size
	const int shiftSize = 2;
	const float alpha = 0.5;
    const float beta = 0.5;
	const int threadNum = 6;

	map<string, VocabNode*> vocabMap;
	// vocabulary map, word -> index,count
	vector<VocabNode*> vocabVec;

	int vocabSize = 0;
	int wordCount = 0;
	int trainingWordCount = 0;

	vector<vector<int>> wordIndexInSentence;

	float **WIH;
	//input -> hidden weight  | vocabSize ＊ hlsize
	//WIH[k][i] : weight of kth Input word to ith Hidden unit

	float ***WHO;
	//hidden -> output weight | hlsize * vocabSize
	//WHO[threadIndex][i][j] weight of ith Hidden unit to jth Output unit

	float **hidden; //hidden[threadIndex][hlsize]
	float **WIHe; //WIHe[threadIndex][hlsize]
    float **WIHe_pre; // monument

	void CreateHuffmanTree()
	{
		cout << "Create Huffman Tree" << endl;
		map<string, VocabNode*>::iterator iter;

		priority_queue <VocabNode*, vector<VocabNode*>, VocabNodeComparator> pq;
		for (iter = vocabMap.begin(); iter != vocabMap.end(); iter++) {
			pq.push(iter->second);
		}

		while (pq.size() > 1) {
			VocabNode* rightNode = pq.top();
			pq.pop();
			VocabNode* leftNode = pq.top();
			pq.pop();

			VocabNode* parent = new VocabNode(leftNode->count + rightNode->count);

			leftNode->nodeCode = 1;
			rightNode->nodeCode = 0;
			leftNode->parent = parent;
			rightNode->parent = parent;

			pq.push(parent);
		}

		for (iter = vocabMap.begin(); iter != vocabMap.end(); iter++) {
			VocabNode* vn = iter->second;
			// cout << iter->first << vn->count << "| node code:" ;
			while (vn->parent != NULL) {
				iter->second->codeArray.push_back(vn->nodeCode);
				//iter->second->codeArray.insert(iter->second->codeArray.begin(), vn->nodeCode);
				// cout << vn->nodeCode;
				vn = vn->parent;
			}
			// cout << endl;
		}
		 cout << " ------------- "<<endl;
		 for (size_t i = 0; i < vocabVec.size(); i++) {
		 	cout << vocabVec[i]->word << " count: " <<  vocabVec[i]->count << " Index: " << vocabVec[i]->index <<" | code: ";
		 	for (size_t j = 0; j < vocabVec[i]->codeArray.size(); j++) {
		 		cout << vocabVec[i]->codeArray[j];
		 	}
		 	cout << endl;
		 }

		cout << "Create Huffman Tree complete" << endl;
	}

	void Init(const vector<string> & sentenceArray) {
		cout << "start init" << endl;

		trainingWordCount = 0;
		vocabSize = 0;
		wordCount = 0;

		for (int i = 0; i < sentenceArray.size(); i++) {
			string sentence = sentenceArray[i];
			// cout << sentenceArray[i] << endl;
			vector<string> wordArray;
			UT_String::split(UT_String::trim(sentence), ' ', wordArray);
			if (wordArray.size() < 3) continue;

			vector<int> wordIndexArray;
			for (int j = 0; j < wordArray.size(); j++) {
				// cout << wordArray[j] << "|";
				string word = wordArray[j];
				if (vocabMap.find(word) == vocabMap.end()) {
					// pair<int, int> p(vocabSize, 1);
					VocabNode *vn = new VocabNode(word, vocabSize, 1);
					vocabMap[word] = vn;
					vocabVec.push_back(vn);
					vocabSize++;
				}
				else {
					vocabMap[word]->count++;
				}
				wordCount++;
				wordIndexArray.push_back(vocabMap[word]->index);
			}
			wordIndexInSentence.push_back(wordIndexArray);
		}

		CreateHuffmanTree();

		WIH = new float*[vocabSize];
		for (size_t i = 0; i < vocabSize; i++) {
			WIH[i] = new float[hlsize];
			for (size_t j = 0; j < hlsize; j++) {
				WIH[i][j] = UT_Math::RandFloat(-0.05, 0.05);
			}
		}

		WHO = new float **[threadNum];
		for (int tindex = 0; tindex < threadNum; tindex++)
		{
			WHO[tindex] = new float *[hlsize];
			for (size_t i = 0; i < hlsize; i++) {
				WHO[tindex][i] = new float[vocabSize];
				for (size_t j = 0; j < vocabSize; j++) {
					WHO[tindex][i][j] = UT_Math::RandFloat(-0.05, 0.05);
				}
			}
		}

		hidden = new float*[threadNum];
		WIHe = new float*[threadNum];
        WIHe_pre = new float*[threadNum];
		for (int i = 0; i < threadNum; i++) {
			hidden[i] = new float[hlsize];
			WIHe[i] = new float[hlsize];
            WIHe_pre[i] = new float[hlsize];
		}

		cout << "Init Complete" << endl;
		cout << "Vocab count:" << vocabSize << endl;
		cout << "Word count:" << wordCount << endl;

	}

	void TrainEach(int *inputWordsIndex, int inputSize, int outputWordIndex, int threadIndex) {
        int iterCount = 0;
        memset(WIHe_pre[threadIndex], 0, sizeof(WIHe_pre[0][0]) * hlsize);
        int outputLayerSize = vocabVec[outputWordIndex]->codeArray.size();

        while (1) {
            iterCount++;
			memset(hidden[threadIndex], 0, sizeof(hidden[0][0]) * hlsize);
			memset(WIHe[threadIndex], 0, sizeof(WIHe[0][0]) * hlsize);

			// input -> hidden
			for (int k = 0; k < inputSize; k++) {
				for (int i = 0; i < hlsize; i++) {
					hidden[threadIndex][i] = hidden[threadIndex][i] + (WIH[inputWordsIndex[k]][i]);
				}
			}
            
            for (int i = 0; i < hlsize; i++) {
                hidden[threadIndex][i] = hidden[threadIndex][i] / (float)inputSize;
            }

            float error = 0;

			for (int j = 0; j < outputLayerSize; j++) {
				float e = 0;
				float outputJ = 0;
				float target = vocabVec[outputWordIndex]->codeArray[j];

				for (int i = 0; i < hlsize; i++) {
					outputJ += hidden[threadIndex][i] * WHO[threadIndex][i][j];
				}


				outputJ = UT_Math::sigmoid(outputJ);
				e = alpha * (target - outputJ) * outputJ * (1 - outputJ);
//				e = alpha * (1- target - outputJ);
                error = error + abs((target - outputJ));

				// for learning weight of input to hidden
				for (int i = 0; i < hlsize; i++) {
					WIHe[threadIndex][i] = WIHe[threadIndex][i] + e * WHO[threadIndex][i][j];
				}

				// learn weight of hidden to output layer
				for (int i = 0; i < hlsize; i++) {
					WHO[threadIndex][i][j] = WHO[threadIndex][i][j] + e * hidden[threadIndex][i];
				}
			}


			//learn weight of input to hidden
			for (int k = 0; k < inputSize; k++) {
				for (int i = 0; i < hlsize; i++) {
					WIH[inputWordsIndex[k]][i] = WIH[inputWordsIndex[k]][i] + (WIHe[threadIndex][i]) + beta * (WIHe_pre[threadIndex][i]);
                    WIHe_pre[threadIndex][i] = WIHe[threadIndex][i];
				}
			}

            if((error / (outputLayerSize)) < 0.005 || iterCount > maxIter) {
//                cout<< "iter count: " << iterCount << endl;
                break;
            }
		}
	}

	void TrainingSentence(int sentenceIndex, int threadIndex)
	{
		for (int i = 0; i < wordIndexInSentence[sentenceIndex].size(); i++) {
			int outputIndex = wordIndexInSentence[sentenceIndex][i];

			//escape from not rare words
			if (vocabVec[outputIndex]->count < 3) continue;

			int inputArray[100];
			int inputCount = 0;
            
			for (int j = 1; ; j++) {
				int inputIndex;
				if (i - j > 0) {
					inputIndex = wordIndexInSentence[sentenceIndex][i - j];
					if (vocabVec[inputIndex]->count >= 3) inputArray[inputCount++] = inputIndex;
				}

				if (i + j < wordIndexInSentence[sentenceIndex].size()) {
					inputIndex = wordIndexInSentence[sentenceIndex][i + j];
					if (vocabVec[inputIndex]->count >= 3) inputArray[inputCount++] = inputIndex;
				}
                
                if (inputCount == shiftSize * 2 || !(i - j > 0 && i + j < wordIndexInSentence[sentenceIndex].size())) {
                    break;
                }

			}

            if(inputCount == 0) continue;

			TrainEach(inputArray, inputCount, outputIndex, threadIndex);
			trainingWordCount++;
			printf("\rProgress: %.3f%%", (float)trainingWordCount / (float)wordCount * 100.0);
			fflush(stdout);
		}
	}

	void TrainingThread(int threadIndex)
	{
		int eachTrainingSize = wordIndexInSentence.size() / threadNum;
		int beginIndex = threadIndex * eachTrainingSize;
		int endIndex = (threadIndex + 1) * eachTrainingSize;
		if (threadIndex == threadNum - 1) {
			endIndex += wordIndexInSentence.size() % threadNum;
		}

		for (int i = beginIndex; i < endIndex; i++) {
			TrainingSentence(i, threadIndex);
		}
	}

	void Train()
	{
		cout << "start training" << endl;
		vector<thread*> threadPool;
		for (int i = 0; i < threadNum; i++) {
			thread *t = new thread(&SemanticNeuralNetwork::TrainingThread, this, i);
			threadPool.push_back(t);
		}

		for (int i = 0; i < threadNum; i++) {
			threadPool[i]->join();
		}

		for (int i = 0; i < threadNum; i++) {
			delete threadPool[i];
		}
	}

	void Save(string path) {
		cout << "\nSaving" << endl;
		ofstream ofile(path);
		map<string, VocabNode*>::iterator iter;

		for (iter = vocabMap.begin(); iter != vocabMap.end(); iter++) {
			ofile << iter->first << " ";
			int wordIndex = iter->second->index;
			for (int i = 0; i < hlsize; i++) {
				ofile << WIH[wordIndex][i] << " ";
			}
			ofile << endl;
		}

		ofile.close();
		cout << "Save completed\n" << endl;
	}

	void TestVocabMap()
	{

		CreateHuffmanTree();
	}

};

bool IsBreakChar(char a)
{
	if (a < 'a' || a > 'z') {
		if (a != ' ' && a != '.' && a != '-') {
			return true;
		}
	}
	return false;
}

void TrainData(string path)
{
	std::ifstream t(path);
	std::stringstream buffer;
	buffer << t.rdbuf();
	string wholeContent = buffer.str();

	transform(wholeContent.begin(), wholeContent.end(), wholeContent.begin(), ::tolower);

	for (int i = 0; i < wholeContent.size(); i++) {
		if (IsBreakChar(wholeContent[i])) {
			wholeContent[i] = '.';
		}
	}
	//    cout << wholeContent << endl;

	vector<string> sentenceArray;
	UT_String::split(wholeContent, ".", sentenceArray);

	SemanticNeuralNetwork snn;
	snn.Init(sentenceArray);
	snn.Train();
	snn.Save(path + ".rep");

}


int main(int arg, char*argvs[]) {
    srand(time(0));
	string path(argvs[1]);
	cout << path << endl;
	TrainData(path);
}
