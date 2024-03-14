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

struct TrieNode {
    int children[ALPHABET_SIZE];
    bool end;
    int value;
    TrieNode() : end(false), value(INT_MIN){
        for(size_t i = 0; i < ALPHABET_SIZE; i++)
            children[i] = -1;
    }
};


class Trie {
    private:
        TrieNode nodes[MAX_NODES];
        int current;
        int nextNode;
    
    public:
        Trie() : current(0), nextNode(1) {}

        void insert(char c){
            int index = c - 32;
            if(nodes[current].children[index] == -1){
                nodes[current].children[index] = nextNode++;
            }

            current = nodes[current].children[index];
        }

        void insert(int value){
            nodes[current].end = true;
            if(value > nodes[current].value)
                nodes[current].value = value;
            
            current = 0;
        }

        void printhelp(int nodeIndex, vector<char>& cword, ofstream& outFile) {
            if(nodes[nodeIndex].end){
                for(char c : cword){
                    outFile << c;
                }
                outFile << " " << nodes[nodeIndex].value << "\n";
            }

            for(int i = 0; i < ALPHABET_SIZE; i++){
                if(nodes[nodeIndex].children[i] != -1){
                    cword.push_back(char(i + 32));
                    printhelp(nodes[nodeIndex].children[i], cword, outFile);
                    cword.pop_back();
                }
            }
        }

        void print(const string& filename) {
            ofstream outFile(filename);
            if(!outFile.is_open()){
                cerr << "Error opening file " << filename << "\n";
                return;
            }

            vector<char> cword;
            printhelp(0, cword, outFile);

            outFile.close();
        }
};

Trie trie;
int counter;

enum state {
    STRING,
    NUM
};

void parseFile(const char* filemem, size_t size){
    state state = STRING;
    counter = 1;

    bool strstart = false;
    bool numstart = false;
    bool bslash = false;
    bool hasnum = false;

    char tval[15];
    int tvalIndex = 0;

    for(size_t i = 0; i < size; i++){
        char cchar = filemem[i];

        switch(state){
            case STRING:
                if(cchar == ' ' || cchar == '\t'){
                    if(strstart){
                        trie.insert(cchar);
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
                            trie.insert(cchar);
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
                            break;
                        }
                    }

                    if(cchar == '\\'){
                        bslash = true;
                        break;
                    }

                    trie.insert(cchar);
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
                    tval[tvalIndex] = '\0';
                    int temp = stoi(tval);
                    tvalIndex = 0;
                    trie.insert(temp);
                } else if((cchar >= '0' && cchar <= '9') ||cchar == '-'){
                    if(!numstart)
                        numstart = true;
                    tval[tvalIndex++] = cchar;
                } else {
                    cerr << "Error at line " << counter << "\n";
                    return;
                }
                break;

            default:
                break;
        }
    }

    if(tvalIndex != 0){
        tval[tvalIndex] = '\0';
        int temp = stoi(tval);
        tvalIndex = 0;
        trie.insert(temp);
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

    counter--;
    cout << "Lines per second: " << double(counter)/duration.count() << "\n";

    string temp = argv[1];
    size_t p3 = temp.find(".txt");
    string toopen = temp.substr(0, p3) + "-result.txt";

    trie.print(toopen);

    // Unmap the file from memory
    munmap(filemem, sb.st_size);

    return 0;
}