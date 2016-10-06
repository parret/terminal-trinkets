#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>

using namespace std;

vector<string> commandList;

vector<string> tokenize(char *input, char delim){
    vector<string> tokens;
    string current;
    for(char* c = input; *c; c++) {
        if(*c == delim){
            current += " ";
            tokens.push_back(current);
            current = "";
        } else {
            current += *c;
        }
    }
    current += " ";
    tokens.push_back(current);

    return tokens;
}

void enumAll(const vector<vector<string> > &allVecs, size_t vecIndex, string strSoFar){
    if (vecIndex >= allVecs.size())
    {
        commandList.push_back(strSoFar);
        return;
    }
    for (size_t i=0; i<allVecs[vecIndex].size(); i++)
        enumAll(allVecs, vecIndex+1, strSoFar+allVecs[vecIndex][i]);
}


using namespace std;
int main(int argc, char *argv[]) {
    vector<vector<string> > perms;
    for(int i = 1; i < argc; i++){
        perms.push_back(tokenize(argv[i], '?'));
    }

    string superCommand = "";
    enumAll(perms, 0, "");
    for(string command : commandList){
        superCommand = superCommand + command + " && ";
    }
    superCommand = superCommand.substr(0, superCommand.length() - 3);
    cout << "RUNNING: " << superCommand << endl;
    system(superCommand.c_str());
}
