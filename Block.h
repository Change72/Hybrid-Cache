//
// Created by change_change_2 on 2024/1/23.
//

#ifndef HYBRIDCACHEEXAMPLE_BLOCK_H
#define HYBRIDCACHEEXAMPLE_BLOCK_H

#include <iostream>
#include <unordered_map>

#include "Configure.h"
#include "BloomFilter.h"

template <typename KeyType, typename ValueType>
class Block {
private:
    int id;
    int current_size;
    BloomFilter blockBloomFilter;


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

    bool full() const {
        return current_size == BLOCK_SIZE;
    }
};

#endif //HYBRIDCACHEEXAMPLE_BLOCK_H
