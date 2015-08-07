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
#include <random>
using namespace std;

struct ComparatorStringInt {
    bool operator()(std::pair<std::string, int> const &lhs, std::pair<std::string, int> const &rhs) const {
        return lhs.second > rhs.second;
    }
};

class UT_File
{
public:
  static bool IsFileExist (const std::string& name)
  {
    ifstream f(name.c_str());
    if (f.good()) {
        f.close();
        return true;
    } else {
        f.close();
        return false;
    }
  }
};

class UT_Math
{
public:
  static int RandInt(int min, int max)
  {
      std::random_device seeder;
      std::mt19937 engine(seeder());
      std::uniform_int_distribution<int> dist(min, max);
      return dist(engine);
  }

  static float CosineSimilarity(vector<int>a, vector<int> b)
  {
      float res = 0.0;
      float anorm = 0.0;
      float bnorm = 0.0;
      for (int i = 0; i < a.size(); ++i) {
          res += (float)(a[i] * b[i]);
          anorm += (float)(a[i]*a[i]);
          bnorm += (float)(b[i]*b[i]);
      }
      res = res / (sqrt(anorm) * sqrt(bnorm));
      return res;
  }
};

class UT_GenomeKit
{
public:
  static void SearchKmer(int k, const string& letters, string & kmer, vector<string> & kmers)
  {
      if (k == 0) {
          kmers.push_back(kmer);
          return;
      }
      for (int j = 0; j < letters.size(); j++) {
          kmer+= letters[j];
          SearchKmer(k-1, letters, kmer, kmers);
          kmer = kmer.substr(0, kmer.size()-1);
      }
  }
  static vector<string> KmerGenerator(const string& letters, int k)
  {
      vector<string> kmers;
      string s = "";
      SearchKmer(k,letters, s, kmers);
      return kmers;
  }
  static void searchCombination(int dep, int k, int n, vector<int> &combination, vector<vector<int>> & combinations)
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
  static void CombinationGenerator(int k, int n)
  {
      vector<int> combination;
      vector<vector<int>> combinations;
      searchCombination(0, k, n, combination, combinations);
      cout << combinations.size();
  }
};

class UT_String
{
public:
  static int CountSubstring(const std::string& str, const std::string& sub)
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
};








#endif //GENOMEKIT_UTIL_H
