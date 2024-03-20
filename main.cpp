//
// Created by chang on 2/6/24.
//
#include <list>

template <typename KeyType>
class LRUList {
private:
    int capacity;
    std::list<KeyType> lruList; // record data index

public:
    LRUList(int capacity) : capacity(capacity), lruList() {}
    // Rest of your class implementation...
};

int main() {
    LRUList<int> lru(10); // Example with a capacity of 10
    // Rest of your code...

    return 0;
}

