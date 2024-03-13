#include <cstring>
#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

using namespace std;

const int ALPHABET_SIZE = 95;
const int MAX_NODES = 10000;

class TrieNode {
public:
    int children[ALPHABET_SIZE];
    bool end;

    TrieNode() : end(false) {
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

        void insert(string str){
            int current = 0;

            for (char c : str) {
                int idx = c - 32;
                if (nodes[current].children[idx] == -1) {
                    nodes[current].children[idx] = nextNode++;
                }

                current = nodes[current].children[idx];
            }

            nodes[current].end = true;
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

        void printhelp(int nodeIndex, vector<char>& cword) {
            if (nodes[nodeIndex].end) {
                for (char c : cword) {
                    cout << c;
                }
                cout << endl;
            }

            for (int i = 0; i < ALPHABET_SIZE; ++i) {
                if (nodes[nodeIndex].children[i] != -1) {
                    cword.push_back(static_cast<char>(i + 32));
                    printhelp(nodes[nodeIndex].children[i], cword);
                    cword.pop_back();
                }
            }
        }

        void print() {
            vector<char> cword;
            printhelp(0, cword);
        }
};

Trie trie;

enum state {
    STRING,
    NUM
};

bool canPrint(char c){
    return c >= 32 && c <= 126;
}

bool isNum(char c){
    return c >= '0' && c <= '9';
}

bool wspace(char c){
    return c == ' ' || c == '\t';
}

bool nline(char c){
    return c == '\n';
}

bool parseFile(const char* str, size_t size){
    state state = STRING;
    int counter = 1;

    bool strstart = false;
    bool numstart = false;
    bool bslash = false;
    bool hasnum = false;


    string temp = "";

    for(size_t i = 0; i < size; i++){
        char cchar = str[i];

        switch(state){
            case STRING:
                if(wspace(cchar)){
                    if(strstart){
                        temp += cchar;
                    }
                } else if(nline(cchar)){
                    if(strstart && !hasnum){
                        cerr << "Error at line " << counter << "\n";
                        return false;
                    }
                    counter++;
                } else if(canPrint(cchar)){
                    if(bslash){
                        if(cchar == '\"' || cchar == '\\'){
                            bslash = false;
                            temp += cchar;
                            break;
                        } else {
                            cerr << "Error at line " << counter << "\n";
                            return false;
                        }
                    }

                    if(!strstart){
                        if(cchar == '\"'){
                            strstart = true;
                            break;
                        } else {
                            cerr << "Error at line " << counter << "\n";
                            return false;
                        }
                    } else {
                        if(cchar == '\"'){
                            strstart = false;
                            hasnum = true;
                            trie.insert(temp);
                            state = NUM;
                            break;
                        }
                    }

                    if(cchar == '\\'){
                        bslash = true;
                        break;
                    }

                    temp += cchar;
                } else {
                    cerr << "Error at line " << counter << "\n";
                    return false;
                }
                break;

            case NUM:
                if(wspace(cchar)){
                    if(numstart){
                        cerr << "Error at line " << counter << "\n";
                        return false;
                    }
                } else if(nline(cchar)){
                    hasnum = false;
                    numstart = false;
                    state = STRING;
                    counter++;
                    //trie.insert(temp);
                    temp = "";
                } else if(isNum(cchar)){
                    numstart = true;
                    temp += cchar;
                } else if(cchar == '-'){
                    temp += cchar;
                } else {
                    cerr << "Error at line " << counter << "\n";
                    return false;
                }

            default:
                break;
        }
    }

    return true;
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

    if(parseFile(filemem, sb.st_size)){
        trie.print();
    } else {

    }

    // Unmap the file from memory
    munmap(filemem, sb.st_size);
    close(fd);

    return 0;
}