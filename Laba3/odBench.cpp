#include <iostream>
#include <string>
#include <chrono>
#include <random>
#include "odspis.h"
using namespace std;
using namespace std::chrono;
class LinkedListBenchmark {
private:
    OdSpis* list;
public:
    LinkedListBenchmark() : list(nullptr) {}
    ~LinkedListBenchmark() {
        if (list) {
            list->cleanupList(list);
        }
    }
    void testAddOperations() {
        cout << "\nADD" << endl;
        if (list) list->cleanupList(list);
        list = new OdSpis("head");
        auto start = high_resolution_clock::now();
        for (int i = 0; i < 1000; i++) {
            list->AddToEnd(list, "element_end_" + to_string(i));
        }
        auto end = high_resolution_clock::now();
        auto durationEnd = duration_cast<microseconds>(end - start);
        cout << "Добавление 1000 элементов в конец: " << durationEnd.count() << " микросекунд" << endl;
        cout << "Размер списка: " << list->SizeList(list) << endl;
        start = high_resolution_clock::now();
        for (int i = 0; i < 500; i++) {
            list->AddToBegin(list, "element_begin_" + to_string(i));
        }
        end = high_resolution_clock::now();
        auto durationBegin = duration_cast<microseconds>(end - start);
        cout << "Добавление 500 элементов в начало: " << durationBegin.count() << " микросекунд" << endl;
        cout << "Размер списка: " << list->SizeList(list) << endl;
        start = high_resolution_clock::now();
        OdSpis* target = list->FindElement(list, "element_end_500");
        if (target) {
            for (int i = 0; i < 100; i++) {
                list->AddChelPosle(target, "element_after_" + to_string(i));
            }
        }
        end = high_resolution_clock::now();
        auto durationAfter = duration_cast<microseconds>(end - start);
        cout << "Добавление 100 элементов после target: " << durationAfter.count() << " микросекунд" << endl;
        cout << "Финальный размер: " << list->SizeList(list) << endl;
    }
    void testSearchOperations() {
        cout << "\nFind" << endl;
        int listSize = list->SizeList(list);
        auto start = high_resolution_clock::now();
        
        int found = 0;
        for (int i = 0; i < 500; i++) {
            if (list->poisk(list, "element_end_" + to_string(i))) {
                found++;
            }
            // Ищем несуществующие элементы тоже
            list->poisk(list, "nonexistent_" + to_string(i));
        }
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        
        cout << "500 операций поиска за: " << duration.count() << " микросекунд" << endl;
        cout << "Найдено элементов: " << found << endl;
        cout << "Среднее время поиска: " << duration.count() / 500.0 << " микросекунд" << endl;
        
        // Тест поиска с возвратом указателя
        start = high_resolution_clock::now();
        
        int pointersFound = 0;
        for (int i = 0; i < 200; i++) {
            OdSpis* element = list->FindElement(list, "element_begin_" + to_string(i % 100));
            if (element) pointersFound++;
        }
        
        end = high_resolution_clock::now();
        auto durationPointer = duration_cast<microseconds>(end - start);
        
        cout << "200 операций поиска с указателем за: " << durationPointer.count() << " микросекунд" << endl;
        cout << "Найдено указателей: " << pointersFound << endl;
    }
    
    void testDeleteOperations() {
        cout << "\nDelete" << endl;
        int initialSize = list->SizeList(list);
        auto start = high_resolution_clock::now();
        for (int i = 0; i < 100; i++) {
            list->deleteToBegin(list);
        }
        auto end = high_resolution_clock::now();
        auto durationBegin = duration_cast<microseconds>(end - start);
        cout << "Удаление 100 элементов из начала: " << durationBegin.count() << " микросекунд" << endl;
        start = high_resolution_clock::now();
        for (int i = 0; i < 100; i++) {
            list->deleteToEnd(list);
        }
        end = high_resolution_clock::now();
        auto durationEnd = duration_cast<microseconds>(end - start);
        cout << "Удаление 100 элементов из конца: " << durationEnd.count() << " микросекунд" << endl;
        start = high_resolution_clock::now();
        int deletedByValue = 0;
        for (int i = 0; i < 200; i++) {
            string target = "element_end_" + to_string(i + 300);
            int sizeBefore = list->SizeList(list);
            list->deletePoZnach(list, target);
            if (list->SizeList(list) < sizeBefore) {
                deletedByValue++;
            }
        }
        end = high_resolution_clock::now();
        auto durationValue = duration_cast<microseconds>(end - start);
        cout << "Удаление 200 элементов по значению: " << durationValue.count() << " микросекунд" << endl;
        cout << "Удалено элементов: " << deletedByValue << endl;
        cout << "Было: " << initialSize << ", стало: " << list->SizeList(list) << endl;
    }
    void testRandomAccessOperations() {
        cout << "\n--- Тест: Случайные операции вставки/удаления ---" << endl;
        for (int i = 0; i < 200; i++) {
            list->AddToEnd(list, "random_test_" + to_string(i));
        }
        int initialSize = list->SizeList(list);
        auto start = high_resolution_clock::now();
        OdSpis* target1 = list->FindElement(list, "random_test_50");
        OdSpis* target2 = list->FindElement(list, "random_test_100");
        OdSpis* target3 = list->FindElement(list, "random_test_150");
        if (target1) {
            for (int i = 0; i < 20; i++) {
                list->AddChelPosle(target1, "inserted_after_50_" + to_string(i));
            }
        }
        if (target2) {
            for (int i = 0; i < 10; i++) {
                list->deleteChelPosle(target2);
            }
        }
        if (target3) {
            for (int i = 0; i < 15; i++) {
                list->AddChelDo(list, target3, "inserted_before_150_" + to_string(i));
            }
        }
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        cout << "Случайные операции вставки/удаления: " << duration.count() << " микросекунд" << endl;
        cout << "Было: " << initialSize << ", стало: " << list->SizeList(list) << endl;
    }
    void testMemoryOperations() {
        cout << "\nMemory" << endl;
        if (list) list->cleanupList(list);
        list = new OdSpis("memory_head");
        auto start = high_resolution_clock::now();
        for (int i = 0; i < 5000; i++) {
            list->AddToEnd(list, "memory_element_" + to_string(i));
        }
        auto endCreate = high_resolution_clock::now();
        start = high_resolution_clock::now();
        list->cleanupList(list);
        endCreate = high_resolution_clock::now();
        auto cleanupDuration = duration_cast<microseconds>(endCreate - start);
        cout << "Полная очистка списка (5000 элементов): " << cleanupDuration.count() << " микросекунд" << endl;
        cout << "Список после очистки: " << (list == nullptr ? "пустой" : "не пустой") << endl;
        list = new OdSpis("final_head");
    }
    void runAllTests() {
        cout << "=== БЕНЧМАРК ТЕСТЫ ДЛЯ ОДНОСВЯЗНОГО СПИСКА ===" << endl;
        testAddOperations();
        testSearchOperations();
        testDeleteOperations();
        testRandomAccessOperations();
        testMemoryOperations();
        cout << "=== ВСЕ ТЕСТЫ СПИСКА ЗАВЕРШЕНЫ ===" << endl;
    }
};
int main() {
    LinkedListBenchmark benchmark;
    benchmark.runAllTests();
    return 0;
}
