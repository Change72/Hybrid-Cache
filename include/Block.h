//
// Created by change_change_2 on 2024/1/23.
//

#ifndef HYBRIDCACHEEXAMPLE_BLOCK_H
#define HYBRIDCACHEEXAMPLE_BLOCK_H

#include <iostream>
#include <unordered_map>
#include <string>
#include <utility>

#include "Configure.h"
#include "BloomFilter.h"
#include "utils.h"


template <typename KeyType, typename ValueType>
class Block {
private:
    std::unordered_map<KeyType, ValueType> data;

    // Overloaded function for types other than std::string
    template<typename T>
    static size_t getSize(const T& t, std::false_type) {
        return sizeof(T);
    }

    // Overloaded function for std::string
    static size_t getSize(const std::string& t, std::true_type) {
        return t.size();
    }

    // Dispatcher function to select appropriate overload
    template<typename T>
    static size_t getSize(const T& t) {
        return getSize(t, std::is_same<T, std::string>());
    }

public:
    Block() : data() {}

    bool get(const KeyType key, ValueType& value) {
        auto pos = data.find(key);
        if (pos != data.end()) {
            value = pos->second;
            return true;
        }
        return false;
    }

    void update(const KeyType key, const ValueType& value) {
        data[key] = value;
    }

    void put(const KeyType key, const ValueType& value) {
        data[key] = value;
    }

    void remove(const KeyType key) {
        data.erase(key);
    }

    // get data
    std::unordered_map<KeyType, ValueType> getData() const {
        return data;
    }

    static size_t getKeyTypeSize(const KeyType& key) {
        return getSize(key);
    }

    static size_t getValueTypeSize(const ValueType& value) {
        return getSize(value);
    }

    size_t getBlockSize() const {
        size_t size = 0;
        for (const auto& pair : data) {
            size += getKeyTypeSize(pair.first) + getValueTypeSize(pair.second);
        }
        return size;
    }

    void readBlockFromDisk(const std::string& dataFileName, const int& diskOffset, const int& keyNum) {
        std::fstream diskFile;
        // open the file and read the block
        diskFile.open(dataFileName, std::ios::in | std::ios::binary);

        diskFile.seekg(diskOffset);
        for (int i = 0; i < keyNum; ++i) {
            KeyType key;
            ValueType value;
            diskFile.read(reinterpret_cast<char*>(&key), sizeof(KeyType));
            diskFile.read(reinterpret_cast<char*>(&value), sizeof(ValueType));
            this->put(key, value);
        }

        diskFile.close();
    }

    void writeBlockToDisk(const std::string& dataFileName, const int& diskOffset) {
        std::fstream diskFile;
        // open the file and update the block, must use std::ios::in | std::ios::out | std::ios::binary
        diskFile.open(dataFileName, std::ios::in | std::ios::out | std::ios::binary);

        diskFile.seekp(diskOffset);
        for (auto& entry : this->data) {
            diskFile.write(reinterpret_cast<const char*>(&entry.first), sizeof(KeyType));
            diskFile.write(reinterpret_cast<const char*>(&entry.second), sizeof(ValueType));
        }

        diskFile.close();
    }
};


class BlockMetaInfo {
private:
    int id;
    size_t current_size_in_bytes;
    int current_key_num;

    bool blockIsInDisk;
    bool blockIsUpdated; // if the block is updated after read from disk

    // on disk offset in bytes
    int diskOffset = -1;

    // only can insert key
    std::string bf_dst;

public:
    BlockMetaInfo() {}

    BlockMetaInfo(int id, size_t current_size_in_bytes, int current_key_num, int diskOffset, std::string  bf_dst) :
            id(id), current_size_in_bytes(current_size_in_bytes), current_key_num(current_key_num), diskOffset(diskOffset), bf_dst(std::move(bf_dst)) {
        blockIsInDisk = true;
        blockIsUpdated = false;
    }

    int getId() const {
        return id;
    }

    void setId(int id) {
        this->id = id;
    }

    size_t getCurrentSize() const {
        return current_size_in_bytes;
    }

