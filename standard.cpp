#include <string>
#include <iostream>
#include <map>
#include <fstream>

using namespace std;

map<string, unsigned int> mmap;

int main(int argv, char *argc[]){
  if(argv != 3){
    cerr << "standard <inputFile> <outputFile>\n";
    return 1;
  }
  // Open the file
  ifstream file(argc[1]);

  // Check if the file is open
  if (!file.is_open()) {
      cerr << "Error opening file: " << argc[1] << endl;
      return 1;
  }

    // Parse line
    string line;
    while (getline(file, line)) {
      size_t p1 = line.find("\"");
      if(p1 == string::npos){
        cerr << "invalid input, no string detected\n";
	return 1;
      }
      size_t p2 = line.rfind("\"");
      if(p2 == string::npos || p1 == p2){
	cerr << "invalid input, string not formatted properly\n";
	return 1;
      }

      string input = line.substr(p1, p2-p1+1);
      string temp = line.substr(p2+1);
      int num = stoi(temp);
      
      cout << num << endl;
    }

    // Close the file
    file.close();

    return 0;
}
