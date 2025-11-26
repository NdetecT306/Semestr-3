#ifndef DOUBLEHASHTABLE_H
#define DOUBLEHASHTABLE_H
#include<iostream>
#include<string>
using namespace std;
const int HASH_SIZE = 256;
class DoubleHashTable {
private:
    struct HashEntry {
        int key;
        string value;
        bool isDeleted;
        bool isEmpty;
        
        HashEntry() : key(-1), value(""), isDeleted(false), isEmpty(true) {}
    };
    HashEntry hashTable[HASH_SIZE];
    int hash1(int key) const {
        return key % HASH_SIZE;
    }
    int hash2(int key) const {
        return 13 - (key % 13);
    }
public:
    DoubleHashTable();
    void INIT_HASH_TABLE();
    bool ADD(int key, string value);
    bool SETDEL(int key, string value);
    bool SET_AT(int key, string value) const;
    void getAllElements(int* keys, string* values, int& count) const;
    bool contains(int key, string value) const;
    bool remove(int key, string value);
    bool insert(int key, string value);
    int size() const;
    bool containsKey(int key) const;
    int elementCount() const;
};

#endif
