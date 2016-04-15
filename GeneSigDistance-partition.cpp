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

using namespace std;

class GeneSigDistance {
       public:
	int kmerNum;
	int kmerLen = 5;
	int kmerSigLen = 512;
	int geneNum;

	vector<string> kmers;
	vector<vector<float>> kmerSigs;
	map<string, vector<float>> kmerSigDict;

	vector<string> genes;
	vector<string> geneSequences;
	vector<vector<float>> geneSigs;
	map<string, vector<float>> geneSigDict;

	vector<vector<vector<float>>> genePartSigs;  // genePartSigs[geneIndex][PartIndex][sig]
	vector<vector<string>> geneParts;
	map<string, vector<vector<float>>> genePartSigDict;

	float **pairDistScore;  // higher when closer

	int **topDistGeneIndex;
	float **topDistScore;

	int threadNum = 8;
	int partLen = 32;
	int breakLen = 10;
	int retrivalNum = 500;

	int finishCount = 0;
	// functions

	void Init(int threadNum, int partLen, int breakLen, int retrivalNum) {
		this->threadNum = threadNum;
		this->partLen = partLen;
		this->breakLen = breakLen;
		this->retrivalNum = retrivalNum;
		cout << "threadNum:" << threadNum << "|partLen:" << partLen << "|breakLen:" << breakLen << "|retrivalNum:" << retrivalNum << endl;
	}

	float GetScoreFromDist(float distance) { return (1.0 / (1.0 + (distance))) * 1000000; }

	void GetSigs(string path, vector<string> &labels, vector<vector<float>> &sigs, map<string, vector<float>> &dict) {
		std::ifstream t(path);
		std::stringstream buffer;
		buffer << t.rdbuf();
		string line = "";
		string label = "";
		for (int i = 0; getline(buffer, line); i++) {
			UT_String::trim(line);
			if (i % 2 == 0) {
				label = line;
			} else {
				vector<std::string> sigStrVec = UT_String::split(line, ' ');
				vector<float> sigVec;
				for (size_t i = 0; i < sigStrVec.size(); i++) {
					sigVec.push_back(stof(sigStrVec[i]));
				}
				labels.push_back(label);
				sigs.push_back(sigVec);
				dict[label] = sigVec;
			}
		}
	}

	void GetKmerSigs(string path) {
		GetSigs(path, kmers, kmerSigs, kmerSigDict);
		kmerNum = kmers.size();
		kmerLen = kmers[0].size();
		kmerSigLen = kmerSigs[0].size();
		cout << "kmer num: " << kmers.size() << endl;
		cout << "kmer len: " << kmerLen << endl;
		cout << "sig len: " << kmerSigs[0].size() << endl;
	}

	void GenePartitionSig(const string &gene, int geneIndex) {
		// cout << gene << endl;

		vector<vector<float>> PartSigs;
		vector<string> genePartVec;
		for (int i = 0; i < gene.size(); i += breakLen) {
			string genePart;
			if (partLen >= gene.size()) {
				genePart = gene;
			} else {
				if (i + partLen >= gene.size()) {
					genePart = gene.substr(gene.size() - partLen, partLen);
				} else {
					genePart = gene.substr(i, partLen);
				}
			}

			vector<string> geneKmers = UT_GenomeKit::GetKmers(genePart, kmerLen);
			vector<float> PartSig(kmerSigLen, 0.0);

			for (int ki = 0; ki < geneKmers.size(); ki++) {
				// cout << geneKmers[ki] << endl;
				vector<float> kmerSig = kmerSigDict[geneKmers[ki]];
				for (int j = 0; j < kmerSigLen; j++) {
					PartSig[j] += kmerSig[j];
				}
			}
			PartSigs.push_back(PartSig);
			genePartVec.push_back(genePart);
			if (i + partLen >= gene.size()) {
				break;
			}
		}
		genePartSigs[geneIndex] = PartSigs;
		geneParts[geneIndex] = genePartVec;
		genePartSigDict[gene] = PartSigs;

		// cout << "geneSigs[geneIndex].size(): " << genePartSigs[geneIndex].size() << endl;
	}

	void CalculateGenePartitionSigThread(int threadIndex) {
		// cout << "CalculateGeneSigsThread" << endl;
		int fragSize = int(geneNum / threadNum);
		int startIndex = fragSize * threadIndex;
		int endIndex = startIndex + fragSize - 1;
		if (threadIndex == threadNum - 1) {
			endIndex = geneNum - 1;
		}
		// cout << startIndex << "|" << endIndex << endl;
		for (int i = startIndex; i <= endIndex; i++) {
			GenePartitionSig(geneSequences[i], i);
			finishCount++;
			printf("\rProgress: %.3f%%", (float)finishCount / (float)geneNum * 100.0);
		}
	}

