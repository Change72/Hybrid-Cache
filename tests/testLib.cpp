#include "HybridCache.h"
#include <iostream>
#include <cassert>

// Define test cases
void test_insert_and_retrieve() {
    HybridCache<int, std::string> cache;

    // Insert some key-value pairs
    cache.insert(1, "Value1");
    cache.insert(2, "Value2");
    cache.insert(3, "Value3");

    // Retrieve values and check
    assert(cache.get(1) == "Value1");
    assert(cache.get(2) == "Value2");
    assert(cache.get(3) == "Value3");

    std::cout << "Insert and retrieve test passed!" << std::endl;
}

int main() {
    // Run test cases
    test_insert_and_retrieve();
}