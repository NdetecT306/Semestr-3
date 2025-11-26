#include "hash.h"
#include <iostream>
#include <chrono>
#include <random>
#include <string>
using namespace std;
using namespace std::chrono;

class HashBenchmark {
private:
    DoubleHashTable hashTable;

public:
    void testInsertOperations() {
        cout << "ADD" << endl;
        auto start = high_resolution_clock::now();
        int inserted = 0;
        for (int i = 0; i < 100; i++) {
            if (hashTable.insert(i, "value_" + to_string(i))) {
                inserted++;
            }
        }
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        cout << "Вставлено " << inserted << " элементов за " 
             << duration.count() << " микросекунд" << endl;
        cout << "Размер таблицы: " << hashTable.size() << endl;
        cout << "Количество элементов: " << hashTable.elementCount() << endl;
        start = high_resolution_clock::now();
        int duplicateAttempts = 0;
        for (int i = 0; i < 50; i++) {
            if (!hashTable.insert(i, "duplicate_" + to_string(i))) {
                duplicateAttempts++;
            }
        }
        end = high_resolution_clock::now();
        duration = duration_cast<microseconds>(end - start);
        cout << "Попыток вставить дубликаты: " << duplicateAttempts << " за " << duration.count() << " микросекунд" << endl;
    }
    void testSearchOperations() {
        cout << "\nFind" << endl;
        for (int i = 0; i < 50; i++) {
            hashTable.insert(i, "search_test_" + to_string(i));
        }
        auto start = high_resolution_clock::now();
        int found = 0;
        for (int i = 0; i < 50; i++) {
            if (hashTable.containsKey(i)) {
                found++;
            }
        }
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        cout << "Найдено " << found << " элементов за " << duration.count() << " микросекунд" << endl;
        start = high_resolution_clock::now();
        int notFound = 0;
        for (int i = 100; i < 150; i++) {
            if (!hashTable.containsKey(i)) {
                notFound++;
            }
        }
        end = high_resolution_clock::now();
        duration = duration_cast<microseconds>(end - start);
        
        cout << "Не найдено " << notFound << " элементов за " << duration.count() << " микросекунд" << endl;
    }
    void testDeleteOperations() {
        cout << "\n=== Тест операций удаления ===" << endl;
        for (int i = 0; i < 30; i++) {
            hashTable.insert(i, "delete_test_" + to_string(i));
        }
        cout << "Элементов до удаления: " << hashTable.elementCount() << endl;
        auto start = high_resolution_clock::now();
        int deleted = 0;
        for (int i = 0; i < 20; i++) {
            if (hashTable.remove(i, "delete_test_" + to_string(i))) {
                deleted++;
            }
        }
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        cout << "Удалено " << deleted << " элементов за " << duration.count() << " микросекунд" << endl;
        cout << "Элементов после удаления: " << hashTable.elementCount() << endl;
    }
    void testCollisionHandling() {
        cout << "\n=== Тест обработки коллизий ===" << endl;
        hashTable.INIT_HASH_TABLE();
        int baseKey = 1000;
        int inserted = 0;
        auto start = high_resolution_clock::now();
        for (int i = 0; i < 20; i++) {
            int key = baseKey + i * hashTable.size(); 
            if (hashTable.insert(key, "collision_test_" + to_string(i))) {
                inserted++;
            }
        }
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        cout << "Вставлено " << inserted << " элементов с коллизиями за " << duration.count() << " микросекунд" << endl;
        cout << "Размер таблицы: " << hashTable.size() << endl;
        cout << "Количество элементов: " << hashTable.elementCount() << endl;
    }
    void testLoadFactorPerformance() {
        cout << "\nMemory" << endl;
        hashTable.INIT_HASH_TABLE();
        int tableSize = hashTable.size();
        vector<double> loadFactors = {0.25, 0.5, 0.75, 0.9};
        for (double loadFactor : loadFactors) {
            hashTable.INIT_HASH_TABLE();
            int elementsToInsert = static_cast<int>(tableSize * loadFactor);
            auto startInsert = high_resolution_clock::now();
            for (int i = 0; i < elementsToInsert; i++) {
                hashTable.insert(i, "load_test_" + to_string(i));
            }
            auto endInsert = high_resolution_clock::now();
            auto insertDuration = duration_cast<microseconds>(endInsert - startInsert);
            auto startSearch = high_resolution_clock::now();
            for (int i = 0; i < 100; i++) {
                hashTable.containsKey(i);
            }
            auto endSearch = high_resolution_clock::now();
            auto searchDuration = duration_cast<microseconds>(endSearch - startSearch);
            cout << "Нагрузка " << (loadFactor * 100) << "%: "<< "Вставка " << elementsToInsert << " элементов за " << insertDuration.count() << " мкс, "<< "Поиск 100 элементов за " << searchDuration.count() << " мкс, "<< "Элементов в таблице: " << hashTable.elementCount() << endl;
        }
    }
    void runAllTests() {
        cout << "Запуск всех тестов для DoubleHashTable" << endl;
        cout << "Размер хеш-таблицы: " << hashTable.size() << endl;
        cout << "=========================================" << endl;
        testInsertOperations();
        testSearchOperations();
        testDeleteOperations();
        testCollisionHandling();
        testLoadFactorPerformance();
        cout << "\n=========================================" << endl;
        cout << "Все тесты завершены!" << endl;
    }
};
int main() {
    HashBenchmark benchmark;
    benchmark.runAllTests();
    return 0;
}
