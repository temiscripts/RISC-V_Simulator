#include "cache.h"
#include <iostream>
using namespace std;

bool DirectMappedCache::read(int address, int &data) {
    int index = address % size;      
    if (cache[index].address == address) {
        data = cache[index].data;
        std::cout << "[CACHE] READ HIT at address " << address << " -> " << data << std::endl;
        return true;
    } else {
        data = address * 10;         
        cache[index].address = address;
        cache[index].data = data;
        std::cout << "[CACHE] READ MISS at address " << address << " -> loaded " << data << std::endl;
        return false;
    }
}

void DirectMappedCache::write(int address, int data) {
    int index = address % size;     
    cache[index].address = address;
    cache[index].data = data;
    std::cout << "[CACHE] WRITE at address " << address << " -> " << data << std::endl;
}