    void setCurrentSize(size_t size) {
        current_size_in_bytes = size;
    }

    int getKeyNum() const {
        return current_key_num;
    }

    void setKeyNum(int keyNum) {
        current_key_num = keyNum;
    }

    bool getBlockIsInDisk() const {
        return blockIsInDisk;
    }

    void setBlockIsInDisk(bool isInDisk) {
        blockIsInDisk = isInDisk;
    }

    bool getBlockIsUpdated() const {
        return blockIsUpdated;
    }

    void setBlockIsUpdated(bool isUpdated) {
        blockIsUpdated = isUpdated;
    }

    int getDiskOffset() const {
        return diskOffset;
    }

    void setDiskOffset(int offset) {
        diskOffset = offset;
    }

    bool full() const {
        return current_size_in_bytes == BLOCK_SIZE;
    }

    int remainingSize() const {
        return BLOCK_SIZE - current_size_in_bytes;
    }

    void increaseCurrentSize(size_t size) {
        current_size_in_bytes += size;
    }

    void increaseCurrentKeyNum(int keyNum) {
        current_key_num += keyNum;
    }

    std::string getBlockBloomFilter() const {
        return bf_dst;
    }

    void setBlockBloomFilter(const std::string& bf_dst) {
        this->bf_dst = bf_dst;
    }

    static void loadBlockMetaData(const std::string& metadataFileName, std::unordered_map<int, BlockMetaInfo>& blocks_metadata) {
        if (fileIsExist(metadataFileName)) {
            std::fstream metadataFile;
            metadataFile.open(metadataFileName, std::ios::in | std::ios::binary);
            int block_id;
            size_t current_size_in_bytes;
            int current_key_num;
            int disk_offset;
            std::string bf_src;
            while (metadataFile.read(reinterpret_cast<char*>(&block_id), sizeof(int))) {
                metadataFile.read(reinterpret_cast<char*>(&current_size_in_bytes), sizeof(current_size_in_bytes));
                metadataFile.read(reinterpret_cast<char*>(&current_key_num), sizeof(int));
                metadataFile.read(reinterpret_cast<char*>(&disk_offset), sizeof(int));

                // read block bloom filter length and block bloom filter
                size_t bf_length;
                metadataFile.read(reinterpret_cast<char*>(&bf_length), sizeof(bf_length));
                bf_src.resize(bf_length);
                metadataFile.read(&bf_src[0], bf_length);

                blocks_metadata[block_id] = BlockMetaInfo(block_id, current_size_in_bytes, current_key_num, disk_offset, bf_src);
            }
            metadataFile.close();
        }
    }

    static void flushBlockMetaData(const std::string& metadataFileName, const std::unordered_map<int, BlockMetaInfo>& blocks_metadata) {
        std::fstream metadataFile;
        metadataFile.open(metadataFileName, std::ios::out | std::ios::binary);
        for (auto& metadata : blocks_metadata) {
            int block_id = metadata.second.getId();
            metadataFile.write(reinterpret_cast<char*>(&block_id), sizeof(int));

            size_t current_size_in_bytes = metadata.second.getCurrentSize();
            metadataFile.write(reinterpret_cast<char*>(&current_size_in_bytes), sizeof(current_size_in_bytes));

            int current_key_num = metadata.second.getKeyNum();
            metadataFile.write(reinterpret_cast<char*>(&current_key_num), sizeof(int));

            int disk_offset = metadata.second.getDiskOffset();
            metadataFile.write(reinterpret_cast<char*>(&disk_offset), sizeof(int));

            // write block bloom filter length and block bloom filter
            size_t bf_length = metadata.second.getBlockBloomFilter().size();
            metadataFile.write(reinterpret_cast<char*>(&bf_length), sizeof(bf_length));

            std::string bf_src = metadata.second.getBlockBloomFilter();
            metadataFile.write(bf_src.c_str(), bf_length);
        }
        metadataFile.close();
    }
};

#endif //HYBRIDCACHEEXAMPLE_BLOCK_H
