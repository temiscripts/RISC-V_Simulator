#ifndef CACHE_H
#define CACHE_H

#include <vector>

struct CacheLine {
    int address = -1;
    int data = 0;
};

class DirectMappedCache {
public:
    DirectMappedCache(int size) : size(size) {
        cache.resize(size);
    }

    bool read(int address, int &data);
    void write(int address, int data);

private:
    int size;
    std::vector<CacheLine> cache;
};

#endif
