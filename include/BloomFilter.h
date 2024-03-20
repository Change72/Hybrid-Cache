//
// Created by change_change_2 on 2024/1/23.
//

#ifndef HYBRIDCACHEEXAMPLE_BLOOMFILTER_H
#define HYBRIDCACHEEXAMPLE_BLOOMFILTER_H

#include <string>
#include <vector>

class BloomFilter {

public:
    BloomFilter(size_t bits_per_key = 10, size_t max_bf_size_ = 1024) : bits_per_key_(bits_per_key),
                                                                   max_bf_size_(max_bf_size_) {
        // We intentionally round down to reduce probing cost a little bit
        // k_ = static_cast<size_t>(bits_per_key * 0.69);  // 0.69 =~ ln(2)
        // e.g. bits_per_key = 10, k_ = 6, k_ is the iteration number of hash function
        k_ = static_cast<size_t>(bits_per_key * 0.69);  // 0.69 =~ ln(2)
        if (k_ < 1) k_ = 1;
        if (k_ > 30) k_ = 30;

    }
    // ~BloomFilter();

    bool KeyMayMatch(const std::string &key, const std::string &filter);

    void insertKey(const std::string &key, std::string *dst);

private:
    size_t bits_per_key_;
    size_t k_;
    size_t max_bf_size_;

};

#endif //HYBRIDCACHEEXAMPLE_BLOOMFILTER_H
