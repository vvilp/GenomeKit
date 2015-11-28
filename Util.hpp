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
    
    static float RandFloat(float min, float max) {
        float random = ((float) rand()) / (float) RAND_MAX;
        float diff = max - min;
        float r = random * diff;
        return min + r;
    }
    
    static float CosineSimilarity(const vector<float> & a, const vector<float> & b)
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
    
    static float sigmoid(float x) {
        return 1.0 / (1.0 + exp(-x));
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
    
    static std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delim)) {
            elems.push_back(item);
        }
        return elems;
    }
    
    static std::vector<std::string> &split(const std::string &s, const string &delims, std::vector<std::string> &elems) {
        std::stringstream stringStream(s);
        std::string line;
        while (std::getline(stringStream, line)) {
            std::size_t prev = 0, pos;
            while ((pos = line.find_first_of(delims, prev)) != std::string::npos) {
                if (pos > prev)
                    elems.push_back(line.substr(prev, pos - prev));
                prev = pos + 1;
            }
            if (prev < line.length())
                elems.push_back(line.substr(prev, std::string::npos));
        }
        return elems;
    }
    
    static std::vector<std::string> split(const std::string &s, char delim) {
        std::vector<std::string> elems;
        split(s, delim, elems);
        return elems;
    }
    
    // trim from start
    static inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
    }
    
    // trim from end
    static inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
    }
    
    // trim from both ends
    static inline std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
    }
};


class UT_GenomeKit
{
private:
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
    
public:
    
    static vector<string> KmerGenerator(const string& letters, int k)
    {
        vector<string> kmers;
        string s = "";
        SearchKmer(k,letters, s, kmers);
        return kmers;
    }
    
    static void CombinationGenerator(int k, int n)
    {
        vector<int> combination;
        vector<vector<int>> combinations;
        searchCombination(0, k, n, combination, combinations);
        cout << combinations.size();
    }
};





#endif //GENOMEKIT_UTIL_H
