#include <iostream>
#include <map>
#include <vector>
#include <random>
#include <utility>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <queue>
#include <cstring>
#include "Util.hpp"
using namespace std;


// void TopClosest(map<string, vector<float>> & wordvecMap, string word)
// {
//     priority_queue <pair<float, string>> q;
//     map<string, vector<float>>::iterator iter;
//     vector<float> wordVec = wordvecMap[word];
//     for (iter = wordvecMap.begin(); iter != wordvecMap.end(); iter++) {
//         if(iter -> first.compare(word) != 0) {
// //            cout << iter -> first << " : ";
// //            cout << Cosdist(iter -> second, wordVec) << endl;
//             q.push(pair<float,string>(UT_Math::CosineSimilarity(iter -> second, wordVec), iter -> first));
//
//         }
//     }
//
//     for (int i = 0; i < 20; i++) {
//         cout << q.top().second << " " << q.top().first << endl;
//         q.pop();
//     }
//
// }

void TopClosest(map<string, vector<float>> & wordvecMap, string word)
{
    int count = 20;
    vector<pair<string,float> *> pairArr (count, NULL);

    map<string, vector<float>>::iterator iter;
    vector<float> wordVec = wordvecMap[word];
    for (iter = wordvecMap.begin(); iter != wordvecMap.end(); iter++) {
        if(iter -> first.compare(word) != 0) {
            float simi = UT_Math::CosineSimilarity(iter -> second, wordVec);
//            cout << simi << endl;
            pair<string,float> * wordDistPair = new pair<string,float>(iter -> first, simi);
//            wordDistPair -> first = iter -> first;
//            wordDistPair -> second = simi;
            for (int i = count - 1; i >= 0; i--) {
                if(pairArr[i] == NULL || pairArr[i]->second < simi) {
                    if (i == count - 1) {
                        pairArr[i] = wordDistPair;
                    }
                    else {
                        pairArr[i+1] = pairArr[i];
                        pairArr[i] = wordDistPair;
                    }

                }
            }
        }
    }

    for (int i = 0; i < pairArr.size(); i++) {
        cout << pairArr[i] -> first << " : " << pairArr[i] -> second << endl;
    }

}

int main(int arg, char*argvs[])
{
    string path(argvs[1]);
    cout << path << endl;

    map<string, vector<float>> wordvecMap;
    ifstream infile(path);
    int vocabCount = 0;
    for (string line; getline(infile, line);) {
        vector<string> raw;
        UT_String::split(line, " \r\n", raw);
        vector<float> wordvec;
        for (int i = 1; i < raw.size(); i++) {
            float d = stof(raw[i]);
            wordvec.push_back(d);
        }
        wordvecMap[raw[0]] = wordvec;
        vocabCount++;
    }
    cout << "vocabCount:" << vocabCount << endl;


    while(1) {
        cout << "input words:";
        string word;
        cin >> word;
        if(wordvecMap.find(word) == wordvecMap.end()) {
            cout << "dont exist" << endl;
            continue;
        }
        TopClosest(wordvecMap, word);
        cout << endl << endl;
    }

    cout << "finish" << endl;

}
