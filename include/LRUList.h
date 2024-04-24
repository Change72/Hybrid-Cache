//
// Created by change_change_2 on 2024/1/23.
//

#ifndef HYBRIDCACHEEXAMPLE_LRULIST_H
#define HYBRIDCACHEEXAMPLE_LRULIST_H

#include <iostream>
#include <list>
#include <algorithm>
// LRUList is a list of blocks. KeyType is the blockid

class LRUList {
private:
    int capacity;
    std::list<int> lruList; // record data index

public:
    LRUList(int capacity) : capacity(capacity), lruList() {}

    void visit(int key) {
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

    int evict() {
        if (!lruList.empty()) {
            auto key = lruList.back();
            lruList.pop_back();
            return key;
        }
        return -1;
    }
};

#endif //HYBRIDCACHEEXAMPLE_LRULIST_H
