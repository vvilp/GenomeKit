//
// Created by vvilp on 2/07/2015.
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <omp.h>
using namespace std;

int CountSubstring(const std::string& str, const std::string& sub)
{
    if (sub.length() == 0) return 0;
    int count = 0;
    for (size_t offset = str.find(sub); offset != std::string::npos;
         offset = str.find(sub, offset + sub.length()))
    {
        ++count;
    }
    return count;
}

void SearchKmer(int k, const string& letters, string & kmer, vector<string> & kmers)
{
    if (k == 0)
    {
        kmers.push_back(kmer);
        return;
    }

    for (int j = 0; j < letters.size(); j++)
    {
        kmer+= letters[j];
        SearchKmer(k-1, letters, kmer, kmers);
        kmer = kmer.substr(0, kmer.size()-1);
    }
}

vector<string> KmerGenerator(const string& letters, int k)
{
    vector<string> kmers;
    string s = "";
    SearchKmer(k,letters, s, kmers);
    return kmers;
}


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


int main(int argc, char *argv[])
{
	ParseArgs(argc, argv);
	ifstream infile(filePathFrom);

    string s = "";
    vector<string> genomeVec;
    int i = 0;
    for (string line; getline(infile, line); i++)
    {
        if(i == 0) continue;

        s.append(line);

        if(s.size() >= 20000) {
            genomeVec.push_back(s);
            s.clear();
        }
    }


	vector<string> kmers = KmerGenerator("ACGT", k_kmer);
	cout << "Kmer generate complete" << endl;


	int **kmerCountMap = new int*[genomeVec.size()];
	for (size_t i = 0; i < genomeVec.size(); i++)
	{
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