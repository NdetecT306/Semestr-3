#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <string>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

// Константы
extern const int HASH_SIZE;
extern const int SCHEMA_HASH_SIZE;

// Класс Vec - вектор для строк
class Vec {
private:
    string* data;
    size_t size;
    size_t capacity;

public:
    Vec();
    Vec(const Vec& other);
    Vec& operator=(const Vec& other);
    ~Vec();

    void reserve(size_t newCap);
    void PUSH(const string& value);
    void erase(size_t index);
    void clear();
    string& operator[](size_t index);
    const string& operator[](size_t index) const;
    size_t getSize() const;
    bool empty() const;
    void sort();

private:
    void mergeSort(size_t left, size_t right);
    void merge(size_t left, size_t mid, size_t right);
};

// Класс DOUBLE_HASH - хэш-таблица
class DOUBLE_HASH {
private:
    int key;
    Vec value;  
    bool isDeleted;
    bool isEmpty;

public:
    DOUBLE_HASH();
    int getKey() const;
    Vec& getValue();
    const Vec& getValue() const;
    bool getIsDeleted() const;
    bool getIsEmpty() const;
    void setKey(int k);
    void setValue(const Vec& val);
    void setIsDeleted(bool deleted);
    void setIsEmpty(bool empty);
    void clear();
    void init(int k, const Vec& val);
    bool isValid() const;
};

// Класс SCHEMA_HASH - хэш-таблица для схем
class SCHEMA_HASH {
private:
    string key;
    DOUBLE_HASH* value;
    bool isDeleted;
    bool isEmpty;

public:
    SCHEMA_HASH();
    string getKey() const;
    DOUBLE_HASH* getValue();
    const DOUBLE_HASH* getValue() const;
    bool getIsDeleted() const;
    bool getIsEmpty() const;
    void setKey(const string& k);
    void setValue(DOUBLE_HASH* val);
    void setIsDeleted(bool deleted);
    void setIsEmpty(bool empty);
    void clear();
    void init(const string& k, DOUBLE_HASH* val);
    bool isValid() const;
};

// Глобальные переменные
extern DOUBLE_HASH hashTable[];
extern SCHEMA_HASH schemaHashTable[];

// Функции хэширования
int hash1(int key);
int hash2(int key);
int schemaHash1(const string& key);
int schemaHash2(const string& key);

// Базовые операции с хэш-таблицами
void CREATETABLE();
void CLEAR();
bool ADD(int key, const Vec& value);
bool SETDEL(int key, const string& value);
void CREATE_SCHEMA_TABLE();
void CLEAR_SCHEMA_TABLE();
bool ADD_SCHEMA(const string& key, DOUBLE_HASH* value);
DOUBLE_HASH* GET_SCHEMA(const string& key);

#endif
