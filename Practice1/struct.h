#pragma once  // Добавляем защиту от множественного включения
#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <chrono>
#include <thread>
#include <functional>
#include <random>
#include <unordered_map>
#include "json.hpp"

using json = nlohmann::json;
using namespace nlohmann::literals;
using namespace std;
namespace fs = filesystem;
class DOUBLE_HASH;

class Vec { // Строковый вектор 
private:  
    string* data; 
    size_t size;
    size_t capacity;

public:
    Vec() : data(nullptr), size(0), capacity(0) {} // Конструктор 
    ~Vec() { // Деструктор
        delete[] data;
    }
    Vec(const Vec& other) : data(nullptr), size(0), capacity(0) { // Копирование
        if (other.size > 0) {
            reserve(other.size);
            for (size_t i = 0; i < other.size; i++) {
                data[i] = other.data[i];
            }
            size = other.size;
        }
    }
    void PUSH(const string& value);
    void erase(size_t index);
    void reserve(size_t new_capacity);
    void clear() { // Очистка
        size = 0;
    }
    string& operator[](size_t index) { // Получение по индексу
        return data[index];
    }
    string& operator[](size_t index) const { // Константная версия
        return data[index];
    }
    size_t getSize() const { // Функция получения размера
        return size;
    }
    bool empty() const { // Функция проверки на пустоту
        return size == 0;
    }
    Vec& operator=(const Vec& other) { 
        if (this != &other) {
            if (other.size > capacity) {
                delete[] data;
                data = new string[other.size];
                capacity = other.size;
            }
            for (size_t i = 0; i < other.size; i++) {
                data[i] = other.data[i];
            }
            size = other.size;
        }
        return *this;
    }
    void sort() { // Сортировка элементов вектора
        if (size <= 1) return;
        std::sort(data, data + size);
    }
    string* begin() { // Указатели
        return data;
    }
    string* end() {
        return data + size;
    } 
    string* begin() const {
        return data;
    }
    string* end() const {
        return data + size;
    }
};
const int HASH_SIZE = 256; // Структуры и глобальные переменные
class DOUBLE_HASH { // Класс хэш-таблицы
private:
    int key;
    Vec value;  
    bool isDeleted;
    bool isEmpty;
public:
    DOUBLE_HASH() : key(-1), value(), isDeleted(false), isEmpty(true) {}
    DOUBLE_HASH(int k, const Vec& val, bool deleted = false, bool empty = false) 
        : key(k)
        , value(val)
        , isDeleted(deleted)
        , isEmpty(empty) {}
    int getKey() const { return key; }
    Vec& getValue() { return value; }
    const Vec& getValue() const { return value; }
    bool getIsDeleted() const { return isDeleted; }
    bool getIsEmpty() const { return isEmpty; }
    void setKey(int k) { key = k; }
    void setValue(const Vec& val) { value = val; }
    void setIsDeleted(bool deleted) { isDeleted = deleted; }
    void setIsEmpty(bool empty) { isEmpty = empty; }
    void clear() {
        value.clear();
        key = -1;
        isDeleted = false;
        isEmpty = true;
    }
    void init(int k, const Vec& val) {
        key = k;
        value = val;
        isDeleted = false;
        isEmpty = false;
    }
    bool isValid() const {
        return !isEmpty && !isDeleted;
    }
};
extern DOUBLE_HASH hashTable[HASH_SIZE]; 
extern json schemaConfig;
extern string mySchema;
extern string currentSchemaName;
extern int tuplesLimit;
extern unordered_map<string, int> tableKeys;
int hash1(int key);
int hash2(int key);
void CREATETABLE();
void CLEAR();
bool ADD(int key, string value);
bool SETDEL(int key, string value);
