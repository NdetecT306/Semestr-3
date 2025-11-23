#include "struct.h"
#include <iostream>
#include <stdexcept>
using namespace std;
const int HASH_SIZE = 1000;
const int SCHEMA_HASH_SIZE = 100;
DOUBLE_HASH hashTable[HASH_SIZE];
SCHEMA_HASH schemaHashTable[SCHEMA_HASH_SIZE];
// Реализация класса Vec
Vec::Vec() : data(nullptr), size(0), capacity(0) {}
Vec::Vec(const Vec& other) : data(nullptr), size(0), capacity(0) {
    reserve(other.size);
    for (size_t i = 0; i < other.size; i++) {
        PUSH(other.data[i]);
    }
}
Vec& Vec::operator=(const Vec& other) {
    if (this != &other) {
        clear();
        reserve(other.size);
        for (size_t i = 0; i < other.size; i++) {
            PUSH(other.data[i]);
        }
    }
    return *this;
}
Vec::~Vec() {
    delete[] data;
}
void Vec::reserve(size_t newCap) {
    if (newCap <= capacity) return;
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
    if (index >= size) return;
    for (size_t i = index; i < size - 1; i++) {
        data[i] = data[i + 1];
    }
    size--;
}
void Vec::clear() {
    delete[] data;
    data = nullptr;
    size = 0;
    capacity = 0;
}
string& Vec::operator[](size_t index) {
    if (index >= size) throw out_of_range("Index out of range");
    return data[index]; 
}
const string& Vec::operator[](size_t index) const { 
    if (index >= size) throw out_of_range("Index out of range");
    return data[index]; 
}
size_t Vec::getSize() const { return size; }
bool Vec::empty() const { return size == 0; }
void Vec::sort() {
    if (size > 1) {
        mergeSort(0, size - 1);
    }
}
void Vec::mergeSort(size_t left, size_t right) {
    if (left < right) {
        size_t mid = left + (right - left) / 2;
        mergeSort(left, mid);
        mergeSort(mid + 1, right);
        merge(left, mid, right);
    }
}
void Vec::merge(size_t left, size_t mid, size_t right) {
    size_t n1 = mid - left + 1;
    size_t n2 = right - mid;
    string* leftArr = new string[n1];
    string* rightArr = new string[n2];
    for (size_t i = 0; i < n1; i++)
        leftArr[i] = data[left + i];
    for (size_t j = 0; j < n2; j++)
        rightArr[j] = data[mid + 1 + j];
    size_t i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (leftArr[i] <= rightArr[j]) {
            data[k] = leftArr[i];
            i++;
        } else {
            data[k] = rightArr[j];
            j++;
        }
        k++;
    }
    while (i < n1) {
        data[k] = leftArr[i];
        i++;
        k++;
    }
    while (j < n2) {
        data[k] = rightArr[j];
        j++;
        k++;
    }
    delete[] leftArr;
    delete[] rightArr;
}
// Реализация класса DOUBLE_HASH
DOUBLE_HASH::DOUBLE_HASH() : key(-1), value(), isDeleted(false), isEmpty(true) {}
int DOUBLE_HASH::getKey() const { return key; }
Vec& DOUBLE_HASH::getValue() { return value; }
const Vec& DOUBLE_HASH::getValue() const { return value; }
bool DOUBLE_HASH::getIsDeleted() const { return isDeleted; }
bool DOUBLE_HASH::getIsEmpty() const { return isEmpty; }
void DOUBLE_HASH::setKey(int k) { key = k; }
void DOUBLE_HASH::setValue(const Vec& val) { value = val; }
void DOUBLE_HASH::setIsDeleted(bool deleted) { isDeleted = deleted; }
void DOUBLE_HASH::setIsEmpty(bool empty) { isEmpty = empty; }
void DOUBLE_HASH::clear() {
    value.clear();
    key = -1;
    isDeleted = false;
    isEmpty = true;
}
void DOUBLE_HASH::init(int k, const Vec& val) {
    key = k;
    value = val;
    isDeleted = false;
    isEmpty = false;
}
bool DOUBLE_HASH::isValid() const {
    return !isEmpty && !isDeleted;
}
// Реализация класса SCHEMA_HASH
SCHEMA_HASH::SCHEMA_HASH() : key(""), value(nullptr), isDeleted(false), isEmpty(true) {}
string SCHEMA_HASH::getKey() const { return key; }
DOUBLE_HASH* SCHEMA_HASH::getValue() { return value; }
const DOUBLE_HASH* SCHEMA_HASH::getValue() const { return value; }
bool SCHEMA_HASH::getIsDeleted() const { return isDeleted; }
bool SCHEMA_HASH::getIsEmpty() const { return isEmpty; }
void SCHEMA_HASH::setKey(const string& k) { key = k; }
void SCHEMA_HASH::setValue(DOUBLE_HASH* val) { value = val; }
void SCHEMA_HASH::setIsDeleted(bool deleted) { isDeleted = deleted; }
void SCHEMA_HASH::setIsEmpty(bool empty) { isEmpty = empty; }
void SCHEMA_HASH::clear() {
    key = "";
    value = nullptr;
    isDeleted = false;
    isEmpty = true;
}
void SCHEMA_HASH::init(const string& k, DOUBLE_HASH* val) {
    key = k;
    value = val;
    isDeleted = false;
    isEmpty = false;
}
bool SCHEMA_HASH::isValid() const {
    return !isEmpty && !isDeleted;
}
// Функции хэширования
int hash1(int key) {
    return key % HASH_SIZE;
}
int hash2(int key) {
    return 13 - (key % 13);
}
int schemaHash1(const string& key) {
    int hash = 0;
    for (char c : key) {
        hash = (hash * 31 + c) % SCHEMA_HASH_SIZE;
    }
    return hash;
}
int schemaHash2(const string& key) {
    int hash = 0;
    for (char c : key) {
        hash = (hash * 17 + c) % SCHEMA_HASH_SIZE;
    }
    return 7 - (hash % 7);
}
// Базовые операции с хэш-таблицами
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
}
bool ADD(int key, const Vec& value) {
    int ind = hash1(key);
    int step = hash2(key);
    int startInd = ind;
    int trials = 0;
    while (trials < HASH_SIZE) {
        if (hashTable[ind].getIsEmpty() || hashTable[ind].getIsDeleted()) {
            break;
        }
        if (hashTable[ind].getKey() == key && !hashTable[ind].getIsDeleted()) {
            Vec& existingVec = hashTable[ind].getValue();
            for (size_t i = 0; i < value.getSize(); i++) {
                existingVec.PUSH(value[i]);
            }
            return true;
        }
        ind = (ind + step) % HASH_SIZE;
        trials++;
        if (ind == startInd) break;
    }
    ind = hash1(key);
    trials = 0;
    while (trials < HASH_SIZE) {
        if (hashTable[ind].getIsEmpty() || hashTable[ind].getIsDeleted()) {
            hashTable[ind].init(key, value);
            return true;
        }
        ind = (ind + step) % HASH_SIZE;
        trials++;
        if (ind == startInd) break;
    }
    return false;
}
bool SETDEL(int key, const string& value) {
    int ind = hash1(key);
    int step = hash2(key);
    int startInd = ind;
    int trials = 0;
    while (trials < HASH_SIZE) {
        if (hashTable[ind].getIsEmpty() && !hashTable[ind].getIsDeleted()) {
            return false;
        }
        if (hashTable[ind].getKey() == key && hashTable[ind].isValid()) {
            hashTable[ind].setIsDeleted(true);
            hashTable[ind].setIsEmpty(true);
            return true;
        }
        ind = (ind + step) % HASH_SIZE;
        trials++;
        if (ind == startInd) break;
    }
    return false;
}
void CREATE_SCHEMA_TABLE() {
    for (int i = 0; i < SCHEMA_HASH_SIZE; i++) {
        schemaHashTable[i].setKey("");
        schemaHashTable[i].setValue(nullptr);
        schemaHashTable[i].setIsDeleted(false);
        schemaHashTable[i].setIsEmpty(true);
    }
}
void CLEAR_SCHEMA_TABLE() {
    for (int i = 0; i < SCHEMA_HASH_SIZE; i++) {
        schemaHashTable[i].clear();
    }
    CREATE_SCHEMA_TABLE();
}
bool ADD_SCHEMA(const string& key, DOUBLE_HASH* value) {
    int ind = schemaHash1(key);
    int step = schemaHash2(key);
    int startInd = ind;
    int trials = 0;
    while (trials < SCHEMA_HASH_SIZE) {
        if (schemaHashTable[ind].getIsEmpty() || schemaHashTable[ind].getIsDeleted()) {
            break;
        }
        if (schemaHashTable[ind].getKey() == key && !schemaHashTable[ind].getIsDeleted()) {
            schemaHashTable[ind].setValue(value);
            return true;
        }
        ind = (ind + step) % SCHEMA_HASH_SIZE;
        trials++;
        if (ind == startInd) break;
    }
    ind = schemaHash1(key);
    trials = 0;
    while (trials < SCHEMA_HASH_SIZE) {
        if (schemaHashTable[ind].getIsEmpty() || schemaHashTable[ind].getIsDeleted()) {
            schemaHashTable[ind].init(key, value);
            return true;
        }
        ind = (ind + step) % SCHEMA_HASH_SIZE;
        trials++;
        if (ind == startInd) break;
    }
    return false;
}
DOUBLE_HASH* GET_SCHEMA(const string& key) {
    int ind = schemaHash1(key);
    int step = schemaHash2(key);
    int startInd = ind;
    int trials = 0;
    while (trials < SCHEMA_HASH_SIZE) {
        if (schemaHashTable[ind].getIsEmpty() && !schemaHashTable[ind].getIsDeleted()) {
            return nullptr;
        }
        if (schemaHashTable[ind].getKey() == key && schemaHashTable[ind].isValid()) {
            return schemaHashTable[ind].getValue();
        }
        ind = (ind + step) % SCHEMA_HASH_SIZE;
        trials++;
        if (ind == startInd) break;
    }
    return nullptr;
}
