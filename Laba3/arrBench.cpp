#include <iostream>
#include <string>
#include <chrono>
#include "array.h"
using namespace std;
using namespace std::chrono;
class ArrayBenchmark {
private:
    Array* arr;
    void testAddAtEnd() {
        cout << "\naddMasAtEnd" << endl;
        delete arr;
        arr = new Array(1000);
        auto start = high_resolution_clock::now();
        for (int i = 0; i < 1000; i++) {
            arr->addMasAtEnd("element_" + to_string(i));
        }
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        cout << "Добавлено 1000 элементов за: " << duration.count() << " микросекунд" << endl;
        cout << "Текущий размер: " << arr->getSize() << endl;
        cout << "Емкость: ";
        arr->getCapacity(); 
    }
    void testAddAtIndex() {
        cout << "\nAddMasAtInd" << endl;
        delete arr;
        arr = new Array(2000);
        for (int i = 0; i < 1000; i++) {
            arr->addMasAtEnd("base_" + to_string(i));
        }
        auto start = high_resolution_clock::now();
        arr->addMasAtInd(0, "inserted_begin");
        arr->addMasAtInd(500, "inserted_middle");
        arr->addMasAtInd(arr->getSize() - 1, "inserted_end");
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        cout << "Вставка по разным индексам за: " << duration.count() << " микросекунд" << endl;
        cout << "Проверка вставки в начало: " << arr->getElementAt(0) << endl;
        cout << "Проверка вставки в середину: " << arr->getElementAt(501) << endl;
        cout << "Размер массива: " << arr->getSize() << endl;
        cout << "Емкость: ";
        arr->getCapacity(); 
    }
    void testSearchByIndex() {
        cout << "\ngetElementAt" << endl;
        delete arr;
        arr = new Array(1000);
        for (int i = 0; i < 500; i++) {
            arr->addMasAtEnd("search_test_" + to_string(i));
        }
        auto start = high_resolution_clock::now();
        for (int i = 0; i < 100; i++) {
            int index = i * 5 % arr->getSize();
            string result = arr->getElementAt(index);
            volatile string temp = result;
        }
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        cout << "100 операций поиска за: " << duration.count() << " микросекунд" << endl;
        cout << "Среднее время на операцию: " << duration.count() / 100.0 << " микросекунд" << endl;
    }
    void testDeleteByIndex() {
        cout << "\ndeleteMasPoInd" << endl;
        delete arr;
        arr = new Array(1000);
        for (int i = 0; i < 500; i++) {
            arr->addMasAtEnd("delete_test_" + to_string(i));
        }
        int initialSize = arr->getSize();
        auto start = high_resolution_clock::now();
        for (int i = 0; i < 50; i++) {
            int indexToDelete = i * 2;
            if (indexToDelete < arr->getSize()) {
                arr->deleteMasPoInd(indexToDelete);
            }
        }
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        cout << "Удаление 50 элементов за: " << duration.count() << " микросекунд" << endl;
        cout << "Было элементов: " << initialSize << ", стало: " << arr->getSize() << endl;
    }
    void testReplaceElement() {
        cout << "\nZamenaMas" << endl;
        delete arr;
        arr = new Array(1000);
        for (int i = 0; i < 300; i++) {
            arr->addMasAtEnd("replace_test_" + to_string(i));
        }
        auto start = high_resolution_clock::now();
        for (int i = 0; i < 100; i++) {
            int index = i * 3 % arr->getSize();
            arr->ZamenaMas(index, "replaced_" + to_string(i));
        }
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        cout << "100 операций замены за: " << duration.count() << " микросекунд" << endl;
        cout << "Проверка замены [0]: " << arr->getElementAt(0) << endl;
        cout << "Проверка замены [50]: " << arr->getElementAt(50) << endl;
    }
    void testBoundaryConditions() {
        cout << "\nГраничные условия" << endl;
        delete arr;
        arr = new Array(5);
        cout << "Тест переполнения:" << endl;
        for (int i = 0; i < 7; i++) {
            arr->addMasAtEnd("test_" + to_string(i));
            cout << "Добавлен элемент " << i << ", размер: " << arr->getSize() << endl;
            cout << "Емкость: ";
            arr->getCapacity();
        }
        cout << "\nТест неверных индексов:" << endl;
        cout << "Поиск по индексу -1: ";
        arr->poiskMasPoInd(-1);
        cout << "Поиск по индексу 100: ";
        arr->poiskMasPoInd(100);
        cout << "Удаление по индексу -1: ";
        arr->deleteMasPoInd(-1);
        cout << "Замена по индексу 100: ";
        arr->ZamenaMas(100, "invalid");
    }
    void testMassiveOperations() {
        cout << "\nМассовые операции" << endl;
        delete arr;
        arr = new Array(10000);
        auto start = high_resolution_clock::now();
        for (int i = 0; i < 5000; i++) {
            arr->addMasAtEnd("massive_" + to_string(i));
        }
        for (int i = 0; i < 1000; i++) {
            if (i % 3 == 0 && arr->getSize() < arr->getCapacity() - 1) {
                arr->addMasAtInd(i % arr->getSize(), "inserted_mass");
            }
            if (i % 5 == 0 && arr->getSize() > 1) {
                arr->deleteMasPoInd(i % arr->getSize());
            }
        }
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(end - start);
        cout << "Массовые операции (5000+ элементов) за: " << duration.count() << " миллисекунд" << endl;
        cout << "Финальный размер: " << arr->getSize() << endl;
    }
public:
    ArrayBenchmark(int capacity) {
        arr = new Array(capacity);
    }
    ~ArrayBenchmark() {
        delete arr;
    }
    void runAllTests() {
        cout << "=== БЕНЧМАРК ТЕСТЫ ДЛЯ КЛАССА ARRAY ===" << endl;
        testAddAtEnd();
        testAddAtIndex();
        testSearchByIndex();
        testDeleteByIndex();
        testReplaceElement();
        testBoundaryConditions();
        testMassiveOperations();
        cout << "=== ВСЕ ТЕСТЫ ЗАВЕРШЕНЫ ===" << endl;
    }
};
int main() {
    ArrayBenchmark benchmark(10000);
    benchmark.runAllTests();
    return 0;
}
