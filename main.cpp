#include <iostream>
#include <vector>
#include <fstream>
#include <stdint.h>
#include <string>
#include <omp.h>
#include "Util.h"
using namespace std;

class Bit {
public:
    static void PrintBit(uint32_t hash) {
        for (int i = 0; i < sizeof(hash) * 8; ++i) {
            putc((hash & 1) ? '1' : '0', stdout);
            hash = hash >> 1;
        }
        putc('\n', stdout);
    }

    static int HammingDistance(uint32_t x, uint32_t y) {
        int dist = 0;
        unsigned c = x ^ y;
        c = c - ((c >> 1) & 0x55555555);
        c = (c & 0x33333333) + ((c >> 2) & 0x33333333);
        c = ((c + (c >> 4) & 0xF0F0F0F) * 0x1010101) >> 24;
        return c;
    }

};


class Hash {
private:
    enum HashMethod {
        FNV_32,
        FNV_64,
        FNV_128,
    };

    static const uint32_t FNV_PRIME_32 = 16777619;
    static const uint32_t FNV_OFFSET_32 = 2166136261U;

public:

    static uint32_t FNV32(const char *s) {
        uint32_t hash = FNV_OFFSET_32, i;
        for (i = 0; i < strlen(s); i++) {
            hash = hash ^ (s[i]);
            hash = hash * FNV_PRIME_32;
        }
        return hash;
    }

    static uint32_t FNV32(const char *s, int count) {
        uint32_t hash = FNV_OFFSET_32, i;
        for (i = 0; i < count; i++) {
            hash = hash ^ (s[i]);
            hash = hash * FNV_PRIME_32;
        }
        return hash;
    }
};


class SimHash {
private:
    int baseSigSize = 32;

public:
    uint32_t GetSignature(const char *s) {
        int kmerSize = 3;
        uint32_t sig = 0;
        uint32_t *kmerSigsArray = new uint32_t[strlen(s)];
        int kmerSigsArrayLen = (int)strlen(s) - kmerSize;
        int *sigV = new int[baseSigSize];
        memset(sigV, 0, baseSigSize * sizeof(*sigV));

        #pragma omp parallel for
        for (int i = 0; i < kmerSigsArrayLen; i++)
        {
            kmerSigsArray[i] = Hash::FNV32(s + i, kmerSize);
        }
        //cout << "Step1 Kmer Signature Finished" << endl;

        for (size_t i = 0; i < kmerSigsArrayLen; i++)
        {
            uint32_t kmerSig = kmerSigsArray[i];
            for (int i = 0; i < sizeof(kmerSig) * 8; ++i) {
                sigV[i] += (kmerSig & 1) ? 1 : -1;
                kmerSig = kmerSig >> 1;
            }
        }

        for (size_t i = 0; i < baseSigSize; i++)
        {
            sig = ((sig << 1) | ((sigV[i] > 0) ? 1 : 0));
        }


        //for (size_t i = 0; i < sigSize; i++)
        //{
        //     //cout << ((sigV[i] > 0) ? 1 : 0);
        //     cout << ((sigV[i])) << " ";
        //}
        //cout << endl;

        delete []sigV;
        delete []kmerSigsArray;
        return sig;
    }


};

double AvgPairDistance(vector<uint32_t> s)
{
    int count = 0;
    int avgDistance = 0;
    for (size_t i = 0; i < s.size(); i++)
    {
        for (size_t j = i + 1; j < s.size(); j++)
        {
            count++;
            avgDistance += Bit::HammingDistance(s[i], s[j]);
        }
    }
    return (double)avgDistance / (double)count;
}

double AvgPairDistance(uint32_t sig, vector<uint32_t> s)
{
    int count = 0;
    int avgDistance = 0;
    for (size_t i = 0; i < s.size(); i++)
    {
        count++;
        avgDistance += Bit::HammingDistance(s[i], sig);
    }
    return (double)avgDistance / (double)count;
}

double AvgPairDistance(vector<uint32_t> s1, vector<uint32_t> s2)
{
    int count = 0;
    int avgDistance = 0;
    for (size_t i = 0; i < s1.size(); i++)
    {
        for (size_t j = 0; j < s2.size(); j++)
        {
            count++;
            avgDistance += Bit::HammingDistance(s1[i], s2[j]);
        }
    }
    return (double)avgDistance / (double)count;
}

void TestCase1()
{
    SimHash sh;
    const char *sequence1 = "AAAAAAAA";
    const char *sequence2 = "AAAAACCC";
    const char *sequence3 = "AAACCAAA";
    const char *sequence4 = "AAACGTAA";
    const char *sequence5 = "AAAACCCC";
    const char *sequence6 = "CCCCCCCC";
    const char *sequence7 = "ACACACAC";

    cout << sequence1 << " - " << sequence2 << " : " <<
            Bit::HammingDistance(sh.GetSignature(sequence1), sh.GetSignature(sequence2)) << endl;
    cout << sequence1 << " - " << sequence3 << " : " <<
            Bit::HammingDistance(sh.GetSignature(sequence1), sh.GetSignature(sequence3)) << endl;
    cout << sequence1 << " - " << sequence4 << " : " <<
            Bit::HammingDistance(sh.GetSignature(sequence1), sh.GetSignature(sequence4)) << endl;
    cout << sequence1 << " - " << sequence5 << " : " <<
            Bit::HammingDistance(sh.GetSignature(sequence1), sh.GetSignature(sequence5)) << endl;
    cout << sequence1 << " - " << sequence6 << " : " <<
            Bit::HammingDistance(sh.GetSignature(sequence1), sh.GetSignature(sequence6)) << endl;
    cout << sequence1 << " - " << sequence7 << " : " <<
            Bit::HammingDistance(sh.GetSignature(sequence1), sh.GetSignature(sequence7)) << endl;
}

void TestCase2()
{
    SimHash sh;
    ifstream infile("example.txt");
    vector<uint32_t> s;
    for (string line; getline(infile, line);)
    {
        s.push_back(sh.GetSignature(line.c_str()));
    }
    infile.close();
    cout << "Example pair | AVG distance : " << AvgPairDistance(s) << endl;

    infile.open("example2.txt");
    vector<uint32_t> s2;
    for (string line; getline(infile, line);)
    {
        s2.push_back(sh.GetSignature(line.c_str()));
    }
    infile.close();
    cout << "Example2 pair | AVG distance : " << AvgPairDistance(s2) << endl;

    cout << "Example - Example2 | AVG distance : " << AvgPairDistance(s, s2) << endl;
}

void TestCase3()
{
}

int main()
{
    //TestCase1();
    //getchar();
    //CombinationGenerator(4,100);
    return 0;
}
