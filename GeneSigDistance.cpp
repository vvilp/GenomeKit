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
	int kmerLen;
	int kmerSigLen;
	int geneNum;

	vector<string> kmers;
	vector<vector<float>> kmerSigs;
	map<string, vector<float>> kmerSigDict;

	vector<string> genes;
	vector<vector<float>> geneSigs;
	map<string, vector<float>> geneSigDict;

	float **pairDistScore;  // higher when closer

	int **topDistGeneIndex;
	float **topDistScore;
	int topk;

	int finishCount = 0;

	// functions
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

				if ((i) % 2000 == 0 && i != 0) {
					cout << "Finish gene sig generation num: " << i / 2 << endl;
				}
			} else {
				vector<string> geneKmers = UT_GenomeKit::GetKmers(line, kmerLen);
				vector<float> geneSig(kmerSigLen, 0.0);
				for (int ki = 0; ki < geneKmers.size(); ki++) {
					vector<float> kmerSig = kmerSigDict[geneKmers[ki]];
					for (int j = 0; j < kmerSigLen; j++) {
						geneSig[j] += kmerSig[j];
					}
				}
				genes.push_back(geneName);
				geneSigs.push_back(geneSig);
				geneSigDict[geneName] = geneSig;
			}
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
		if (UT_File::IsFileExist(sigPath)) {
			cout << "Gene sig exist. Read from file" << endl;
			GetSigs(sigPath, genes, geneSigs, geneSigDict);

		} else {
			cout << "Gene sig does not exist. Generating" << endl;
			GenerateGeneSigs(genePath);
			SaveGeneSigs(sigPath);
		}
		geneNum = genes.size();
		cout << "Genes num: " << geneNum << endl;
	}

	void CalculatGeneDistScoreThread(int threadIndex, int startIndex, int endIndex) {
		// cout << "startIndex : " << startIndex << "endIndex : " << endIndex << endl;
		float dist = 0.0;
		for (size_t i = startIndex; i <= endIndex; i++) {
			for (size_t j = i; j < geneNum; j++) {
				if (i == j) {
					dist = 0;
				} else {
					dist = UT_Math::TMPDis(geneSigs[i], geneSigs[j]);
				}
				pairDistScore[i][j] = (1.0 / (1.0 + (dist))) * 1000000;
				pairDistScore[j][i] = pairDistScore[i][j];
			}
			finishCount++;
			printf("\rProgress: %.3f%%", (float)finishCount / (float)geneNum * 100.0);
		}
	}

	void CalculateGenePairDistScore(int threadNum = 8) {
		pairDistScore = new float *[geneNum];
		for (size_t i = 0; i < geneNum; i++) {
			pairDistScore[i] = new float[geneNum];
		}

		// divide into different group for threads
		int startIndex = 0;
		int endIndex = 0;
		int groupCount = (int)(geneNum * geneNum / 2 / threadNum);
		cout << "thread groupCount: " << groupCount << endl;

		vector<thread *> threadPool;

		for (int i = 0; i < threadNum; i++) {
			startIndex = endIndex == 0 ? 0 : endIndex + 1;

			if (i != threadNum - 1) {
				int gapNum = geneNum - startIndex;
				endIndex = geneNum - (int)sqrt(gapNum * gapNum - groupCount * 2);
			} else {
				endIndex = geneNum - 1;
			}
			cout << "thread id:" << i << endl;
			cout << "startIndex: " << startIndex << endl;
			cout << "endIndex: " << endIndex << endl;
			thread *t = new thread(&GeneSigDistance::CalculatGeneDistScoreThread, this, i, startIndex, endIndex);
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
			for (size_t j = 0; j < topk; j++) {
				char outputstr[1024];
				sprintf(outputstr, "%s Q0 %s 0 %f STANDARD\n", genes[i].c_str(), genes[topDistGeneIndex[i][j]].c_str(), topDistScore[i][j]);
				ofile << string(outputstr);
			}
		}
		ofile.close();
	}

	void InsertTop(int geneIndex, int geneIndexTo, float score) {
		// cout << "topk: " << topk << endl;

		for (int i = topk - 1; i >= 0; i--) {
			if (topDistScore[geneIndex][i] < score) {
				if (i < topk - 1) {
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

	void RankTopGenePairDistScore(int topk = 200) {
		cout << "\nRankTopGenePairDistScore" << endl;
		this->topk = topk;

		topDistGeneIndex = new int *[geneNum];
		topDistScore = new float *[geneNum];
		for (size_t i = 0; i < geneNum; i++) {
			topDistGeneIndex[i] = new int[topk];
			topDistScore[i] = new float[topk];
			memset(topDistGeneIndex[i], 0, sizeof(topDistGeneIndex[0][0]) * topk);
			memset(topDistScore[i], 0.0, sizeof(topDistScore[0][0]) * topk);
		}

		for (size_t i = 0; i < geneNum; i++) {
			for (size_t j = i; j < geneNum; j++) {
				InsertTop(i, j, pairDistScore[i][j]);
				if (i != j) {
					InsertTop(j, i, pairDistScore[i][j]);
				}
			}
			printf("\rProgress: %.3f%%", (float)i / (float)geneNum * 100.0);
		}
		cout << "finish" << endl;
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
	string geneSigPath = kmerSigPath + "_gene_sig";

	GeneSigDistance gsd;
	gsd.GetKmerSigs(kmerSigPath);
	gsd.GetGeneSigs(genePath, geneSigPath);
	gsd.CalculateGenePairDistScore(8);
	gsd.RankTopGenePairDistScore(400);
	gsd.SaveTrecEval(geneSigPath + "_trec_result");
}
