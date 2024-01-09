#include <iostream>
#include <unordered_map>
#include <list>
#include <fstream>
#include <string>
#include <sstream>

const int MEMORY_CACHE_SIZE = 4 * 1024 * 1024;  // 10 MB
const int DISK_CACHE_SIZE = 10 * 1024 * 1024;   // 10 MB
const int BLOCK_SIZE = 2 * 1024 * 1024;           // 2 MB

template <typename KeyType, typename ValueType>
class LRUCache {
private:
    int capacity;
    std::list<KeyType> lruList;
    std::unordered_map<KeyType, std::pair<ValueType, typename std::list<KeyType>::iterator>> cache;

public:
    LRUCache(int capacity) : capacity(capacity) {}

    void set(const KeyType key, const ValueType value) {
        auto pos = cache.find(key);
        if (pos == cache.end()) {
            lruList.push_front(key);
            cache[key] = { value, lruList.begin() };
            if (cache.size() > capacity) {
                cache.erase(lruList.back());
                lruList.pop_back();
            }
        }
        else {
            lruList.erase(pos->second.second);
            lruList.push_front(key);
            cache[key] = { value, lruList.begin() };
        }
    }

    bool get(const KeyType key, ValueType &value) {
        auto pos = cache.find(key);
        if (pos == cache.end())
            return false;
        lruList.erase(pos->second.second);
        lruList.push_front(key);
        cache[key] = { pos->second.first, lruList.begin() };
        value = pos->second.first;
        return true;
    }

    int getCapacity() const {
        return capacity;
    }

    void evict() {
        if (!lruList.empty()) {
            KeyType last = lruList.back();
            lruList.pop_back();
            cache.erase(last);
        }
    }
};

template <typename KeyType, typename ValueType>
class HybridCache {
private:
    std::unordered_map<int, LRUCache<KeyType, ValueType>> memoryBlocks;
    LRUCache<KeyType, ValueType> onDiskCache;
    std::fstream diskFile;
    int currentMemoryBlock;

public:
    HybridCache() : onDiskCache(DISK_CACHE_SIZE / BLOCK_SIZE),
                    diskFile("hybrid_cache.dat", std::ios::in | std::ios::out | std::ios::binary) {
        if (!diskFile) {
            diskFile.open("hybrid_cache.dat", std::ios::out | std::ios::binary);
            diskFile.seekp(DISK_CACHE_SIZE - 1);
            diskFile.write("", 1);
            diskFile.close();
            diskFile.open("hybrid_cache.dat", std::ios::in | std::ios::out | std::ios::binary);
        }
    }

    ValueType get(KeyType key) {
        if (memoryBlocks.empty()) {
            return onDiskCache.get(key);
        } else {
            for (auto& block : memoryBlocks) {
                ValueType value = block.second.get(key);
                if (value != ValueType()) {
                    return value;
                }
            }
            return onDiskCache.get(key);
        }
    }

    void put(KeyType key, const ValueType& value) {
        if (memoryBlocks.empty() || memoryBlocks[currentMemoryBlock].getCapacity() == 0) {
            // Create a new memory block
            memoryBlocks.emplace(currentMemoryBlock, LRUCache<KeyType, ValueType>(MEMORY_CACHE_SIZE / BLOCK_SIZE));
        }

        // Insert into the current memory block
        memoryBlocks[currentMemoryBlock].refer(key, value);

        // Write to disk if the current memory block is full
        if (memoryBlocks[currentMemoryBlock].getCapacity() == 0) {
            writeToDisk(currentMemoryBlock);
            onDiskCache.evict(); // Evict from on-disk cache after writing to disk
            ++currentMemoryBlock;
        }
    }

    void writeToDisk(int blockIndex) {
        for (const auto& entry: memoryBlocks[blockIndex]) {
            diskFile.seekp(static_cast<std::streamoff>(entry.first) * BLOCK_SIZE);
            diskFile.write(reinterpret_cast<const char*>(&entry.second.first), sizeof(ValueType));
        }
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