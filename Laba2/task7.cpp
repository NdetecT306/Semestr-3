#include <iostream>
#include <string>
using namespace std;
const int HASH_SIZE = 256;
const int MAX_FREQ = 1000000; 
struct LFU_NODE {
    int key;
    int value;
    int w;  
    bool del;
    bool emp;
};
LFU_NODE hashTable[HASH_SIZE];
int minW = 0; 
int currentSize = 0;  
int C = 0;     
int hash1(int key) {
    return abs(key) % HASH_SIZE;
}
int hash2(int key) {
    return 13 - (abs(key) % 13);
}
void CREATE(int cap) {
    C = cap;
    currentSize = 0;
    for (int i = 0; i < HASH_SIZE; i++) {
        hashTable[i].key = -1;
        hashTable[i].value = -1;
        hashTable[i].w = 0;
        hashTable[i].del = false;
        hashTable[i].emp = true;
    }
    minW = 0;
}
int findLFUNode() {
    int lfuIndex = -1;
    int minFreq = MAX_FREQ;
    for (int i = 0; i < HASH_SIZE; i++) {
        if (!hashTable[i].emp && !hashTable[i].del) {
            if (hashTable[i].w < minFreq) {
                minFreq = hashTable[i].w;
                lfuIndex = i;
            }
        }
    }
    return lfuIndex;
}
int findKeyIndex(int key) {
    int ind = hash1(key);
    int step = hash2(key);
    for (int attempts = 0; attempts < HASH_SIZE; attempts++) {
        if (hashTable[ind].emp && !hashTable[ind].del) {
            return -1; 
        }
        if (!hashTable[ind].emp && !hashTable[ind].del &&
            hashTable[ind].key == key) {
            return ind; 
        }
        ind = (ind + step) % HASH_SIZE;
    }
    return -1; 
}
int findInsertSlot(int key) {
    int ind = hash1(key);
    int step = hash2(key);
    int firstDeleted = -1;
    for (int attempts = 0; attempts < HASH_SIZE; attempts++) {
        if (hashTable[ind].emp && !hashTable[ind].del) {
            return ind;
        }
        if (hashTable[ind].del && firstDeleted == -1) {
            firstDeleted = ind;
        }
        if (!hashTable[ind].emp && !hashTable[ind].del && hashTable[ind].key == key) {
            return ind;
        }

        ind = (ind + step) % HASH_SIZE;
    }
    if (firstDeleted != -1) {
        return firstDeleted;
    }
    return -1; // Нет места
}
bool SET(int key, int value) {
    int existingIndex = findKeyIndex(key);
    if (existingIndex != -1) {
        hashTable[existingIndex].value = value;
        hashTable[existingIndex].w++;
        return true;
    }
    if (currentSize >= C) {
        int lfuIndex = findLFUNode();
        if (lfuIndex == -1) {
            return false; // Не удалось найти элемент для удаления
        }
        hashTable[lfuIndex].emp = true;
        hashTable[lfuIndex].del = true;
        currentSize--;
    }
    int insertIndex = findInsertSlot(key);
    if (insertIndex == -1) {
        return false; 
    }
    hashTable[insertIndex].key = key;
    hashTable[insertIndex].value = value;
    hashTable[insertIndex].w = 1;
    hashTable[insertIndex].del = false;
    hashTable[insertIndex].emp = false;
    currentSize++;
    minW = 1;
    return true;
}
int GET(int key) {
    int ind = findKeyIndex(key);
    if (ind == -1) {
        return -1;
    }
    hashTable[ind].w++;
    return hashTable[ind].value;
}
bool DELETE(int key) {
    int ind = findKeyIndex(key);
    if (ind == -1) {
        return false; // Ключ не найден
    }
    hashTable[ind].del = true;
    hashTable[ind].emp = true;
    currentSize--;
    return true;
}
void updateMinFrequency() {
    minW = MAX_FREQ;
    for (int i = 0; i < HASH_SIZE; i++) {
        if (!hashTable[i].emp && !hashTable[i].del) {
            if (hashTable[i].w < minW) {
                minW = hashTable[i].w;
            }
        }
    }
    if (minW == MAX_FREQ) {
        minW = 0;
    }
}
int main() {
    return 0;
}
