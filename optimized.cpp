#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cctype>

using namespace std;

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

    //Get file input, line by line
    size_t currentPos = 0;
    while (currentPos < (sb.st_size)) {
        char* line = filemem + currentPos;
        size_t lineLength = strcspn(line, "\n");

        cout.write(line, lineLength+1);
        cout << endl;

        //moves on to the next line
        currentPos += lineLength + 1;
    }

    // Unmap the file from memory
    munmap(filemem, sb.st_size);
    close(fd);

    return 0;
}