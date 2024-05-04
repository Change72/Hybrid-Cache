#ifndef HYBRIDCACHEEXAMPLE_HYBRIDCACHE_H
#define HYBRIDCACHEEXAMPLE_HYBRIDCACHE_H

#include <iostream>
#include <unordered_map>
#include <string>

#include "BloomFilter.h"
#include "LRUList.h"
#include "Block.h"

template <typename KeyType, typename ValueType>
class HybridCache {
private:
    std::unordered_map<int, BlockMetaInfo> blocks_metadata;
    std::unordered_map<int, Block<KeyType, ValueType>> blocks_inmemory; // include only memory blocks

    LRUList lruMemoryList;
    LRUList lruDiskList; // record data index

    std::string dataFileName;
    std::string metadataFileName;

    std::unordered_map<int, int> diskOffsetToBlockId;

    BloomFilter blockBloomFilter;
    bool loadPreviousResult;

public:
    HybridCache() : lruMemoryList(MEMORY_CACHE_SIZE / BLOCK_SIZE),
                    lruDiskList(DISK_CACHE_SIZE / BLOCK_SIZE),
                    dataFileName("hybrid.cache"),
                    metadataFileName("hybrid.meta"),
                    loadPreviousResult(false),
                    blockBloomFilter(BLOOM_FILTER_BITS_PER_KEY, MAX_BF_SIZE) {
        if (loadPreviousResult) {
            BlockMetaInfo::loadBlockMetaData(metadataFileName, blocks_metadata);
            for (auto &metadata: blocks_metadata) {
                diskOffsetToBlockId[metadata.second.getDiskOffset()] = metadata.first;
            }
        }

        if (!fileIsExist(dataFileName)) {
            initializeDiskFile(dataFileName);
        }
    }

    // New constructor with parameters
    HybridCache(
            size_t memoryCacheSize,
            size_t diskCacheSize,
            size_t blockSize,
            const std::string& dataFile,
            const std::string& metadataFile,
            bool loadPrevResult,
            int bloomFilterBitsPerKey,
            int maxBloomFilterSize
    ) : lruMemoryList(memoryCacheSize / blockSize),
        lruDiskList(diskCacheSize / blockSize),
        dataFileName(dataFile),
        metadataFileName(metadataFile),
        loadPreviousResult(loadPrevResult),
        blockBloomFilter(bloomFilterBitsPerKey, maxBloomFilterSize) {
        if (loadPreviousResult) {
            BlockMetaInfo::loadBlockMetaData(metadataFileName, blocks_metadata);
            for (auto &metadata: blocks_metadata) {
                diskOffsetToBlockId[metadata.second.getDiskOffset()] = metadata.first;
            }
        }

        if (!fileIsExist(dataFileName)) {
            initializeDiskFile(dataFileName);
        }
    }

    ~HybridCache() {
        BlockMetaInfo::flushBlockMetaData(metadataFileName, blocks_metadata);
    }

    void createBlockBloomFilterByKeys(const std::vector<std::string> &keys, std::string &bf_dst) {
        for (const auto &key : keys) {
            blockBloomFilter.insertKey(key, &bf_dst);
        }
    }

    bool insertEntryAvailable(const int block_id, const KeyType& key, const ValueType& value) {
        if (blocks_metadata[block_id].getCurrentSize() + Block<KeyType, ValueType>::getKeyTypeSize(key) +
            Block<KeyType, ValueType>::getValueTypeSize(value) > BLOCK_SIZE) {
            return false;
        }
        return true;
    }

    void evictBlockFromMemory() {
        auto block_id = lruMemoryList.evict();
        auto &block = blocks_inmemory[block_id];
        // if the block is from disk and updated, write it back to disk
        if (blocks_metadata[block_id].getBlockIsInDisk() && blocks_metadata[block_id].getBlockIsUpdated()) {
            block.writeBlockToDisk(dataFileName, blocks_metadata[block_id].getDiskOffset());
        }

        // if the block is a new block, write it to disk
        if (!blocks_metadata[block_id].getBlockIsInDisk()) {
            int offset = findAvailableSlotInDisk(dataFileName);
            //

            block.writeBlockToDisk(dataFileName, offset);
            blocks_metadata[block_id].setDiskOffset(offset);
            blocks_metadata[block_id].setBlockIsInDisk(true);
        }

        blocks_metadata[block_id].setBlockIsUpdated(false);
        blocks_inmemory.erase(block_id);
    }

