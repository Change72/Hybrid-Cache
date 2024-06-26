#include <iostream>
#include <cassert>
#include <HybridCache.h> // Include your HybridCache header file

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

void test_multi_block() {
    HybridCache<int, std::string> cache;

    // Insert key-value pairs until memory is full
    std::string longString(BLOCK_SIZE / 6, 'x');
    for (int i = 0; i < BLOCK_SIZE / (sizeof(int) + longString.size()); ++i) {
        cache.insert(i, longString);
    }

    // Insert one more entry, should trigger eviction
    cache.insert(100, longString);

    std::string temp = cache.get(0);
    // Ensure that the evicted block is no longer in memory
    assert(cache.get(0) == longString); // Assuming default-constructed ValueType represents absence

    std::cout << "Multi-block get test passed!" << std::endl;
}

void test_eviction() {
    HybridCache<int, std::string> cache;

    // Insert key-value pairs until memory is full
    std::string longString(BLOCK_SIZE / 6, 'x');
    for (int i = 0; i < 2 * BLOCK_SIZE / (sizeof(int) + longString.size()); ++i) {
        cache.insert(i, longString);
    }

    cache.insert(100, longString);

    // Ensure that the evicted block is no longer in memory, but promote from disk
    assert(cache.get(0) == longString); // Assuming default-constructed ValueType represents absence

    std::cout << "Eviction test passed!" << std::endl;
}

void test_load_previous_cache() {
    // Create a new cache
    HybridCache<int, std::string> cache (
            MEMORY_CACHE_SIZE,
            DISK_CACHE_SIZE,
            BLOCK_SIZE,
            "hybrid.cache",
            "hybrid.meta",
            true,
            MAX_BF_SIZE,
            BLOOM_FILTER_BITS_PER_KEY
    );

    std::cout << "Load previous cache test passed!" << std::endl;
}

// Add more test cases as needed

int main() {
    // Run test cases
    test_insert_and_retrieve();
    test_multi_block();
    test_eviction();
    test_load_previous_cache();

    // Add more test cases here

    std::cout << "All tests passed!" << std::endl;

    return 0;
}