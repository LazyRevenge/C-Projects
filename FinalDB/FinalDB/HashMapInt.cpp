#include "IntHashMap.h"
#include <cmath>

static bool isPrime(size_t n) {
    if (n < 2) return false;
    if (n % 2 == 0) return (n == 2);
    for (size_t i = 3; i * i <= n; i += 2)
        if (n % i == 0) return false;
    return true;
}

static size_t nextPrime(size_t n) {
    while (!isPrime(n)) ++n;
    return n;
}

IntHashMap::IntHashMap(size_t initialSize)
    : capacity(nextPrime(initialSize)), size(0)
{
    table.resize(capacity);
}

IntHashMap::~IntHashMap() {}

size_t IntHashMap::hashKey(int key) const {
    return std::hash<int>()(key);
}

size_t IntHashMap::hashStep(int key) const {
    return 1 + (std::hash<int>()(key) % (capacity - 2));
}

void IntHashMap::insert(int key, void* ptr) {
    if (size + 1 > capacity * 0.75) {
        rehash();
    }
    size_t index = hashKey(key) % capacity;
    size_t step = hashStep(key);
    while (table[index].inUse) {
        if (table[index].key == key) {
            table[index].ptr = ptr;
            return;
        }
        index = (index + step) % capacity;
    }
    table[index].inUse = true;
    table[index].key = key;
    table[index].ptr = ptr;
    ++size;
}

void IntHashMap::remove(int key) {
    size_t index = hashKey(key) % capacity;
    size_t step = hashStep(key);
    while (table[index].inUse) {
        if (table[index].key == key) {
            table[index].inUse = false;
            table[index].ptr = nullptr;
            --size;
            return;
        }
        index = (index + step) % capacity;
    }
}

void IntHashMap::clear() {
    for (auto& entry : table) {
        entry.inUse = false;
        entry.ptr = nullptr;
    }
    size = 0;
}

void IntHashMap::rehash() {
    size_t newCapacity = nextPrime(capacity * 2);
    std::vector<Entry> oldTable = std::move(table);

    table.clear();
    table.resize(newCapacity);
    capacity = newCapacity;
    size = 0;

    for (auto& entry : oldTable) {
        if (entry.inUse) {
            insert(entry.key, entry.ptr);
        }
    }
}