//
// Created by change_change_2 on 2024/1/23.
//

#include "BloomFilter.h"

bool BloomFilter::KeyMayMatch(const std::string &key, const std::string &dst) {
    const char *array = dst.data();
    std::hash<std::string> hashFunction;
    size_t h = hashFunction(key);
    const uint32_t delta = (h >> 17) | (h << 15);  // Rotate right 17 bits
    for (size_t j = 0; j < k_; j++) {
        const uint32_t bitpos = h % max_bf_size_;
        if ((array[bitpos / 8] & (1 << (bitpos % 8))) == 0) return false;
        h += delta;
    }
    return true;
}

void BloomFilter::insertKey(const std::string &key, std::string *dst) {
    std::hash<std::string> hashFunction;
    size_t h = hashFunction(key);
    const uint32_t delta = (h >> 17) | (h << 15);  // Rotate right 17 bits
    for (size_t j = 0; j < k_; j++) {
        const uint32_t bitpos = h % max_bf_size_;
        (*dst)[bitpos / 8] |= (1 << (bitpos % 8));
        h += delta;
    }
}

