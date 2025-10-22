#include <iostream>
#include <string>
#include <climits>
using namespace std;

const int HASH_SIZE = 256;

struct LFU_NODE {
    int key;
    int value;
    int freq;  
    int timestamp; 
    bool occupied;
};

LFU_NODE hashTable[HASH_SIZE];
int currentSize = 0;  
int capacity = 0;     
int timeCounter = 0;

int hash1(int key) {
    return key % HASH_SIZE;
}

int hash2(int key) {
    return 13 - (key % 13);
}

void CREATE(int cap) {
    capacity = cap;
    currentSize = 0;
    timeCounter = 0;
    for (int i = 0; i < HASH_SIZE; i++) {
        hashTable[i].key = -1;
        hashTable[i].value = -1;
        hashTable[i].freq = 0;
        hashTable[i].timestamp = -1;
        hashTable[i].occupied = false;
    }
}

int findKeyIndex(int key) {
    int ind = hash1(key);
    int step = hash2(key);
    for (int attempts = 0; attempts < HASH_SIZE; attempts++) {
        if (!hashTable[ind].occupied) {
            return -1;
        }
        if (hashTable[ind].occupied && hashTable[ind].key == key) {
            return ind;
        }
        ind = (ind + step) % HASH_SIZE;
    }
    return -1;
}

int findInsertSlot(int key) {
    int ind = hash1(key);
    int step = hash2(key);
    for (int attempts = 0; attempts < HASH_SIZE; attempts++) {
        if (!hashTable[ind].occupied) {
            return ind;
        }
        ind = (ind + step) % HASH_SIZE;
    }
    return -1;
}

int findLFUNodeToEvict() { //Смотрим на частоту посещения
    int lfuIndex = -1;
    int minFreq = 0x7fffffff;
    int oldestTime = 0x7fffffff;
    for (int i = 0; i < HASH_SIZE; i++) {
        if (hashTable[i].occupied) {
            if (hashTable[i].freq < minFreq) {
                minFreq = hashTable[i].freq;
                oldestTime = hashTable[i].timestamp;
                lfuIndex = i;
            } else if (hashTable[i].freq == minFreq && hashTable[i].timestamp < oldestTime) {
                oldestTime = hashTable[i].timestamp;
                lfuIndex = i;
            }
        }
    }
    return lfuIndex;
}

bool SET(int key, int value) { //Добавляем элемент
    int existingIndex = findKeyIndex(key);
    if (existingIndex != -1) {        // Обновляем значение при повторном обращении
        hashTable[existingIndex].value = value;
        hashTable[existingIndex].freq++;
        hashTable[existingIndex].timestamp = timeCounter++;
        return true;
    }
    if (currentSize >= capacity) {
        int lfuIndex = findLFUNodeToEvict();
        if (lfuIndex == -1) {
            return false;
        }
        hashTable[lfuIndex].occupied = false;
        currentSize--;
    }
    int insertIndex = findInsertSlot(key);
    if (insertIndex == -1) {
        return false;
    }
    hashTable[insertIndex].key = key;
    hashTable[insertIndex].value = value;
    hashTable[insertIndex].freq = 1;
    hashTable[insertIndex].timestamp = timeCounter++;
    hashTable[insertIndex].occupied = true;
    currentSize++;
    return true;
}

int GET(int key) { //Выводим элемент
    int ind = findKeyIndex(key);
    if (ind == -1) {
        return -1;
    }
    hashTable[ind].freq++;
    hashTable[ind].timestamp = timeCounter++;
    return hashTable[ind].value;
}

int main() {
    int cap;
    cout << "Емкость: ";
    cin >> cap;
    int Q;
    cout << "Кол-во запросов: ";
    cin >> Q;
    
    CREATE(cap);
    cout << "Введите команды:" << endl;
    
    for(int i = 0; i < Q; i++) {
        string command;
        cin >> command;
        
        if(command == "SET") {
            int key, value;
            cin >> key >> value;
            bool result = SET(key, value);
        }
        else if(command == "GET") {
            int key;
            cin >> key;
            int result = GET(key);
            cout << result  << endl;
        }
    }
    cout << endl;
    return 0;
}
