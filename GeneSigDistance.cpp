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

map<string, vector<float>> GetKmerSigDict(string path) {
	map<string, vector<float>> kmerSigDict;
	ifstream t(path);
	string line = "";
	string kmer = "";
	int kmerSigLen = 0;
	for (int i = 0; getline(t, line); i++) {
		UT_String::trim(line);
		if (i % 2 == 0) {
			// cout << line << endl;
			kmer = line;
		} else {
			vector<std::string> sigStrVec = UT_String::split(line, ' ');
			if (kmerSigLen == 0) {
				kmerSigLen = sigStrVec.size();
			}
			vector<float> sigVec;
			for (size_t i = 0; i < sigStrVec.size(); i++) {
				sigVec.push_back(stof(sigStrVec[i]));
			}
			kmerSigDict[kmer] = sigVec;
			// for (size_t i = 0; i < sigVec.size(); i++) {
			//     cout << sigVec[i] << " ";
			// }
		}
		// if(i%2==1)
		// break;
	}
	t.close();
	cout << "kmer num:" << endl;
	cout << "kmer sig lenth:" << kmerSigLen << endl;
	cout << "kmer size: " << kmerSigDict.begin()->first.size() << endl;
	return kmerSigDict;
}

map<string, vector<float>> GetGeneSigDict(string path, map<string, vector<float>> &kmerSigDict) {
	map<string, vector<float>> geneSigDict;
	ifstream t(path);
	string line = "";
	string geneName = "";
	int kmerSize = kmerSigDict.begin()->first.size();
	int sigSize = kmerSigDict.begin()->second.size();
	for (int i = 0; getline(t, line); i++) {
		UT_String::trim(line);
		if (line[0] == '>') {
			geneName = line.substr(1);
			if ((i) % 1000 == 0) {
				cout << "finish gene sig generation num: " << i / 2 << endl;
			}
		} else {
			vector<string> geneKmers = UT_GenomeKit::GetKmers(line, kmerSize);
			vector<float> geneSig(sigSize, 0.0);
			// cout << geneKmers.size() << endl;
			for (int ki = 0; ki < geneKmers.size(); ki++) {
				vector<float> kmerSig = kmerSigDict[geneKmers[ki]];
				for (int j = 0; j < sigSize; j++) {
					geneSig[j] += kmerSig[j];
				}
			}

			geneSigDict[geneName] = geneSig;
			// for (int j = 0 ; j < sigSize ; j++) {
			//     cout << geneSig[j] << " ";
			// }
		}
		//
		// if(i%2==1)
		// break;
	}
	cout << "Finish Gene Sig Calcualtion!" << endl;
	return geneSigDict;
}

void TopGeneSimilarityScore(map<string, vector<float>> &geneSigDict, int topKnum = 10, string outputPath = "output_trec_eval") {
	map<string, vector<pair<string, float>>> genePairScore;
	map<string, vector<float>>::iterator iter;
	map<string, vector<float>>::iterator iter2;

    int count = 0;
	for (iter = geneSigDict.begin(); iter != geneSigDict.end(); iter++) {
		vector<pair<string, float>> geneScoreVec(topKnum, pair<string, float>("", 0.0));
		for (iter2 = geneSigDict.begin(); iter2 != geneSigDict.end(); iter2++) {
			float score = UT_Math::CosineSimilarity(iter->second, iter2->second);
			// cout << iter->first << " - " << iter2->first << " : " << score << endl;
			pair<string, float> geneScore(iter2->first, score);
			for (int i = topKnum - 1; i >= 0; i--) {
				if (geneScoreVec[i].second < score) {
					if (i == topKnum - 1) {
						geneScoreVec[i] = geneScore;
					} else {
						geneScoreVec[i + 1] = geneScoreVec[i];
						geneScoreVec[i] = geneScore;
					}
				} else {
					break;
				}
			}
		}
		genePairScore[iter->first] = geneScoreVec;
        count++;
        if(count % 1000 == 0) {
            cout << "finish similarity score calculation: " << count << endl;
        }
	}

	// output as trec eval file
	/*
	'''
	output:
	301	Q0	FR940202-2-00150	104	  2.129133	STANDARD
	301	Q0	FR940202-2-00151	414	  1.724760	STANDARD
	301	Q0	FR940202-2-00154	124	  2.104024	STANDARD
	301	Q0	FR940203-1-00036	233	  1.912871	STANDARD
	301	Q0	FR940203-1-00038	326	  1.800881	STANDARD
	301	Q0	FR940203-1-00039	273	  1.870957	STANDARD
	'''

	def output(keyi, keyj, score, f):
	    output_str = "%s Q0 %s 0 %d STANDARD\n" % (keyi, keyj, score)
	    f.write(output_str)

	*/

    cout << "saving trec_eval_output" << endl;
	ofstream ofile(outputPath);
	map<string, vector<pair<string, float>>>::iterator iterGPS;
	for (iterGPS = genePairScore.begin(); iterGPS != genePairScore.end(); iterGPS++) {
		// cout << iterGPS->first << " - " << endl;
		vector<pair<string, float>> geneScoreVec = iterGPS->second;
		for (size_t i = 0; i < geneScoreVec.size(); i++) {
			// cout << geneScoreVec[i].first << " : " << geneScoreVec[i].second << endl;
			if (geneScoreVec[i].first != "") {
				char outputstr[1024];
				sprintf(outputstr, "%s Q0 %s 0 %f STANDARD\n", iterGPS->first.c_str(), geneScoreVec[i].first.c_str(), geneScoreVec[i].second);
				ofile << string(outputstr);
			}
		}
	}
	ofile.close();
}

int main() {
	string sigPath = "../TestData3_test89/3mer-semantic-sig-128";
	string genePath = "../TestData3_test89/genes.faa";
	string outputPath = sigPath + "_trec_eval";
	map<string, vector<float>> kmerSigDict = GetKmerSigDict(sigPath);
	map<string, vector<float>> geneSigDict = GetGeneSigDict(genePath, kmerSigDict);
	TopGeneSimilarityScore(geneSigDict, 10, outputPath);
}
