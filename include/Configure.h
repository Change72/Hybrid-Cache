//
// Created by change_change_2 on 2024/1/23.
//

#ifndef HYBRIDCACHEEXAMPLE_CONFIGURE_H
#define HYBRIDCACHEEXAMPLE_CONFIGURE_H

const int MEMORY_CACHE_SIZE = 4 * 1024 * 1024;  // 10 MB
const int DISK_CACHE_SIZE = 10 * 1024 * 1024;   // 10 MB
const int BLOCK_SIZE = 2 * 1024 * 1024;         // 2 MB

const int BLOOM_FILTER_BITS_PER_KEY = 10;
const int MAX_BF_SIZE = 1024;

#endif //HYBRIDCACHEEXAMPLE_CONFIGURE_H
