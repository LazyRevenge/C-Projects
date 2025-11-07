#pragma once
#include <vector>

class IntHashMap {
public:
    IntHashMap(size_t initialSize = 101);
    ~IntHashMap();

    void insert(int key, void* ptr);

    template <typename T>
    T* find(int key) const {
        size_t index = hashKey(key) % capacity;
        size_t start = index;
        size_t step = hashStep(key);
        while (table[index].inUse) {
            if (table[index].key == key)
                return static_cast<T*>(table[index].ptr);
            index = (index + step) % capacity;
            if (index == start) break;
        }
        return nullptr;
    }

    void remove(int key);
    void clear();

private:
    struct Entry {
        int key;
        void* ptr;
        bool inUse;
        Entry() : key(0), ptr(nullptr), inUse(false) {}
    };

    std::vector<Entry> table;
    size_t capacity;
    size_t size;

    size_t hashKey(int key) const;
    size_t hashStep(int key) const;
    void rehash();
};