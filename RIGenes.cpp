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

void KmerRISig(int kmerLen, int sigLen, string path, string savePath)
{
    cout << kmerLen << endl;
    cout << sigLen << endl;
    cout << path << endl;
    cout << savePath << endl;

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

int main(int arg, char *argvs[])
{
    srand(time(NULL));
    int kmerLen = stoi(string(argvs[1]));
    int sigLen = stoi(string(argvs[2]));
    string genePath = string(argvs[3]);
    string savePath = string(argvs[4]);
    KmerRISig(kmerLen,sigLen,genePath,savePath);


}
