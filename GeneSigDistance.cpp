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

map<string, vector<float>> GetSigDict(string path) {
	map<string, vector<float>> SigDict;
	ifstream t(path);
	string line = "";
	string kmer = "";
	for (int i = 0; getline(t, line); i++) {
		UT_String::trim(line);
		if (i % 2 == 0) {
			// cout << line << endl;
			kmer = line;
		} else {
			vector<std::string> sigStrVec = UT_String::split(line, ' ');
			vector<float> sigVec;
			for (size_t i = 0; i < sigStrVec.size(); i++) {
				sigVec.push_back(stof(sigStrVec[i]));
			}
			SigDict[kmer] = sigVec;
			// for (size_t i = 0; i < sigVec.size(); i++) {
			//     cout << sigVec[i] << " ";
			// }
		}
		// if(i%2==1)
		// break;
	}
	t.close();
	cout << "kmer num:" << endl;
	return SigDict;
}

map<string, vector<float>> GenerateGeneSigDict(string path, map<string, vector<float>> &kmerSigDict, string outputPath) {
	map<string, vector<float>> geneSigDict;
	ifstream t(path);
	string line = "";
	string geneName = "";
	int kmerSize = kmerSigDict.begin()->first.size();
	int sigSize = kmerSigDict.begin()->second.size();
	for (int i = 0; getline(t, line); i++) {
		UT_String::trim(line);
		if (line[0] == '>') {
			geneName = line.substr(1, line.find(',') - 1);

			// transfer G1_SE015 -> >S015/00001
			// char name[100];
			// string first = geneName.substr(geneName.find("SE") + 2);
			// int second = stoi(geneName.substr(1, geneName.find('_') - 1));
			// sprintf(name, "S%s/%05d", first.c_str(), second);
			// geneName = string(name);

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

	cout << "Saving" << endl;
	ofstream ofile(outputPath);
	map<string, vector<float>>::iterator iter;
	for (iter = geneSigDict.begin(); iter != geneSigDict.end(); iter++) {
		ofile << iter->first << endl;
		for (size_t i = 0; i < iter->second.size(); i++) {
			ofile << iter->second[i] << " ";
		}
		ofile << endl;
	}
	ofile.close();
	return geneSigDict;
}

void InsertToVector(vector<pair<string, float>> &geneScoreVec, pair<string, float> geneScore) {
	for (int i = geneScoreVec.size() - 1; i >= 0; i--) {
		if (geneScoreVec[i].second < geneScore.second) {
			if (i == geneScoreVec.size() - 1) {
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

void TopGeneSimilarityScore(map<string, vector<float>> &geneSigDict, int topKnum = 10, string outputPath = "output_trec_eval") {
	cout << "top gene similarity score calculating..." << endl;
	map<string, vector<pair<string, float>> *> genePairScore;
	map<string, vector<float>>::iterator iter;
	map<string, vector<float>>::iterator iter2;

	int count = 0;
	for (iter = geneSigDict.begin(); iter != geneSigDict.end(); iter++) {
		vector<pair<string, float>> *geneScoreVecAB;
		if (genePairScore.find(iter->first) != genePairScore.end()) {
			geneScoreVecAB = genePairScore[iter->first];
		} else {
			geneScoreVecAB = new vector<pair<string, float>>(topKnum, pair<string, float>("", 0.0));
			genePairScore[iter->first] = geneScoreVecAB;
		}
		for (iter2 = iter; iter2 != geneSigDict.end(); iter2++) {
			vector<pair<string, float>> *geneScoreVecBA;
			if (genePairScore.find(iter2->first) != genePairScore.end()) {
				geneScoreVecBA = genePairScore[iter2->first];
			} else {
				geneScoreVecBA = new vector<pair<string, float>>(topKnum, pair<string, float>("", 0.0));
				genePairScore[iter2->first] = geneScoreVecBA;
			}

			float score = UT_Math::TMPDis(iter->second, iter2->second);
			score = (1.0 / (1.0 + (score))) * 1000000;

			// A - B
			pair<string, float> geneScore1(iter2->first, score);
			InsertToVector(*geneScoreVecAB, geneScore1);

			if (iter != iter2) {
				// B - A
				pair<string, float> geneScore2(iter->first, score);
				InsertToVector(*geneScoreVecBA, geneScore2);
			}
		}

		count++;
		if (count % 200 == 0) {
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
	map<string, vector<pair<string, float>> *>::iterator iterGPS;
	for (iterGPS = genePairScore.begin(); iterGPS != genePairScore.end(); iterGPS++) {
		// cout << iterGPS->first << " - " << endl;
		vector<pair<string, float>> *geneScoreVec = iterGPS->second;
		for (size_t i = 0; i < (*geneScoreVec).size(); i++) {
			// cout << geneScoreVec[i].first << " : " << geneScoreVec[i].second << endl;
			if ((*geneScoreVec)[i].first != "") {
				char outputstr[1024];
				sprintf(outputstr, "%s Q0 %s 0 %f STANDARD\n", iterGPS->first.c_str(), (*geneScoreVec)[i].first.c_str(), (*geneScoreVec)[i].second);
				ofile << string(outputstr);
			}
		}
	}
	ofile.close();
}

int main(int arg, char *argvs[]) {

	// string kmerSigPath = "../TestData3_test89/3mer-semantic-sig-128";
	// string geneSigPath = "../TestData3_test89/3mer-semantic-sig-128-gene-sig";
	// string genePath = "../TestData3_test89/genes_100.faa";

	// string kmerSigPath = "../TestData2/sig/5mer_semantic_sig_256_all_dna_new_all";
	// string geneSigPath = "../TestData2/sig/5mer_semantic_sig_256_all_dna_new_all-gene_sig";
	// string genePath = "../TestData2/alfsim-data2/DB/all_dna.fa";

	// argvs[1] sig file
	// argvs[2] gene file

	string kmerSigPath = string(argvs[1]);
	string genePath = string(argvs[2]);
	string geneSigPath = kmerSigPath + "_gene_sig";

	string scoreOutputPath = geneSigPath + "_trec_eval";
	map<string, vector<float>> kmerSigDict = GetSigDict(kmerSigPath);

	map<string, vector<float>> geneSigDict;
	if (UT_File::IsFileExist(geneSigPath)) {
		cout << "Gene sig exist. Read from file" << endl;
		geneSigDict = GetSigDict(geneSigPath);
	} else {
		cout << "Gene sig does not exist. Generating" << endl;
		geneSigDict = GenerateGeneSigDict(genePath, kmerSigDict, geneSigPath);
	}

	TopGeneSimilarityScore(geneSigDict, 800, scoreOutputPath);
}
