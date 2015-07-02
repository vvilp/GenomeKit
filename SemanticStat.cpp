//
// Created by vvilp on 2/07/2015.
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
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


int main()
{
    ifstream infile("/Volumes/Macintosh\ HD\ New/Documents/Data/Genome/all.fna/Escherichia_coli__BL21_Gold_DE3_pLysS_AG__uid59245/NC_012947.fna");

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

    vector<string> kmers = KmerGenerator("ACGT", 5);

    ofstream file;
    file.open("test_5.csv");

    for (int i = 0; i < genomeVec.size(); i++) {
        string genomeFrag = genomeVec[i];
        for (string kmer : kmers) {
            file << CountSubstring(genomeFrag, kmer) << " ";
        }
        file << endl;
        cout << i << endl;
    }
    file.close();

    return 0;
}