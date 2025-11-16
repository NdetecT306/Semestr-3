#include "struct.h"
// Реализация глобальных переменных
DOUBLE_HASH hashTable[HASH_SIZE]; 
json schemaConfig;
string mySchema = "schema.json";
string currentSchemaName = "";
int tuplesLimit = 10;
unordered_map<string, int> tableKeys;
void Vec::reserve(size_t newCap) {// Реализации методов Vec
    if (newCap <= capacity) {
        return;
    }
    string* new_data = new string[newCap];
    for (size_t i = 0; i < size; i++) {
        new_data[i] = data[i];
    }
    delete[] data;
    data = new_data;
    capacity = newCap;
}
void Vec::PUSH(const string& value) {
    if (size >= capacity) {
        size_t newCapacity = (capacity == 0) ? 1 : capacity * 2;
        reserve(newCapacity);
    }
    data[size] = value;
    size++;
}
void Vec::erase(size_t index) {
    if (index >= size) {
        return;
    }
    for (size_t i = index; i < size - 1; i++) {
        data[i] = data[i + 1];
    }
    size--;
}
int hash1(int key) {// Реализации функций хэш-таблицы
    return key % HASH_SIZE;
}
int hash2(int key) {
    return 13 - (key % 13);
}
void CREATETABLE() {
     for (int i = 0; i < HASH_SIZE; i++) {
        hashTable[i].setKey(-1);
        hashTable[i].getValue().clear(); 
        hashTable[i].setIsDeleted(false);
        hashTable[i].setIsEmpty(true);
    }
}
void CLEAR() {
    for (int i = 0; i < HASH_SIZE; i++) {
        hashTable[i].clear();
    }
    CREATETABLE();
    tableKeys.clear();
}
bool ADD(int key, string value) {
    int ind = hash1(key);
    int step = hash2(key);
    int startInd = ind;
    int trials = 0;
    while (trials < HASH_SIZE) {
        if (hashTable[ind].getIsEmpty() || hashTable[ind].getIsDeleted()) {
            break;
        }
        if (hashTable[ind].getKey() == key && !hashTable[ind].getIsDeleted()) {
            hashTable[ind].getValue().PUSH(value); 
            return true;
        }
        ind = (ind + step) % HASH_SIZE;
        trials++;
        if (ind == startInd) {
            break;
        }
    }
    ind = hash1(key);
    step = hash2(key);
    startInd = ind;
    trials = 0;
    while (trials < HASH_SIZE) {
        if (hashTable[ind].getIsEmpty() || hashTable[ind].getIsDeleted()) {
            Vec newVec;
            newVec.PUSH(value);
            hashTable[ind].init(key, newVec);
            return true;
        }
        ind = (ind + step) % HASH_SIZE;
        trials++;
        if (ind == startInd) {
            break;
        }
    }
    return false;
}
bool SETDEL(int key, string value) {
    int ind = hash1(key);
    int step = hash2(key);
    int startInd = ind;
    int trials = 0;
    while (trials < HASH_SIZE) {
        if (hashTable[ind].getIsEmpty() && !hashTable[ind].getIsDeleted()) {
            return false;
        }
        if (hashTable[ind].getKey() == key && !hashTable[ind].getIsDeleted()) {
            Vec& vec = hashTable[ind].getValue();
            for (size_t i = 0; i < vec.getSize(); i++) {
                if (vec[i] == value) {
                    vec.erase(i); 
                    if (vec.empty()) { 
                        hashTable[ind].setIsDeleted(true);
                        hashTable[ind].setIsEmpty(true);
                    }
                    return true;
                }
            }
            return false;
        }
        ind = (ind + step) % HASH_SIZE;
        trials++;
        if (ind == startInd) {
            break;
        }
    }
    return false;
}
