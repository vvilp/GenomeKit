/*
Generate Genome Signature by RandomIndexing
RIGenome KmerCountFile
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <string>
#include <map>
#include <omp.h>
#include "Util.hpp"
#include <boost/filesystem.hpp>
using namespace std;

namespace fs = boost::filesystem;

vector<string> GetFileList(string dirPath)
{
    vector<string> fileList;
    fs::path someDir(dirPath);
    fs::directory_iterator end_iter;
    if (fs::exists(someDir) && fs::is_directory(someDir)) {
        for (fs::directory_iterator dir_iter(someDir); dir_iter != end_iter; ++dir_iter) {
            if (fs::is_regular_file(dir_iter->path())) {
                string current_file = dir_iter->path().string();
                //cout << current_file << endl;
                fileList.push_back(current_file);
            }
        }
    }
    return fileList;
}

vector<int> GetRandomIndexVector(int size, float density)
{
    vector<int> RIndexVector(size);
    int randIndexNum = size * density; //Fill x% position with -1, 0, 1
    int ternary[3] = {-1, 0 ,1};

    for (int i = 0; i < randIndexNum; ++i) {
        RIndexVector[UT_Math::RandInt(0,size)] = ternary[UT_Math::RandInt(0,2)];
    }

    return RIndexVector;
}

template<typename T>
void PrintVector(const vector<T> &v)
{
    for (int i = 0; i < v.size(); ++i) {
        cout << v[i] << " ";
    }
    cout << endl;
}

void GenomeRISig()
{
    int indexSize = 4096 * 10;
    int fnum = 9;
    string kCountPath[] = {
            "kmer_count_10/Abaca_bunchy_top_virus_uid28697_NC_010314.fna.fa",
            "kmer_count_10/Abaca_bunchy_top_virus_uid28697_NC_010315.fna.fa",
            "kmer_count_10/Abaca_bunchy_top_virus_uid28697_NC_010316.fna.fa",
            "kmer_count_10/Abutilon_mosaic_Bolivia_virus_uid62479_NC_015045.fna.fa",
            "kmer_count_10/Abutilon_mosaic_Bolivia_virus_uid62479_NC_015048.fna.fa",
            "kmer_count_10/Abutilon_mosaic_Brazil_virus_uid81009_NC_016574.fna.fa",
            "kmer_count_10/Adeno_associated_virus___1_uid15323_NC_002077.fna.fa",
            "kmer_count_10/Adeno_associated_virus___2_uid14060_NC_001401.fna.fa",
            "kmer_count_10/Adeno_associated_virus___3_uid14319_NC_001729.fna.fa",
    };

    string output = "GenomeRI";
    ofstream ofile(output);
    //map<string, vector<int>> genomeRI;
    vector<vector<int>> genomeRIs;
    map<string, vector<int>> kmersRIs;
    for (int i = 0; i < fnum; ++i) {
        ifstream infile(kCountPath[i]);
        vector<int> genomeRI(indexSize);
        for (string line; getline(infile, line);)
        {
            if(line[0] == '>') continue;
            vector<int> kmerRI;
            if(kmersRIs.find(line) == kmersRIs.end()) {
                kmerRI = GetRandomIndexVector(indexSize, 0.001);
                kmersRIs.insert(std::pair<string, vector<int>>(line,kmerRI));
            } else {
                kmerRI = kmersRIs[line];
            }

            for (int j = 0; j < indexSize; ++j) {
                genomeRI[j] += kmerRI[j];
            }
        }
        infile.close();

        for (int k = 0; k < indexSize; ++k) {
            ofile << (genomeRI[k]==0? 0:1) << " ";
        }
        ofile << endl;
        int numofone = 0;
        for (int k = 0; k < indexSize; ++k) {
            numofone += (genomeRI[k]==0? 0:1);
        }
        cout << "num of 1:" << numofone << endl;
        cout << endl;
        genomeRIs.push_back(genomeRI);

    }

    ofile.close();

    for (int i = 0; i < genomeRIs.size(); ++i) {
        for (int j = 0; j < genomeRIs.size(); ++j) {
            int hdis = 0;
            for (int k = 0; k < indexSize; ++k) {
                if((genomeRIs[i][k] == 0 && genomeRIs[j][k]!=0) || (genomeRIs[i][k] != 0 && genomeRIs[j][k]==0))
                    hdis ++;
            }
            cout << i << "-" << j << " hamming dis: " << hdis << " | cos: " << UT_Math::CosineSimilarity(genomeRIs[i], genomeRIs[j]) << endl;


        }
        cout << endl;
    }
}

bool isFileContainStr(string filePath, string str)
{
    bool isContain = false;
    ifstream infile(filePath);
    infile.seekg(0, std::ios::end);
    std::streamsize filesize = infile.tellg();
    infile.seekg(0, std::ios::beg);
    char *buffer = new char[filesize];
    if (infile.read(buffer, filesize))
    {
        char *p = strstr(buffer,str.c_str());
        if(p != NULL) {
            isContain = true;
        }
    }

    delete buffer;
    infile.close();
    return isContain;
}

int SearchKmerCount(string filePath, string str)
{
    int count = 0;
    ifstream infile(filePath);
    infile.seekg(0, std::ios::end);
    std::streamsize filesize = infile.tellg();
    infile.seekg(0, std::ios::beg);
    char *buffer = new char[filesize];
    if (infile.read(buffer, filesize))
    {
        char *p = strstr(buffer,str.c_str());
        if(p != NULL) {
            char *pre = p;
            while(*(pre--) != '>') {}
            char sint[64];
            int i = 0;
            for (char *cp = pre+2 ; cp < p-1 ; cp++, i++) {
                sint[i] = *cp;
            }
            count = atoi(sint);
        }
    }

    delete buffer;
    infile.close();
    return count;
}


void KmerRISig()
{
    int indexSize = 512;
    vector<string> kmers = UT_GenomeKit::KmerGenerator("ACGT", 6);
    int knum = kmers.size();
    cout << "6mer Count : "<< kmers.size() << endl;

    vector<string> fileList = GetFileList("512_Genome_6mer_Count");
    map<string, vector<int>> genomeRIs;

    ofstream ofile("6mer_RandIndexing_Sig");
    for (int k = 0; k < knum; ++k) {
        vector<int> kmerRI(indexSize);
        for (int i = 0; i < fileList.size(); ++i) {
            if(isFileContainStr(fileList[i], kmers[k])){
                //cout << fileList[i] << endl;
                vector<int> genomeRI;
                if(genomeRIs.find(fileList[i]) == genomeRIs.end()) {
                    genomeRI = GetRandomIndexVector(indexSize, 0.1);
                    genomeRIs.insert(std::pair<string, vector<int>>(fileList[i],genomeRI));
                } else {
                    genomeRI = genomeRIs[fileList[i]];
                }

                for (int j = 0; j < indexSize; ++j) {
                    kmerRI[j] += genomeRI[j];
                }
            }
        }

        cout << kmers[k] << " ";
        for (int l = 0; l < indexSize; ++l) {
            // cout << (kmerRI[l] == 0 ? 0:1) << " ";
            // ofile << kmerRI[l] << " ";
            ofile << (kmerRI[l] <= 0 ? 0:1) << " ";
        }
        cout << endl;
        ofile << endl;
    }
    ofile.close();

}

void KmerCountSequence()
{
    int kmerCount = 500;
    vector<string> kmers;

    ifstream kmerListFile("kmerList");
    int i = 0;
    for (string line; getline(kmerListFile, line) && i < kmerCount; i++) {
        kmers.push_back(line);
    }
    kmerListFile.close();

    ofstream ofile("kmer_count_vector");
    vector<string> fileList = GetFileList("kmer_count_10");
    map<string, vector<int>> kmerCountMap;

    //#pragma omp parallel for
    for (int k = 0; k < kmers.size(); ++k) {
        vector<int> kmerCountVec;
        for (int i = 0; i < fileList.size(); ++i) {
            kmerCountVec.push_back(SearchKmerCount(fileList[i], kmers[k]));
        }
        kmerCountMap.insert(std::pair<string, vector<int>>(kmers[k],kmerCountVec));
        cout << "Complete:" << k << " | " << kmers[k] << endl;
    }

    for(map<string, vector<int>>::iterator it = kmerCountMap.begin(); it != kmerCountMap.end(); it++) {
        //cout << it->first << endl;
        //PrintVector(it->second);
        ofile << it->first << endl;
        vector<int> kmerCountVec = it->second;
        for (size_t i = 0; i < kmerCountVec.size(); i++) {
            ofile << kmerCountVec[i] << " ";
        }
        ofile << endl;
    }

    ofile.close();
}

void _6merCountSequence()
{
    vector<string> kmers = UT_GenomeKit::KmerGenerator("ACGT", 6);
    cout << "6mer Count : "<< kmers.size() << endl;

    vector<string> fileList = GetFileList("512_Genome_6mer_Count");
    map<string, vector<int>> kmerCountMap;

    for (int k = 0; k < kmers.size(); ++k) {
        vector<int> kmerCountVec;
        for (int i = 0; i < fileList.size(); ++i) {
            kmerCountVec.push_back(SearchKmerCount(fileList[i], kmers[k]));
        }
        kmerCountMap.insert(std::pair<string, vector<int>>(kmers[k],kmerCountVec));
        cout << "Complete:" << k << " | " << kmers[k] << endl;
    }

    ofstream ofile("512_6mer_count_vector");
    for(map<string, vector<int>>::iterator it = kmerCountMap.begin(); it != kmerCountMap.end(); it++) {
        //cout << it->first << endl;
        //PrintVector(it->second);
        ofile << it->first << endl;
        vector<int> kmerCountVec = it->second;
        for (size_t i = 0; i < kmerCountVec.size(); i++) {
            ofile << kmerCountVec[i] << " ";
        }
        ofile << endl;
    }

    ofile.close();
}

int main(int argc, char *argv[])
{
    KmerRISig();
    // KmerCountSequence();
    // _10merCountSequence();
    return 0;
}