	void GenerateGeneSigs(string path) {
		std::ifstream t(path);
		std::stringstream buffer;
		buffer << t.rdbuf();
		string geneName;
		string line;
		for (int i = 0; getline(buffer, line); i++) {
			UT_String::trim(line);
			// cout << line << endl;
			if (line[0] == '>') {
				geneName = line.substr(1, line.find(',') - 1);

				// transfer G1_SE015 -> >S015/00001
				char name[100];
				string first = geneName.substr(geneName.find("SE") + 2);
				int second = stoi(geneName.substr(1, geneName.find('_') - 1));
				sprintf(name, "S%s/%05d", first.c_str(), second);
				geneName = string(name);

				genes.push_back(geneName);
			} else {
				geneSequences.push_back(line);
			}
		}
		geneNum = genes.size();

		geneSigs = vector<vector<float>>(genes.size());
		genePartSigs = vector<vector<vector<float>>>(genes.size());
		geneParts = vector<vector<string>>(genes.size());
		finishCount = 0;
		vector<thread *> threadPool;
		for (int i = 0; i < threadNum; i++) {
			thread *t = new thread(&GeneSigDistance::CalculateGenePartitionSigThread, this, i);
			threadPool.push_back(t);
		}

		for (int i = 0; i < threadNum; i++) {
			threadPool[i]->join();
		}

		for (int i = 0; i < threadNum; i++) {
			delete threadPool[i];
		}
	}

	void SaveGeneSigs(string savePath) {
		cout << "Saving" << endl;
		ofstream ofile(savePath);
		map<string, vector<float>>::iterator iter;
		for (iter = geneSigDict.begin(); iter != geneSigDict.end(); iter++) {
			ofile << iter->first << endl;
			for (size_t i = 0; i < iter->second.size(); i++) {
				ofile << iter->second[i] << " ";
			}
			ofile << endl;
		}
		ofile.close();
	}

	void GetGeneSigs(string genePath, string sigPath) {
		GenerateGeneSigs(genePath);
		geneNum = genes.size();
		cout << "\nGenes num: " << geneNum << endl;
	}

	bool IsInArray(int array[], int size, int a) {
		for (size_t i = 0; i < size; i++) {
			if (array[i] == a) {
				return true;
			}
		}
		return false;
	}

	float CalculateTopGenePartScore(int geneA, int geneB, int topBestK) {
		float avgScore = 0.0;
		float scoreArr[64] = {0};
		int scorePartIndexA[64] = {0};
		int scorePartIndexB[64] = {0};

		for (int i = 0; i < genePartSigs[geneA].size(); i++) {
			for (int j = 0; j < genePartSigs[geneB].size(); j++) {
				float score = UT_Math::TMPDis(genePartSigs[geneA][i], genePartSigs[geneB][j]);
				score = GetScoreFromDist(score);
				for (int k = topBestK - 1; k >= 0; k--) {
					if (scoreArr[k] > score) {
						break;
					}
					scoreArr[k + 1] = scoreArr[k];
					scoreArr[k] = score;

					scorePartIndexA[k + 1] = scorePartIndexA[k];
					scorePartIndexA[k] = i;

					scorePartIndexB[k + 1] = scorePartIndexB[k];
					scorePartIndexB[k] = j;
				}
			}
		}

		// for (size_t i = 0; i < topBestK; i++) {
		// 	for (size_t j = i + 1; j < topBestK; j++) {
		// 		if (scorePartIndexA[i] == scorePartIndexA[j]) scorePartIndexA[j] = -1;
		// 		if (scorePartIndexB[i] == scorePartIndexB[j]) scorePartIndexB[j] = -1;
		// 	}
		// }
		int count = 0;
		for (size_t i = 0; i < topBestK; i++) {
			if (scorePartIndexA[i] != -1 && scorePartIndexB[i] != -1) {
				avgScore += scoreArr[i];
				count++;
			}
		}

		// debug
		// cout << "GA:" << geneA << "|GB:" << geneB << endl;
		// cout << "score:" << avgScore / topBestK << endl;
		//
		// for (size_t i = 0; i < topBestK; i++) {
		// 	cout << "A:" << geneParts[geneA][scorePartIndexA[i]] << endl;
		// 	cout << "B:" << geneParts[geneB][scorePartIndexB[i]] << endl;
		// }

        int lenDiffW = abs((long)(genePartSigs[geneA].size() - genePartSigs[geneB].size())) * 2000;

        float result = avgScore / count - lenDiffW;
        result = result <= 0.0? 1.0: result;

		return result;
	}

	void CalculateGenePartScore(int geneA, int geneB) {
		float score = 0.0;
		if (geneA == geneB) {
			score = GetScoreFromDist(0.0);
			pairDistScore[geneA][geneB] = score;
		} else {
			if (pairDistScore[geneA][geneB] != 0.0 || pairDistScore[geneB][geneA] != 0.0) {
				score = pairDistScore[geneA][geneB] != 0.0 ? pairDistScore[geneA][geneB] : pairDistScore[geneB][geneA];
			} else {
				score = CalculateTopGenePartScore(geneA, geneB, 5);
				pairDistScore[geneA][geneB] = score;
				pairDistScore[geneB][geneA] = score;
			}
		}
		// cout << "insert : " << geneA << " " << geneB << " " << score << endl;
		InsertTop(geneA, geneB, score);
	}

