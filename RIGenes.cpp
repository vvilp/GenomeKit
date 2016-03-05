#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <string>
#include <map>
#include "Util.hpp"
using namespace std;

vector<int> GetRandomIndexVector(int size, float density)
{
    vector<int> RIndexVector(size);
    int randIndexNum = size * density; //Fill x% position with -1, 0, 1
    int ternary[3] = {-1, 0 ,1};

    for (int i = 0; i < randIndexNum; ++i) {
        RIndexVector[UT_Math::RandInt(0,size-1)] = ternary[UT_Math::RandInt(0,2)];
    }

    return RIndexVector;
}

void KmerRISig()
{
    int kmerLen = 6;
    int sigLen = 1024;
    string path = "TestData2/alfsim-data1/DB/all_dna.fa";
    string savePath = "kmerRISig";

    map<string, vector<int>> kmerSigDict;

    std::ifstream t(path);
    std::stringstream buffer;
    buffer << t.rdbuf();
    string line = "";
    string label = "";
    vector<int> geneRI;
    for (int i = 0; getline(buffer, line); i++) {
        // cout << line << endl;
        if (i % 2 == 0) {
            label = line;
        } else {
            geneRI = GetRandomIndexVector(sigLen, 0.1);

            vector<string> geneKmers = UT_GenomeKit::GetKmers(line, kmerLen);

            for (int ki = 0; ki < geneKmers.size(); ki++) {
                string kmer = geneKmers[ki];
                if(kmerSigDict.find(kmer) == kmerSigDict.end()) {
                    kmerSigDict[kmer] = geneRI;
                } else {
                    vector<int> kmerSig = kmerSigDict[kmer];
                    for(int j = 0; j < sigLen; j++) {
                        kmerSig[j] = kmerSig[j] + geneRI[j];
                    }
                    kmerSigDict[kmer] = kmerSig;
                }
            }
        }
        if(i%100==0) {
            cout << "finish:" << i << endl;
        }
    }

    cout << "Saving" << endl;
    ofstream ofile(savePath);
    map<string, vector<int>>::iterator iter;
    for (iter = kmerSigDict.begin(); iter != kmerSigDict.end(); iter++) {
        ofile << iter->first << endl;
        for (size_t i = 0; i < iter->second.size(); i++) {
            ofile << iter->second[i] << " ";
        }
        ofile << endl;
    }
    ofile.close();

}

int main()
{
    KmerRISig();
}
