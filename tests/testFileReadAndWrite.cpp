//
// Created by chang on 3/19/24.
//
#include <iostream>
#include <fstream>
#include <cstring>
#include <unistd.h> // Include for getcwd()

using namespace std;

int main() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != nullptr) {
        cout << "Current working directory: " << cwd << endl;
    } else {
        cerr << "Failed to get current working directory." << endl;
        return 1;
    }

    const char* dataFileName = "example.txt";

    // if the file content length is less than offset (10), the file will be extended with null bytes
    const int offset = 10; // Example offset
    const char* newData = "updated data";

    std::ifstream infile(dataFileName);
    if (!infile.good()) {
        cerr << "The file does not exist." << endl;
        return 1;
    }

    // Open the file in both read and write mode
    // The file must exist
    fstream diskFile(dataFileName,  ios::in | ios::out | ios::binary);

    if (!diskFile.is_open()) {
        cerr << "Failed to open the file." << endl;
        return 1;
    }

    // Move the file pointer to the desired position
    diskFile.seekp(offset, ios::beg);

    // Write the new data at the specified position
    diskFile.write(newData, strlen(newData));

    // Close the file
    diskFile.close();

    cout << "Data updated successfully." << endl;

    return 0;
}