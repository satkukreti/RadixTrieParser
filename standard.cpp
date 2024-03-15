#include <string>
#include <iostream>
#include <map>
#include <fstream>
#include <chrono>
#include <unistd.h>

using namespace std;

map<string, unsigned int> mmap;

bool canPrint(char c){
    return c >= 32 && c <= 126;
}

bool isNum(char c){
    return c >= '0' && c <= '9';
}

bool wspace(char c){
    return c == ' ' || c == '\t';
}

int main(int argc, char *argv[]){
  if(argc != 2){
    cerr << "standard <inputFile>\n";
    return 1;
  }
  // Open the file
  ifstream file(argv[1]);

  // Check if the file is open
  if (!file.is_open()) {
      cerr << "Error opening file: " << argv[1] << endl;
      return 1;
  }

  auto start_tp = chrono::steady_clock::now();

  // Parse line
  int counter = 1;
  string line;
  while (getline(file, line)) {
    size_t p1 = line.find("\"");
    if(p1 != 0){
      for(size_t i = 0; i < p1; i++){
        if(!wspace(line[i])){
          cerr << "Error at line " << counter << "." << endl;
          return 1;
        }
      }
    }
    if(p1 == string::npos){
      cerr << "Error at line " << counter << "." << endl;
      return 1;
    }

    size_t p2 = line.rfind("\"");
    if(p2 == string::npos || p1 == p2){
      cerr << "Error at line " << counter << "." << endl;
      return 1;
    }

    string name;
    string temp;

    for(size_t i = p1+1; i < p2; i++){
      char cchar = line[i];

      if (cchar == '\\') {
        if (i + 1 < p2) {
            char nchar = line[i + 1];
            if (nchar == '\\' || nchar == '"') {
                name += nchar;
                i++;
            } else {
              cerr << "Error at line " << counter << "." << endl;
              return 1;
            }
        } else {
            cerr << "Error at line " << counter << "." << endl;
            return 1;
        }
      } else if (canPrint(cchar)) {
          name += cchar;
      } else {
        cerr << "Error at line " << counter << "." << endl;
        return 1;
      }
    }

    bool noNum = true;
    for(size_t i = p2+1; i < line.size(); i++){
      char cchar = line[i];
      
      if(wspace(cchar) && noNum){

      } else if(isNum(cchar)){
        temp += cchar;
        noNum = false;
      } else {
        cerr << "Error at line " << counter << "." << endl;
        return 1;
      }
    }

    unsigned int num = stoi(temp);
    unsigned int max = mmap[name];
    if(max < num){
      mmap[name] = num;
    }
    counter++;
  }

  // Close the file
  file.close();

  auto stop_tp = chrono::steady_clock::now();
  auto duration = chrono::duration<double>(stop_tp - start_tp);

  counter--;
  cout << "Lines per second: " << double(counter)/duration.count() << "\n";

  string temp = argv[1];
  size_t p3 = temp.find(".txt");
  string toopen = temp.substr(0, p3) + "-result.txt";
  ofstream outfile(toopen);

  if(outfile.is_open()){
    map<string, unsigned int>::iterator itr;
    for(itr = mmap.begin(); itr != mmap.end(); itr++){
      outfile << itr->first << " " << itr->second << endl;
    }
    outfile.close();
  } else {
    cerr << "Unable to create output file\n";
  }

  return 0;
}