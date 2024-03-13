#include <cstring>
#include <fstream>
#include <fstream>
#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include <climits>
#include <chrono>

using namespace std;

const int ALPHABET_SIZE = 95;
const int MAX_NODES = 200000;

class TrieNode {
public:
    int children[ALPHABET_SIZE];
    bool end;
    int value;

    TrieNode() : end(false), value(INT_MIN){
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            children[i] = -1;
        }
    }
};

class Trie {
    private:
        TrieNode nodes[MAX_NODES];
        int nCount;
        int nextNode;
    
    public:
        Trie() : nCount(1), nextNode(1) {}

        void insert(string str, int value){
            int current = 0;

            for (char c : str) {
                int idx = c - 32;
                if (nodes[current].children[idx] == -1) {
                    nodes[current].children[idx] = nextNode++;
                }

                current = nodes[current].children[idx];
            }

            nodes[current].end = true;
            if(value > nodes[current].value)
                nodes[current].value = value;
        }

        bool search(string str){
            int current = 0;

            for (char c : str) {
                int idx = c - 32;
                if (nodes[current].children[idx] == -1) {
                    return false;
                }

                current = nodes[current].children[idx];
            }

            return (current != -1 && nodes[current].end);
        }

        void printhelp(int nodeIndex, vector<char>& cword, ofstream& outFile) {
            if (nodes[nodeIndex].end) {
                for (char c : cword) {
                    outFile << c;
                }
                outFile << " " << nodes[nodeIndex].value << "\n";
            }

            for (int i = 0; i < ALPHABET_SIZE; ++i) {
                if (nodes[nodeIndex].children[i] != -1) {
                    cword.push_back(static_cast<char>(i + 32));
                    printhelp(nodes[nodeIndex].children[i], cword, outFile);
                    cword.pop_back();
                }
            }
        }

        void print(const string& filename) {
            ofstream outFile(filename);
            if (!outFile.is_open()) {
                cerr << "Error opening file " << filename << "\n";
                return;
            }

            vector<char> cword;
            printhelp(0, cword, outFile);

            outFile.close();
        }
};

Trie trie;

enum state {
    STRING,
    NUM
};

void parseFile(const char* str, size_t size){
    state state = STRING;
    int counter = 1;

    bool strstart = false;
    bool numstart = false;
    bool bslash = false;
    bool hasnum = false;

    char temp[21];
    char tnum[15];
    int tempIndex = 0;
    int tnumIndex = 0;

    for(size_t i = 0; i < size; i++){
        char cchar = str[i];

        switch(state){
            case STRING:
                if(cchar == ' ' || cchar == '\t'){
                    if(strstart){
                        temp[tempIndex++] = cchar;
                    }
                } else if(cchar == '\n'){
                    if(strstart && !hasnum){
                        cerr << "Error at line " << counter << "\n";
                        return;
                    }
                    counter++;
                } else if(cchar >= 32 && cchar <= 126){
                    if(bslash){
                        if(cchar == '\"' || cchar == '\\'){
                            bslash = false;
                            temp[tempIndex++] = cchar;
                            break;
                        } else {
                            cerr << "Error at line " << counter << "\n";
                            return;
                        }
                    }

                    if(!strstart){
                        if(cchar == '\"'){
                            strstart = true;
                            break;
                        } else {
                            cerr << "Error at line " << counter << "\n";
                            return;
                        }
                    } else {
                        if(cchar == '\"'){
                            strstart = false;
                            hasnum = true;
                            state = NUM;
                            temp[tempIndex] = '\0';
                            break;
                        }
                    }

                    if(cchar == '\\'){
                        bslash = true;
                        break;
                    }

                    temp[tempIndex++] = cchar;
                } else {
                    cerr << "Error at line " << counter << "\n";
                    return;
                }
                break;

            case NUM:
                if(cchar == ' ' || cchar == '\t'){
                    if(numstart){
                        cerr << "Error at line " << counter << "\n";
                        return;
                    }
                } else if(cchar == '\n'){
                    hasnum = false;
                    numstart = false;
                    state = STRING;
                    counter++;

                    tnum[tnumIndex] = '\0';
                    int i = stoi(tnum);

                    trie.insert(temp, i);
                    tempIndex = 0;
                    tnumIndex = 0;
                } else if((cchar >= '0' && cchar <= '9') ||cchar == '-'){
                    if(!numstart)
                        numstart = true;
                    tnum[tnumIndex++] = cchar;
                } else {
                    cerr << "Error at line " << counter << "\n";
                    return;
                }
                break;

            default:
                break;
        }
    }

    return;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Error: ./optimized <input_file>\n";
        return 1;
    }

    const char* fname = argv[1];
    int fd = open(fname, O_RDONLY);
    struct stat sb;

    if (fd == -1) {
        cerr << "Error: could not open file\n";
        return 1;
    }

    if (fstat(fd, &sb) == -1) {
        cerr << "Error: could not get file size\n";
        close(fd);
        return 1;
    }

    // Allocating memory for the input file
    char* filemem = static_cast<char*>(mmap(nullptr, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0));

    if (filemem == MAP_FAILED) {
        cerr << "Error: could not map file into memory\n";
        close(fd);
        return 1;
    }

    close(fd);

    auto start_tp = chrono::steady_clock::now();

    parseFile(filemem, sb.st_size);

    auto stop_tp = chrono::steady_clock::now();
    auto duration = chrono::duration<double>(stop_tp - start_tp);

    cout << "Elapsed time: " << duration.count() << endl;

    string temp = argv[1];
    size_t p3 = temp.find(".txt");
    string toopen = temp.substr(0, p3) + "-result.txt";

    trie.print(toopen);

    // Unmap the file from memory
    munmap(filemem, sb.st_size);

    return 0;
}