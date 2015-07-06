#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <sstream>
#include <queue>
#include <omp.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "Util.h"

using namespace std;


string orginalFASTAPath;
int k_kmer;

string outNameKmerCountMap;
string outNameTopKmerCount;
string outNameTopPair;

const int _5merCountThreshold = 50;
const int _7merCountThreshold = 5;

enum ProcessState {
    KMER_BEGIN,
    KMER_TABLE,
    KMER_TOP_PAIR_COUNT,
    ERROR,
    COMPLETE
};

void KmerTopCount(const vector<string> &kmers, int col, const int *sumCount);

void ParseArgs(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-i")) {
            orginalFASTAPath = string(argv[++i]);
        }

        if (!strcmp(argv[i], "-k")) {
            k_kmer = atoi(argv[++i]);
        }
    }
}

void KmerCountMap() {
    cout << "-> Kmer Count Map start" << endl;
    ifstream infile(orginalFASTAPath);
    string s = "";
    vector<string> genomeVec;

    //Split genome to multilines
    int i = 0;
    for (string line; getline(infile, line); i++) {
        if (i == 0) continue;

        s.append(line);

        if (s.size() >= 20000) {
            genomeVec.push_back(s);
            s.clear();
        }
    }


    vector<string> kmers = KmerGenerator("ACGT", k_kmer);
    cout << "->-> Kmer generate complete" << endl;


    int **kmerCountMap = new int *[genomeVec.size()];
    for (size_t i = 0; i < genomeVec.size(); i++) {
        kmerCountMap[i] = new int[kmers.size()];
    }


#pragma omp parallel for
    for (int i = 0; i < genomeVec.size(); i++) {
        string genomeFrag = genomeVec[i];

        for (int j = 0; j < kmers.size(); j++) {
            string kmer = kmers[j];
            kmerCountMap[i][j] = CountSubstring(genomeFrag, kmer);
        }
    }


    ofstream file;
    file.open(outNameKmerCountMap);
    file << genomeVec.size() << " " << kmers.size() << endl;
    for (int i = 0; i < genomeVec.size(); i++) {
        for (int j = 0; j < kmers.size(); j++) {
            file << kmerCountMap[i][j] << " ";
        }
        file << endl;
    }


    file.close();

    for (size_t i = 0; i < genomeVec.size(); i++) {
        delete[] kmerCountMap[i];
    }
    delete[] kmerCountMap;

    cout << "-> Kmer Count Map complete" << endl;
}

void KmerTopCount(const vector<string> &kmers, int *sumCount) {
    priority_queue<pair<int, int>> pq;

    for (size_t i = 0; i < kmers.size(); i++)
    {
        pq.push(pair<int, int>(sumCount[i], i));
    }

    ofstream ofile(outNameTopKmerCount);

    for (int i = 0; i < 50; ++i) {
        int index = pq.top().second;
        int value = pq.top().first;
        cout << "index: " << index << " | kmer:" << kmers [index] << " | Sum: " << value << endl;
        ofile << kmers[index] << "\t" << value << endl;
        pq.pop();
    }

    ofile.close();
}


void KmerPairOccurrence(const vector<string> &kmers, int **kmerCountMap, int row, int col) {

    priority_queue<pair<int, pair<int, int>>> pq;
    int kmerCountThreshhold = k_kmer == 5 ? _5merCountThreshold : _7merCountThreshold;
#pragma omp parallel for
    for (int i = 0; i < col; i++) {
        for (int j = i + 1; j < col; j++) {
            int count = 0;
            for (int lineIndex = 0; lineIndex < row; lineIndex++) {
                if (kmerCountMap[lineIndex][i] > kmerCountThreshhold && kmerCountMap[lineIndex][j] > kmerCountThreshhold) {
                    count++;
                }
            }
            pair<int, pair<int, int>> OccurencePair(count, pair<int, int>(i, j));
            pq.push(OccurencePair);
            if (pq.size() % 1000000 == 0) {
                cout << "Current Q size:" << pq.size() << endl;
            }

        }
    }

    ofstream ofile(outNameTopPair);
    for (int i = 0; i < 50; ++i) {
        int value = pq.top().first;
        pair<int, int> indexPair = pq.top().second;
        cout << value << " | kmer1: " << indexPair.first << " " << kmers[indexPair.first] <<
        " | kmer2: " << indexPair.second << " " << kmers[indexPair.second] << endl;
        ofile << kmers[indexPair.first] << "," << kmers[indexPair.second] << "\t" << value << endl;
        pq.pop();
    }
    ofile.close();

}


void KmerTopPairCount()
{
    cout << "-> KmerTopPairCount Start" << endl;

    vector<string> kmers = KmerGenerator("ACGT", k_kmer);
    ifstream infile(outNameKmerCountMap);
    int row, col;
    infile >> row >> col;

    int **kmerCountMap = new int *[row];
    int *sumCount = new int[col];
    memset(sumCount, 0, sizeof(int) * col);
    for (size_t i = 0; i < row; i++) {
        kmerCountMap[i] = new int[col];
        for (size_t j = 0; j < col; j++) {
            infile >> kmerCountMap[i][j];
            sumCount[j] += kmerCountMap[i][j];
        }
    }

   KmerTopCount(kmers, sumCount);
    cout << "->-> KmerTopCount Finish" << endl;
    KmerPairOccurrence(kmers, kmerCountMap, row, col);
    cout << "->-> KmerPairOccurence Finish" << endl;

    cout << "-> KmerTopPairCount Complete" << endl;
}

ProcessState FileNameProcess()
{
    cout << "-> File Name Process" << endl;
    if(!IsFileExist(orginalFASTAPath)) {
        return ProcessState::ERROR;
    }
    int findIndex = orginalFASTAPath.find_last_of("/\\");
    findIndex = findIndex >= 0 && findIndex < orginalFASTAPath.length() ? findIndex : -1;
    string fileName = orginalFASTAPath.substr(findIndex+1);

    outNameKmerCountMap = fileName + "_" +to_string(k_kmer) + "merCountMap.out";
    outNameTopKmerCount = fileName + "_" +to_string(k_kmer) + "merTopKmerCount.out";
    outNameTopPair = fileName + "_" +to_string(k_kmer) + "merTopPair.out";

    cout << "-> File Name Process complete" << endl;
    if(IsFileExist(outNameKmerCountMap)) {
        return ProcessState::KMER_TOP_PAIR_COUNT;
    }

    return ProcessState::KMER_TABLE;
}

int main(int argc, char *argv[]) {

    ParseArgs(argc, argv);
    ProcessState state = ProcessState::KMER_BEGIN;

    while(state != ProcessState::COMPLETE)
    {
        switch(state) {
            case ProcessState::KMER_BEGIN:
                state = FileNameProcess();
                break;
            case ProcessState::KMER_TABLE:
                KmerCountMap();
                state = ProcessState::KMER_TOP_PAIR_COUNT;
                break;
            case ProcessState::KMER_TOP_PAIR_COUNT:
                KmerTopPairCount();
                state = ProcessState::COMPLETE;
                break;
            case ProcessState::ERROR:
                cout << "ERROR" << endl;
                state = ProcessState::COMPLETE;
                break;
            default:
                break;
        }
    }

    cout << "Finish" << endl;
    //getchar();
    return 0;
}