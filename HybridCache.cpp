#include <iostream>
#include <unordered_map>
#include <list>
#include <fstream>
#include <string>
#include <sstream>

const int MEMORY_CACHE_SIZE = 4 * 1024 * 1024;  // 10 MB
const int DISK_CACHE_SIZE = 10 * 1024 * 1024;   // 10 MB
const int BLOCK_SIZE = 2 * 1024 * 1024;           // 2 MB

template <typename KeyType>
class LRUList {
private:
    int capacity;
    std::list<KeyType> lruList; // record data index

public:
    LRUList(int capacity) : capacity(capacity) {}

    void visit(const KeyType key) {
        auto pos = std::find(lruList.begin(), lruList.end(), key);
        if (pos != lruList.end()) {
            lruList.erase(pos);
        }
        lruList.push_front(key);
        if (lruList.size() > capacity) {
            lruList.pop_back();
        }
    }

    int getCapacity() const {
        return capacity;
    }

    bool full() const {
        return lruList.size() == capacity;
    }

    KeyType evict() {
        if (!lruList.empty()) {
            auto key = lruList.back();
            lruList.pop_back();
            return key;
        }
    }
};

template <typename KeyType, typename ValueType>
class Block {
private:
    int id;
    int current_size;
    std::unordered_map<KeyType, ValueType> data;

public:
    Block(int id) : id(id), current_size(0){}

    bool get(const KeyType key, ValueType& value) {
        auto pos = data.find(key);
        if (pos != data.end()) {
            value = pos->second;
            return true;
        }
        return false;
    }

    bool put(const KeyType key, const ValueType& value) {
        if (current_size < BLOCK_SIZE) {
            data[key] = value;
            current_size += sizeof(key) + sizeof(value);
            return true;
        }
        return false;
    }

    int getCapacity() const {
        return current_size;
    }

};



template <typename KeyType, typename ValueType>
class HybridCache {
private:
    std::list<Block<KeyType, ValueType>> memoryBlocks; // include only memory blocks
    LRUList<int> lruMemoryList; // record data index
    LRUList<int> lruDiskList; // record data index
    std::fstream diskFile;
    int currentBlockIndex;
    // data index on disk mapping
    std::unordered_map<int, int> dataIndexMap;
    // block floom filter
    // overall big hash table
    // key can be selective to different blocks
    // from disk, block compaction
public:
    HybridCache() : lruMemoryList(MEMORY_CACHE_SIZE / BLOCK_SIZE),
                    lruDiskList(DISK_CACHE_SIZE / BLOCK_SIZE),
                    currentBlockIndex(0),
                    diskFile("hybrid_cache.dat", std::ios::in | std::ios::out | std::ios::binary) {
        if (!diskFile) {
            diskFile.open("hybrid_cache.dat", std::ios::out | std::ios::binary);
            diskFile.seekp(DISK_CACHE_SIZE - 1);
            diskFile.write("", 1);
            diskFile.close();
            diskFile.open("hybrid_cache.dat", std::ios::in | std::ios::out | std::ios::binary);
        }
        for (int i = 0; i < DISK_CACHE_SIZE / BLOCK_SIZE; ++i) {
            dataIndexMap[i] = -1;
        }
    }

    void insertEmptyBlock() {
        if (memoryBlocks.size() >= MEMORY_CACHE_SIZE / BLOCK_SIZE) {
            diskToMemory();
        }
        auto block = Block<KeyType, ValueType>(currentBlockIndex);
        memoryBlocks.emplace_back(currentBlockIndex);
        ++currentBlockIndex;
    }

    void memoryToDisk() {

    }

    void diskToMemory() {
        // write data
        //
        for (int i = 0; i < lruList.getCapacity(); ++i) {
            Blocks.emplace_back(i);
            diskFile.seekg(static_cast<std::streamoff>(i) * BLOCK_SIZE);
            for (int j = 0; j < BLOCK_SIZE; j += sizeof(KeyType) + sizeof(ValueType)) {
                KeyType key;
                ValueType value;
                diskFile.read(reinterpret_cast<char*>(&key), sizeof(KeyType));
                diskFile.read(reinterpret_cast<char*>(&value), sizeof(ValueType));
                Blocks.back().put(key, value);
            }
        }
    }


    // if set/get first visit to provent target been evicted

#TODO how to get key and correspondign data index
    bool get(const KeyType key, ValueType& value) {
        int dataIndex = key / BLOCK_SIZE;
        if (dataIndex == currentBlockIndex) {
            if (Blocks.back().get(key, value)) {
//                lruList.visit(dataIndex);
                return true;
            }
        } else {
            for (auto& data : Blocks) {
                if (data.get(key, value)) {
//                    lruList.visit(dataIndex);
                    return true;
                }
            }
        }
        return false;
    }

    bool put(const KeyType key, const ValueType& value) {

    }


    void flush_data() {
        // evict by lru
        for (auto& data : Blocks) {
            writeToDisk(data);
        }
    }

    void evict(){
        // evict by lru
        for (auto& data : Blocks) {
            writeToDisk(data);
        }
    }


    void writeToDisk(int dataIndex, int offset) {
        diskFile.seekp(offset * BLOCK_SIZE);
        auto data = Blocks[dataIndex];
        for (auto key)
    }
};

// Simple testing code
int main() {
    HybridCache<int, std::string> cache;

    // Populate the cache
    for (int i = 0; i < 50; ++i) {
        std::stringstream value;
        value << "Value " << i;
        cache.put(i, value.str());
    }

    // Access data from the cache
    for (int i = 0; i < 50; ++i) {
        std::cout << "Key: " << i << ", Value: " << cache.get(i) << std::endl;
    }

    return 0;
}