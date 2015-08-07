//
// Created by vvilp on 13/07/2015.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <string>
#include <map>
#include <boost/filesystem.hpp>

#include "Util.hpp"

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

vector<int> GetRandomIndexVector(int size)
{
    vector<int> RIndexVector(size);
    int randIndexNum = size * 0.05; //Fill 5% position with -1, 0, 1
    int ternary[3] = {-1, 0 ,1};

    for (int i = 0; i < randIndexNum; ++i) {
        RIndexVector[UT_Math::RandInt(0,size)] = ternary[UT_Math::RandInt(0,2)];
    }

    return RIndexVector;
}

void GeneratekmerRandomIndexingVector()
{
    int indexVectorLen = 1024;
    vector<string> fileList = GetFileList("/Volumes/Macintosh HD New/Documents/Data/Genome/TestVirusData/kmer_count/");
    map<string, vector<int>> contextMapVectors;// kmer -> context vector

    for (int i = 0; i < fileList.size(); ++i) {
        vector<int> randomIndexVector = GetRandomIndexVector(indexVectorLen);

        ifstream infile(fileList[i]);
        for (string line; getline(infile, line);)
        {
            if(line[0] == '>') continue;
            //cout << line << endl;
            if(contextMapVectors.find(line) == contextMapVectors.end()) {
                contextMapVectors.insert(std::pair<string, vector<int>>(line,randomIndexVector));
            } else {
                vector<int> contextVector = contextMapVectors[line];
                for (int j = 0; j < indexVectorLen; ++j) {
                    contextVector[j] += randomIndexVector[j];
                }
                contextMapVectors[line] = contextVector;
            }

        }
        infile.close();
        cout << "Finish - " << fileList[i] << endl;
    }
    cout << "all finish" << endl;
    //output
    ofstream ofile("kmer_contextVector");
    for (auto iter : contextMapVectors) {
        ofile << iter.first << endl;
        vector<int> contextVector = iter.second;
        for (int i = 0; i < indexVectorLen; ++i) {
            ofile << contextVector[i] << " ";
        }
        ofile << endl;
    }
    ofile.close();
}

int main()
{
    int indexVectorLen = 1024;
    ifstream infile("kmer_contextVector");
    map<string, vector<int>> contextMapVectors;
    vector<string> kmerVector;
    for (int i = 0; i < 10; ++i) {
        string kmer;
        infile >> kmer;
        vector<int> kmerContextVector(indexVectorLen);
        for (int j = 0; j <indexVectorLen; ++j) {
            infile >> kmerContextVector[j];
        }
        contextMapVectors.insert(std::pair<string, vector<int>>(kmer,kmerContextVector));
        kmerVector.push_back(kmer);
    }

    for (int k = 0; k < kmerVector.size(); ++k) {
        for (int i = k+1; i < kmerVector.size(); ++i) {
            cout << kmerVector[k] << " - " << kmerVector[i] << " : " <<
                    UT_Math::CosineSimilarity(contextMapVectors[kmerVector[k]], contextMapVectors[kmerVector[i]]) << endl;
        }
    }


//    for (auto iter : contextMapVectors) {
//        cout << iter.first << endl;
//        vector<int> contextVector = iter.second;
//        for (int i = 0; i < indexVectorLen; ++i) {
//            cout << contextVector[i] << " ";
//        }
//        cout << endl;
//    }

    infile.close();

}
