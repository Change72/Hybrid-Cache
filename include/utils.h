//
// Created by chang on 3/5/24.
//

#ifndef HYBRIDCACHEEXAMPLE_UTILS_H
#define HYBRIDCACHEEXAMPLE_UTILS_H

#include <random>
#include <fstream>

int getRandomInt() {
    std::random_device rd; // Obtain a random seed from the hardware
    std::mt19937 eng(rd()); // Standard mersenne_twister_engine seeded with rd()

    // Define the distribution
    std::uniform_int_distribution<int> distr(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());

    // Generate a random integer
    return distr(eng);
}

// initialize the disk file with all 0s
void initializeDiskFile(const std::string& dataFileName) {
    std::ofstream diskFile;
    diskFile.open(dataFileName, std::ios::out | std::ios::binary);
    diskFile.seekp(DISK_CACHE_SIZE - 1);
    diskFile.write("", 1);
    diskFile.close();
}

bool fileIsExist(const std::string& filename) {
    std::ifstream infile(filename);
    return infile.good();
}

bool diskFileIsFull(const std::string& dataFileName) {
    // check if the disk file is existing
    if (!fileIsExist(dataFileName)) {
        return false;
    }
    std::fstream diskFile;
    diskFile.open(dataFileName, std::ios::in | std::ios::binary);
    return diskFile.tellp() >= DISK_CACHE_SIZE;
}

// find available slot in the disk
int findAvailableSlotInDisk(const std::string& dataFileName) {
    std::fstream diskFile;
    diskFile.open(dataFileName, std::ios::in | std::ios::binary);
    int offset = 0;
    while (diskFile.tellp() < DISK_CACHE_SIZE) {
        diskFile.seekp(offset);
        int c;
        diskFile.read(reinterpret_cast<char*>(&c), sizeof(int));
        if (c == 0) {
            return offset;
        }
        offset += BLOCK_SIZE;
    }
    // todo if the disk is full, we need to evict some blocks. Now we need to manually 'rm hybrid.*' if full
    // in disk lru list
    return -1;
}

#endif //HYBRIDCACHEEXAMPLE_UTILS_H