	void CalculateGenePartDistScoreThread(int threadIndex) {
		float dist = 0.0;
		int fragSize = int(geneNum / threadNum);
		int startIndex = fragSize * threadIndex;
		int endIndex = startIndex + fragSize - 1;
		if (threadIndex == threadNum - 1) {
			endIndex = geneNum - 1;
		}

		// cout << "test" << endl;

		for (int i = startIndex; i <= endIndex; i++) {
			for (int j = 0; j < geneNum; j++) {
				CalculateGenePartScore(i, j);
			}
			finishCount++;
			printf("\rProgress: %.3f%%", (float)finishCount / (float)geneNum * 100.0);
			// if(finishCount % 10 == 0) {
			// cout << "finish count:" << finishCount << endl;
			// }
			// break;
		}
	}

	void CalculateGenePairDistScore() {
		cout << "CalculateGenePairDistScore" << endl;

		topDistGeneIndex = new int *[geneNum];
		topDistScore = new float *[geneNum];
		for (size_t i = 0; i < geneNum; i++) {
			topDistGeneIndex[i] = new int[retrivalNum];
			topDistScore[i] = new float[retrivalNum];
			memset(topDistGeneIndex[i], 0, sizeof(topDistGeneIndex[0][0]) * retrivalNum);
			memset(topDistScore[i], 0.0, sizeof(topDistScore[0][0]) * retrivalNum);
		}

		pairDistScore = new float *[geneNum];
		for (size_t i = 0; i < geneNum; i++) {
			pairDistScore[i] = new float[geneNum];
			memset(pairDistScore[i], 0.0, sizeof(pairDistScore[0][0]) * geneNum);
		}

		finishCount = 0;
		vector<thread *> threadPool;
		for (int i = 0; i < threadNum; i++) {
			thread *t = new thread(&GeneSigDistance::CalculateGenePartDistScoreThread, this, i);
			threadPool.push_back(t);
		}

		for (int i = 0; i < threadNum; i++) {
			threadPool[i]->join();
		}

		for (int i = 0; i < threadNum; i++) {
			delete threadPool[i];
		}

		cout << "\nCalculateGenePairDistScore finish" << endl;
	}

	void SaveTrecEval(string path) {
		cout << "saving trec_eval_output" << endl;
		ofstream ofile(path);
		for (size_t i = 0; i < geneNum; i++) {
			for (size_t j = 0; j < retrivalNum; j++) {
				char outputstr[1024];
				sprintf(outputstr, "%s Q0 %s 0 %f STANDARD\n", genes[i].c_str(), genes[topDistGeneIndex[i][j]].c_str(), topDistScore[i][j]);
				ofile << string(outputstr);
			}
		}
		ofile.close();
	}

	void InsertTop(int geneIndex, int geneIndexTo, float score) {
		// cout << "retrivalNum: " << retrivalNum << endl;

		for (int i = retrivalNum - 1; i >= 0; i--) {
			if (topDistScore[geneIndex][i] < score) {
				if (i < retrivalNum - 1) {
					topDistScore[geneIndex][i + 1] = topDistScore[geneIndex][i];
					topDistGeneIndex[geneIndex][i + 1] = topDistGeneIndex[geneIndex][i];
				}
				topDistScore[geneIndex][i] = score;
				topDistGeneIndex[geneIndex][i] = geneIndexTo;
			} else {
				break;
			}
		}
	}
};

int main(int arg, char *argvs[]) {
	// string kmerSigPath = "../TestData3_test89/3mer-semantic-sig-128";
	// string geneSigPath = "../TestData3_test89/3mer-semantic-sig-128-gene-sig";
	// string genePath = "../TestData3_test89/genes_100.faa";
	//
	// string kmerSigPath = "../TestData2/sig/5mer_semantic_sig_256_all_dna_new_all";
	// string geneSigPath = "../TestData2/sig/5mer_semantic_sig_256_all_dna_new_all_gene_sig";
	// string genePath = "../TestData2/alfsim-data2/DB/all_dna.fa";

	string kmerSigPath = string(argvs[1]);
	string genePath = string(argvs[2]);
	string threadNumStr = string(argvs[3]);
	string geneSigPath = kmerSigPath + "_gene_sig";

	int threadNum = stoi(threadNumStr);
	cout << "Thread Num: " << threadNum << endl;

	GeneSigDistance gsd;
	gsd.Init(threadNum, 20, 5, 500);
	gsd.GetKmerSigs(kmerSigPath);
	gsd.GetGeneSigs(genePath, geneSigPath);
	gsd.CalculateGenePairDistScore();
	gsd.SaveTrecEval(geneSigPath + "_trec_result");
}