    int insertNewBlock() {
        if (lruMemoryList.full()) {
            evictBlockFromMemory();
        }

        // create a new block and initial the block metadata
        auto block = Block<KeyType, ValueType>();
        int block_id = getRandomInt();
        blocks_inmemory[block_id] = block;
        std::string bf_dst;
        bf_dst.resize(MAX_BF_SIZE);
        blocks_metadata[block_id] = BlockMetaInfo(block_id, 0, 0, -1, bf_dst);
        blocks_metadata[block_id].setBlockIsInDisk(false);

        lruMemoryList.visit(block_id);
        return block_id;
    }

    void insert(KeyType key, ValueType value) {
        // check if the key is in the memory, if yes, update the value
        for (auto &metadata : blocks_metadata) {
            if (blockBloomFilter.KeyMayMatch(std::to_string(key), metadata.second.getBlockBloomFilter())) {
                auto &block = blocks_inmemory[metadata.second.getId()];
                // if the block is in the disk, read it to memory
                if (metadata.second.getBlockIsInDisk()) {
                    if (lruMemoryList.full()) {
                        evictBlockFromMemory();
                    }
                    block.readBlockFromDisk(dataFileName, metadata.second.getDiskOffset(), metadata.second.getKeyNum());
                    metadata.second.setBlockIsInDisk(false);
                }
                // if the key is in the block, update the value
                ValueType originalValue;
                block.get(key, originalValue);
                if (originalValue != value) {
                    block.update(key, value);
                    metadata.second.setBlockIsUpdated(true);
                }
                lruMemoryList.visit(metadata.second.getId());
                return;
            }
        }

        // if the key is not in the memory, check if there is available block to insert the key-value pair
        int availBlockId = -1;
        for (auto &block : blocks_inmemory) {
            if (insertEntryAvailable(block.first, key, value)) {
                availBlockId = block.first;
                break;
            }
        }
        if (availBlockId == -1) {
            availBlockId = insertNewBlock();
        }


        auto &block = blocks_inmemory[availBlockId];
        block.put(key, value);
        blocks_metadata[availBlockId].increaseCurrentSize(Block<KeyType, ValueType>::getKeyTypeSize(key) +
                                                          Block<KeyType, ValueType>::getValueTypeSize(value));
        std::string bf_dst = blocks_metadata[availBlockId].getBlockBloomFilter();
        blockBloomFilter.insertKey(std::to_string(key), &bf_dst);
        blocks_metadata[availBlockId].setBlockBloomFilter(bf_dst);

        blocks_metadata[availBlockId].increaseCurrentKeyNum(1);
        blocks_metadata[availBlockId].setBlockIsUpdated(true);
        lruMemoryList.visit(availBlockId);
    }

    int keyExist(KeyType key) {
        for (auto &metadata : blocks_metadata) {
            if (blockBloomFilter.KeyMayMatch(std::to_string(key), metadata.second.getBlockBloomFilter())) {
                if (metadata.second.getBlockIsInDisk())
                    return 1; // the key is in the disk
                else
                    return 0; // the key is in the memory
            }
        }
        return -1;
    }

    // get
    ValueType get(KeyType key) {
        for (auto &metadata : blocks_metadata) {
            if (blockBloomFilter.KeyMayMatch(std::to_string(key), metadata.second.getBlockBloomFilter())) {
                auto &block = blocks_inmemory[metadata.second.getId()];

                if (metadata.second.getBlockIsInDisk()) {
                    if (lruMemoryList.full()) {
                        evictBlockFromMemory();
                    }
                    block.readBlockFromDisk(dataFileName, metadata.second.getDiskOffset(), metadata.second.getKeyNum());
                    metadata.second.setBlockIsInDisk(false);
                }

                ValueType value;
                if (block.get(key, value)) {
                    lruMemoryList.visit(metadata.second.getId());
                    return value;
                }
            }
        }
        return ValueType();
    }


};

#endif //HYBRIDCACHEEXAMPLE_HYBRIDCACHE_H
