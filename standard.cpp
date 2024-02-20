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

  // Check if the file is successfully opened
  if (!file.is_open()) {
      cerr << "Error opening file: " << argc[1] << endl;
      return 1;
  }

    // Read and parse the file line by line
    string line;
    while (getline(file, line)) {
        // Process each line as needed
       cout << "Line: " << line << endl;
    }

    // Close the file
    file.close();

    return 0;
}
