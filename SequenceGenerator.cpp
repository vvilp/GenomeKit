//
// Generator similar random sequence
//
// SequenceGenerator -c "ACGT" -len 500 -line 20 -diffNum 10 (-diffRate 0.2) -out test.txt


#include <iostream>
#include <fstream>
#include <ctime>
#include <string>

using namespace std;

const char *clist = NULL;
int len = 100;
int line = 10;
int diffNum = 10;
const char *filePath = NULL;

void ParseArgs(int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-c")) {
            clist = argv[++i];
            cout << "Generate From: "<< clist << endl;
        }

        if (!strcmp(argv[i], "-len")) {
            len = atoi(argv[++i]);
            cout << "Generate length: "<< len << endl;
        }

        if (!strcmp(argv[i], "-line")) {
            line = atoi(argv[++i]);
            cout << "Generate lines: "<< line << endl;
        }

        if (!strcmp(argv[i], "-diffNum")) {
            diffNum = atoi(argv[++i]);
            cout << "different char number each line: "<< diffNum << endl;
        }

        if (!strcmp(argv[i], "-diffRate")) {
            double diffRate = atof(argv[++i]);
            diffNum = int (double(len) * diffRate);
            cout << "different char number each line: "<< diffNum << endl;
        }

        if (!strcmp(argv[i], "-out")) {
            filePath = argv[++i];
            cout << "output file path: "<< filePath << endl;
        }
    }
}


int RandRange(int min, int max) {
    return min + (rand() % (max - min + 1));
}


string RandString(string s, int change_num) {
    for (int i = 0; i < change_num; ++i) {
        int randStrIndex = RandRange(0, (int) s.length() - 1);
        int randCIndex = RandRange(0, (int) strlen(clist) - 1);
        s[randStrIndex] = clist[randCIndex];
    }
    return s;
}


int main(int argc, char *argv[]) {

    ParseArgs(argc, argv);

    if (clist == NULL) {
        clist = "ACGT";
    }

    if (filePath == NULL) {
        filePath = "example.txt";
    }

    string randStr;
    srand((unsigned int) time(0));

    for (int i = 0; i < len; ++i) {
        int randCIndex = RandRange(0, (int) strlen(clist) - 1);
        randStr.append(1, clist[randCIndex]);
    }

    ofstream file;
    file.open(filePath);
    for (int i = 0; i < line; ++i) {
        file << RandString(randStr, diffNum) << endl;
    }

    cout << "Generate Completely" << endl;

    return 0;
}
