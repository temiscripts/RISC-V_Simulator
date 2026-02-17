#include "cache.h"
#include <iostream>
using namespace std;

bool DirectMappedCache::read(int address, int &data) {
    int index = address % size;
    int tag = address / size;

    if (cache[index].address == address) {
        data = cache[index].data;
        cout << "  [CACHE] READ Addr " << address << " -> HIT, Data = " << data << endl;
        return true;
    } else {
        cout << "  [CACHE] READ Addr " << address << " -> MISS, loading..." << endl;
        cache[index].address = address;
        cache[index].data = data; 
    }
}

void DirectMappedCache::write(int address, int data) {
    int index = address % size;
    cache[index].address = address;
    cache[index].data = data;
    cout << "  [CACHE] WRITE Addr " << address << ", Data = " << data << endl;
}
