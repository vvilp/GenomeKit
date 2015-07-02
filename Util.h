//
// Created by vvilp on 2/07/2015.
//

#ifndef GENOMEKIT_UTIL_H
#define GENOMEKIT_UTIL_H

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

#endif //GENOMEKIT_UTIL_H
