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

#include "Util.hpp"
using namespace std;

class SemanticNeuralNetwork {
public:
	const int iter = 25;
	const int hlsize = 200;//hidden layer size
	const int shiftSize = 5;
	const float alpha = 0.1;
	const int threadNum = 8;

	map<string, pair<int, int>> vocabMap;
	// vocabulary map, word -> index,count
	int vocabSize = 0;
    int wordCount = 0;
    int trainingWordCount = 0;

//	vector<int> contextsIndexVec;
    vector<vector<int>> wordIndexInSentence;

	float **WIH;
	//input -> hidden weight  | vocabSize ＊ hlsize
	//WIH[k][i] : weight of kth Input word to ith Hidden unit

	float ***WHO;
	//hidden -> output weight | hlsize * vocabSize
	//WHO[threadIndex][i][j] weight of ith Hidden unit to jth Output unit


	float **hidden; //hidden[threadIndex][hlsize]
	float **WIHe; //WIHe[threadIndex][hlsize]



    
    void Init(const vector<string> & sentenceArray) {
        trainingWordCount = 0;
        vocabSize = 0;
        wordCount = 0;

        for (int i = 0; i < sentenceArray.size(); i++) {
            
            string sentence = sentenceArray[i];
//            cout << sentenceArray[i] << endl;
            vector<string> wordArray;
            UT_String::split(UT_String::trim(sentence), ' ', wordArray);
            
            if(wordArray.size() < 3) continue;
            
            vector<int> wordIndexArray;
            for (int j = 0; j < wordArray.size(); j++) {
//                cout << wordArray[j] << "|";
                string word = wordArray[j];
                if (vocabMap.find(word) == vocabMap.end()) {
                    pair<int, int> p(vocabSize, 1);
                    vocabMap[word] = p;
                    vocabSize++;
                } else {
                    vocabMap[word].second++;
                }
                wordCount++;
                wordIndexArray.push_back(vocabMap[word].first);
            }
            
            wordIndexInSentence.push_back(wordIndexArray);
            
        }
        
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
        for (int i = 0; i < threadNum; i++) {
            hidden[i] = new float[hlsize];
            WIHe[i] = new float[hlsize];
        }

        cout << "Init Complete" << endl;
        cout << "Vocab count:" << vocabSize << endl;
        cout << "Word count:" << wordCount << endl;
        
    }

	void TrainEach(int *inputWordsIndex, int inputSize, int outputWordIndex, int threadIndex) {

//		int inputSize = inputWordsIndex.size();
		int localIter = iter;

		while (localIter-- != 0) {
			memset(hidden[threadIndex], 0, sizeof(hidden[0][0]) * hlsize);
			memset(WIHe[threadIndex], 0, sizeof(WIHe[0][0]) * hlsize);

			// input -> hidden
			for (int k = 0; k < inputSize; k++) {
				for (int i = 0; i < hlsize; i++) {
					hidden[threadIndex][i] = hidden[threadIndex][i] + WIH[inputWordsIndex[k]][i];
				}
			}

			for (int j = 0; j < vocabSize; j++) {
				float e = 0;
				float outputJ = 0;
				float target = (j == outputWordIndex) ? 1.0 : 0;

				for (int i = 0; i < hlsize; i++) {
					outputJ += hidden[threadIndex][i] * WHO[threadIndex][i][j];
				}

				outputJ = UT_Math::sigmoid(outputJ);
				e = alpha * (target - outputJ) * outputJ * (1 - outputJ);

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
					WIH[inputWordsIndex[k]][i] = WIH[inputWordsIndex[k]][i] + WIHe[threadIndex][i];
				}
			}
		}
	}
    
    void TrainingSentence(int sentenceIndex, int threadIndex)
    {
        
        for (int i = 0; i < wordIndexInSentence[sentenceIndex].size(); i++) {
            int outputIndex = wordIndexInSentence[sentenceIndex][i];
            
            int inputArray[100];
            int inputCount = 0;
            for (int j = 1; j <= shiftSize; j++) {
                if (i - j > 0)
                    inputArray[inputCount++] = (wordIndexInSentence[sentenceIndex][i - j]);
                if (i + j < wordIndexInSentence[sentenceIndex].size())
                    inputArray[inputCount++] = (wordIndexInSentence[sentenceIndex][i + j]);
            }
            
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
            TrainingSentence(i,threadIndex);
        }
    }

	void Train()
	{
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
        cout << "Saving" << endl;
		ofstream ofile(path);
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
		cout << "\nSave completed\n" << endl;
	}
    
    void TestVocabMap()
    {
        map<string, pair<int, int>>::iterator iter;
        
        for (iter = vocabMap.begin(); iter != vocabMap.end(); iter++) {
            cout << iter -> first << " | Index:" << iter -> second.first << " | Count:" << iter -> second.second << endl;
        }
    }

};

//void ReformatString(string & line)
//{
//	transform(line.begin(), line.end(), line.begin(), ::tolower);
//	for (int i = 0; i < line.size(); i++)
//	{
//		if (line[i] <'a' || line[i] > 'z') {
//			line[i] = ' ';
//		}
//	}
//}
//
//void TestSNN(string path) {
//	srand(time(0));
//
//	ifstream infile(path);
//	vector<string> contexts;
//	for (string line; getline(infile, line);) {
//		ReformatString(line);
//		if (!line.empty())
//			UT_String::split(line, " ", contexts);
//	}
//
//	cout << "word size:" << contexts.size() << endl;
//
//	SemanticNeuralNetwork snn;
//	snn.Init(contexts);
//	snn.Train();
//	snn.Save(path+".rep");
//}

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
    snn.TestVocabMap();
//    snn.Train();
//    snn.Save(path+".rep");
    
}


int main(int arg, char*argvs[]) {
//	string path(argvs[1]);
//	cout << path << endl;
//    TrainData(path);
    TrainData("TrainData");
//	getchar();
}
