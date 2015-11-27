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


void Top10Closest(map<string, vector<float>> & wordvecMap, string word)
{
    priority_queue <pair<float, string>> q;
    map<string, vector<float>>::iterator iter;
    vector<float> wordVec = wordvecMap[word];
    for (iter = wordvecMap.begin(); iter != wordvecMap.end(); iter++) {
        if(iter -> first.compare(word) != 0) {
//            cout << iter -> first << " : ";
//            cout << Cosdist(iter -> second, wordVec) << endl;
            q.push(pair<float,string>(UT_Math::CosineSimilarity(iter -> second, wordVec), iter -> first));
            
        }
    }
    
    for (int i = 0; i < 10; i++) {
        cout << q.top().second << " " << q.top().first << endl;
        q.pop();
    }
    
}

int main()
{
    map<string, vector<float>> wordvecMap;
    ifstream infile("testData_WordRepresentation");
    int vocabCount = 0;
    for (string line; getline(infile, line);) {
        vector<string> raw;
        UT_String::split(line, " ", raw);
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
        Top10Closest(wordvecMap, word);
        cout << endl << endl;
    }
    
    cout << "finish" << endl;

}
