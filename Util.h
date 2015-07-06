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

struct ComparatorStringInt {
    bool operator()(std::pair<std::string, int> const &lhs, std::pair<std::string, int> const &rhs) const {
        return lhs.second > rhs.second;
    }
};

bool IsFileExist (const std::string& name) {
    ifstream f(name.c_str());
    if (f.good()) {
        f.close();
        return true;
    } else {
        f.close();
        return false;
    }
}

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


//
void searchCombination(int dep, int k, int n, vector<int> &combination, vector<vector<int>> & combinations)
{
    if(combination.size() == k) {
        combinations.push_back(combination);
        return;
    }

    for (int i = dep; i < n; ++i) {
        combination.push_back(i);
        searchCombination(i+1, k, n, combination, combinations);
        combination.pop_back();
    }
}
void CombinationGenerator(int k, int n)
{
    vector<int> combination;
    vector<vector<int>> combinations;
    searchCombination(0, k, n, combination, combinations);

//    for (int i = 0; i < combinations.size(); ++i) {
//        for (int j = 0; j < combinations[i].size(); ++j) {
//            cout << combinations[i][j] << " ";
//        }
//        cout << endl;
//    }

    cout << combinations.size();
}

#endif //GENOMEKIT_UTIL_H
