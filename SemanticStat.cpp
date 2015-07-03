#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <sstream>
#include <queue>
#include <omp.h>
#include "Util.h"

using namespace std;


string filePathFrom;
string filePathTo;
int k_kmer;

void ParseArgs(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-i")) {
            filePathFrom = argv[++i];
            cout << "Generate From: " << filePathFrom << endl;
        }

        if (!strcmp(argv[i], "-o")) {
            filePathTo = argv[++i];
            cout << "output file path: " << filePathTo << endl;
        }

        if (!strcmp(argv[i], "-k")) {
            k_kmer = atoi(argv[++i]);
            cout << "k: " << k_kmer << endl;
        }
    }
}

int CountKmer(int argc, char *argv[]) {
    ParseArgs(argc, argv);
    ifstream infile(filePathFrom);

    string s = "";
    vector<string> genomeVec;
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
    cout << "Kmer generate complete" << endl;


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
    file.open(filePathTo);
    for (int i = 0; i < genomeVec.size(); i++) {
        for (int j = 0; j < kmers.size(); j++) {
            file << kmerCountMap[i][j] << " ";
        }
        file << endl;
    }


    file.close();
    cout << "finish" << endl;
    getchar();

    return 0;
}


struct CompareTester {
    bool operator()(std::pair<std::string, int> const &lhs, std::pair<std::string, int> const &rhs) const {
        return lhs.second > rhs.second;
    }
};

void KmerPairOccurrence(const vector<string> &kmers, int **kmerCountMap, int row, int col) {

    priority_queue<pair<int, pair<int, int>>> pq;

    #pragma omp parallel for
    for (int i = 0; i < col; i++) {
        for (int j = i + 1; j < col; j++) {
            int count = 0;
            for (int lineIndex = 0; lineIndex < row; lineIndex++) {
                if (kmerCountMap[lineIndex][i] > 5 && kmerCountMap[lineIndex][j] > 5) {
                    count++;
                }
            }
            pair<int, pair<int, int>> OccurencePair(count, pair<int,int>(i,j));
            pq.push(OccurencePair);
            if(pq.size() % 1000000 == 0) {
                cout << "Current Q size:" << pq.size() << endl;
            }

        }
    }

    ofstream ofile("7merPairOccurrence.txt");
    for (int i = 0; i < 20; ++i) {
    	int value = pq.top().first;
        pair<int,int> indexPair = pq.top().second;
        cout << value << " | kmer1: " << indexPair.first << " " << kmers[indexPair.first] <<
                " | kmer2: " << indexPair.second << " " << kmers[indexPair.second] <<endl;
        ofile << kmers[indexPair.first] << "," << kmers[indexPair.second] << "\t" << value << endl;
    	pq.pop();
    }
    ofile.close();

}

int main(int argc, char *argv[]) {
    vector<string> kmers = KmerGenerator("ACGT", 7);
    ifstream infile("test-k7.csv");
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

    cout << "Read File Finish" << endl;

    KmerPairOccurrence(kmers, kmerCountMap, row, col);

    // kmer-count-top50

//    priority_queue<pair<int, int>> pq;
//
//    for (size_t i = 0; i < col; i++)
//    {
//    	pq.push(pair<double, int>(sumCount[i], i));
//    	//cout << sumCount[i] << endl;
//    }
//
//    ofstream ofile("kmer-count-top50.txt");
//
//    for (int i = 0; i < 50; ++i) {
//    	int index = pq.top().second;
//    	int value = pq.top().first;
//    	std::cout << "index: " << index << " | kmer:" << kmers [index] << " | Sum: " << value << std::endl;
//    	ofile << kmers[index] << "\t" << value << endl;
//    	pq.pop();
//    }
//
//    ofile.close();



    cout << "Finish" << endl;
    getchar();
    return 0;
}