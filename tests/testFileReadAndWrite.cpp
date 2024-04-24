//
// Created by chang on 3/19/24.
//
#include <iostream>
#include <fstream>
#include <cstring>
#include <unistd.h> // Include for getcwd()

using namespace std;

void test_write_and_read_int() {
    const char* dataFileName = "example.txt";

    // write an int to the file
    int data = 123;
    ofstream outfile(dataFileName, ios::binary);
    if (!outfile.good()) {
        cerr << "Failed to open the file." << endl;
        return;
    }
    outfile.write(reinterpret_cast<const char*>(&data), sizeof(data));
    outfile.close();

    cout << "Data written successfully." << endl;

    // read the int from the file
    int readData;
    ifstream infile_int(dataFileName, ios::binary);
    if (!infile_int.good()) {
        cerr << "The file does not exist." << endl;
        return;
    }
    infile_int.read(reinterpret_cast<char*>(&readData), sizeof(readData));
    infile_int.close();

    cout << "Data read successfully: " << readData << endl;
}

void test_write_and_read_size_t() {
    const char* dataFileName = "example.txt";

    // write a size_t to the file
    size_t data = 1234567890;
    ofstream outfile(dataFileName, ios::binary);
    if (!outfile.good()) {
        cerr << "Failed to open the file." << endl;
        return;
    }
    outfile.write(reinterpret_cast<const char*>(&data), sizeof(data));
    outfile.close();

    cout << "Data written successfully." << endl;

    // read the size_t from the file
    size_t readData;
    ifstream infile_size_t(dataFileName, ios::binary);
    if (!infile_size_t.good()) {
        cerr << "The file does not exist." << endl;
        return;
    }
    infile_size_t.read(reinterpret_cast<char*>(&readData), sizeof(readData));
    infile_size_t.close();

    cout << "Data read successfully: " << readData << endl;
}

int main() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != nullptr) {
        cout << "Current working directory: " << cwd << endl;
    } else {
        cerr << "Failed to get current working directory." << endl;
        return 1;
    }

    const char* dataFileName = "example.txt";

    test_write_and_read_size_t();

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