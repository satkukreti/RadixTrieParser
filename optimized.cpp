#include <cstring>
#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

using namespace std;

vector<string> vstring;

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
    bool bslash = false;
    bool hasnum = false;

    string temp = "";

    for(size_t i = 0; i < size; i++){
        char cchar = str[i];

        switch(state){
            case STRING:
                if(wspace(cchar)){

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
                            temp += cchar;
                            break;
                        } else {
                            cerr << "Error at line " << counter << "\n";
                            return false;
                        }
                    } else {
                        if(cchar == '\"'){
                            strstart = false;
                            hasnum = true;
                            temp += cchar;
                            state = NUM;
                            break;
                        }
                    }

                    if(cchar == '\\'){
                        bslash = true;
                        temp += cchar;
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

                } else if(nline(cchar)){
                    hasnum = false;
                    state = STRING;
                    counter++;
                    vstring.push_back(temp);
                    temp = "";
                } else if(isNum(cchar)){
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

    cout << counter << endl;

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
        for(string s: vstring){
            cout << s << "\n";
        }
    } else {

    }

    // Unmap the file from memory
    munmap(filemem, sb.st_size);
    close(fd);

    return 0;
}