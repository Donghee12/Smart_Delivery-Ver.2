#include "utils.h"
#include <fstream>
#include <iostream>

using namespace std;

vector<string> classNames;

void loadClassNames(const string& filePath) {
    ifstream file(filePath);
    if (!file.is_open()) {
        cerr << "Error: Could not open class file!" << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        classNames.push_back(line);
    }
    file.close();
}

string getClassName(int classID) {
    if (classID >= 0 && classID < classNames.size()) {
        return classNames[classID];
    }
    return "Unknown";
}
