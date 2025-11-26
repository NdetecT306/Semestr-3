#include "hash.h"

DoubleHashTable::DoubleHashTable() {
    INIT_HASH_TABLE();
}

void DoubleHashTable::INIT_HASH_TABLE() {
    for (int i = 0; i < HASH_SIZE; i++) {
        hashTable[i].value = "";
        hashTable[i].isDeleted = false;
        hashTable[i].isEmpty = true;
        hashTable[i].key = -1;
    }
}

bool DoubleHashTable::ADD(int key, string value) {
    int index = hash1(key);
    int step = hash2(key);
    int startIndex = index;
    int attempts = 0;
    
    while (attempts < HASH_SIZE) {
        if (hashTable[index].isEmpty || hashTable[index].isDeleted) {
            hashTable[index].key = key;
            hashTable[index].value = value;
            hashTable[index].isDeleted = false;
            hashTable[index].isEmpty = false;
            return true;
        }
        if (hashTable[index].key == key && !hashTable[index].isDeleted) {
            return false; 
        }
        index = (index + step) % HASH_SIZE;
        attempts++;
        if (index == startIndex) {
            break;
        }
    }
    return false;
}

bool DoubleHashTable::SETDEL(int key, string value) {
    int index = hash1(key);
    int step = hash2(key);
    int startIndex = index;
    int attempts = 0;
    
    while (attempts < HASH_SIZE) {
        if (hashTable[index].isEmpty && !hashTable[index].isDeleted) {
            return false;
        }
        if (hashTable[index].key == key && hashTable[index].value == value && !hashTable[index].isDeleted) {
            hashTable[index].isDeleted = true;
            hashTable[index].isEmpty = true;
            return true;
        }
        index = (index + step) % HASH_SIZE;
        attempts++;
        if (index == startIndex) {
            break;
        }
    }
    return false;
}

bool DoubleHashTable::SET_AT(int key, string value) const {
    int index = hash1(key);
    int step = hash2(key);
    int startIndex = index;
    int attempts = 0;
    
    while (attempts < HASH_SIZE) {
        if (hashTable[index].isEmpty && !hashTable[index].isDeleted) {
            return false;
        }
        if (hashTable[index].key == key && hashTable[index].value == value && !hashTable[index].isDeleted) {
            return true;
        }
        index = (index + step) % HASH_SIZE;
        attempts++;
        if (index == startIndex) {
            break;
        }
    }
    return false;
}

void DoubleHashTable::getAllElements(int* keys, string* values, int& count) const {
    count = 0;
    for (int i = 0; i < HASH_SIZE; i++) {
        if (!hashTable[i].isEmpty && !hashTable[i].isDeleted) {
            keys[count] = hashTable[i].key;
            values[count] = hashTable[i].value; 
            count++;
        }
    }
}

bool DoubleHashTable::contains(int key, string value) const {
    return SET_AT(key, value);
}

bool DoubleHashTable::remove(int key, string value) {
    return SETDEL(key, value);
}

bool DoubleHashTable::insert(int key, string value) {
    return ADD(key, value);
}

int DoubleHashTable::size() const {
    return HASH_SIZE;
}

bool DoubleHashTable::containsKey(int key) const {
    int index = hash1(key);
    int step = hash2(key);
    int startIndex = index;
    int attempts = 0;
    
    while (attempts < HASH_SIZE) {
        if (hashTable[index].isEmpty && !hashTable[index].isDeleted) {
            return false;
        }
        if (hashTable[index].key == key && !hashTable[index].isDeleted) {
            return true;
        }
        index = (index + step) % HASH_SIZE;
        attempts++;
        if (index == startIndex) {
            break;
        }
    }
    return false;
}

int DoubleHashTable::elementCount() const {
    int count = 0;
    for (int i = 0; i < HASH_SIZE; i++) {
        if (!hashTable[i].isEmpty && !hashTable[i].isDeleted) {
            count++;
        }
    }
    return count;
}
