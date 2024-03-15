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
size_t counter;

enum State {
    Normal,
    StringContent,
    Escape
};

void parseFile(const char* filemem, size_t size) {
    int value;
    counter = 1;
    State state = Normal;

    for(size_t i = 0; i < size; i++){
        char c = filemem[i];

        switch(state) {
            case Normal:
                if(c == '\n'){
                    counter++;
                } else if(c == '\"'){
                    state = StringContent;
                } else if(c != ' ' && c != '\t'){
                    cerr << "Error at line " << counter << "\n";
                    return;
                }
                break;
                
            case StringContent:
                if(c == '\\'){
                    state = Escape;
                } else if(c == '\"'){
                    //calculates num
                    state = Normal;
                    i++;
                    while(i < size && (filemem[i] == ' ' || filemem[i] == '\t'))
                        i++;
                    value = 0;
                    if(i >= size || !(filemem[i] >= '0' && filemem[i] <= '9')){
                        cerr << "Error at line " << counter << "\n";
                        return;
                    }
                    while(i < size && filemem[i] >= '0' && filemem[i] <= '9'){
                        value = value * 10 + (filemem[i] - '0');
                        i++;
                    }
                    trie.insert(value);
                    i--;
                } else {
                    trie.insert(c);
                }
                break;

            case Escape:
                if(c != '\\' && c != '\"'){
                    cerr << "Error at line " << counter << "\n";
                    return;
                }
                trie.insert(c);
                state = StringContent;
                break;

            default:
                break;
        }
    }
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

    cout << "Lines per second: " << double(counter)/duration.count() << "\n";

    string temp = argv[1];
    size_t p3 = temp.find(".txt");
    string toopen = temp.substr(0, p3) + "-result.txt";

    trie.print(toopen);

    // Unmap the file from memory
    munmap(filemem, sb.st_size);

    return 0;
}