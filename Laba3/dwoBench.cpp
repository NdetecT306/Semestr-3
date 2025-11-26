#include "dwospis.h"
#include <iostream>
#include <chrono>
#include <random>
#include <string>

using namespace std;
using namespace std::chrono;

class StudentGroupBenchmark {
private:
    StudentGroup group;
void clearGroup() {
        group.clear();
    }
    void fillGroup(int count, const string& prefix = "student") {
        for (int i = 0; i < count; i++) {
            group.addStudentToEnd(prefix + "_" + to_string(i));
        }
    }
    void testAddOperations() {
        cout << "\naddStudentToStart/End/After/Before" << endl;
        clearGroup();
        auto start = high_resolution_clock::now();
        for (int i = 0; i < 1000; i++) {
            group.addStudentToStart("start_" + to_string(i));
        }
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        cout << "Добавление 1000 элементов в начало: " << duration.count() << " микросекунд" << endl;
        cout << "Размер списка: " << (!group.isEmpty() ? "не пуст" : "пуст") << endl;
        cout << "Первый элемент: " << group.getFirst() << endl;
        cout << "Последний элемент: " << group.getLast() << endl;
        clearGroup();
        start = high_resolution_clock::now();
        for (int i = 0; i < 1000; i++) {
            group.addStudentToEnd("end_" + to_string(i));
        }
        end = high_resolution_clock::now();
        duration = duration_cast<microseconds>(end - start);
        cout << "\nДобавление 1000 элементов в конец: " << duration.count() << " микросекунд" << endl;
        cout << "Первый элемент: " << group.getFirst() << endl;
        cout << "Последний элемент: " << group.getLast() << endl;
        clearGroup();
        fillGroup(100, "base");
        start = high_resolution_clock::now();
        for (int i = 0; i < 100; i++) {
            group.addStudentAfter("base_50", "after_" + to_string(i));
        }
        end = high_resolution_clock::now();
        duration = duration_cast<microseconds>(end - start);
        cout << "\nДобавление 100 элементов после 'base_50': " << duration.count() << " микросекунд" << endl;
    }
    void testDeleteOperations() {
        cout << "\nDeleteStart/End/Before/After" << endl;
        fillGroup(1000);
        auto start = high_resolution_clock::now();
        for (int i = 0; i < 100; i++) {
            if (!group.isEmpty()) {
                group.deleteStudentFromStart();
            }
        }
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        cout << "Удаление 100 элементов из начала: " << duration.count() << " микросекунд" << endl;
        cout << "Первый элемент после удаления: " << group.getFirst() << endl;
        fillGroup(1000);
        start = high_resolution_clock::now();
        for (int i = 0; i < 100; i++) {
            if (!group.isEmpty()) {
                group.deleteStudentFromEnd();
            }
        }
        end = high_resolution_clock::now();
        duration = duration_cast<microseconds>(end - start);
        cout << "\nУдаление 100 элементов из конца: " << duration.count() << " микросекунд" << endl;
        cout << "Последний элемент после удаления: " << group.getLast() << endl;
        fillGroup(500);
        start = high_resolution_clock::now();
        for (int i = 0; i < 50; i++) {
            group.deleteStudentByValue("student_" + to_string(i));
        }
        end = high_resolution_clock::now();
        duration = duration_cast<microseconds>(end - start);
        cout << "\nУдаление 50 элементов по значению: " << duration.count() << " микросекунд" << endl;
        fillGroup(500);
        start = high_resolution_clock::now();
        for (int i = 0; i < 50; i++) {
            group.deleteStudentAfter("student_0");
        }
        end = high_resolution_clock::now();
        duration = duration_cast<microseconds>(end - start);
        cout << "\nУдаление 50 элементов после 'student_0': " << duration.count() << " микросекунд" << endl;
    }
    void testMemoryOperations() {
        cout << "\n--- Тест: Операции с памятью ---" << endl;
        clearGroup();
        auto start = high_resolution_clock::now();
        fillGroup(10000);
        auto endCreate = high_resolution_clock::now();
        auto createDuration = duration_cast<microseconds>(endCreate - start);
        cout << "Создание списка (10000 элементов): " << createDuration.count() << " микросекунд" << endl;
        start = high_resolution_clock::now();
        clearGroup();
        auto endClear = high_resolution_clock::now();
        auto clearDuration = duration_cast<microseconds>(endClear - start);
        cout << "Полная очистка списка: " << clearDuration.count() << " микросекунд" << endl;
        cout << "Список пуст: " << (group.isEmpty() ? "да" : "нет") << endl;
        fillGroup(5000);
        start = high_resolution_clock::now();
        StudentGroup copyGroup = group;
        auto endCopy = high_resolution_clock::now();
        auto copyDuration = duration_cast<microseconds>(endCopy - start);
        cout << "\nКопирование списка (5000 элементов): " << copyDuration.count() << " микросекунд" << endl;
        cout << "Оригинал пуст: " << (group.isEmpty() ? "да" : "нет") << endl;
        cout << "Копия пуста: " << (copyGroup.isEmpty() ? "да" : "нет") << endl;
    }
    void testSearchOperations() {
        cout << "\nFind" << endl;
        clearGroup();
        fillGroup(5000);
        auto start = high_resolution_clock::now();
        int foundCount = 0;
        for (int i = 0; i < 100; i++) {
            if (group.contains("student_" + to_string(i * 50))) {
                foundCount++;
            }
        }
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        cout << "Поиск 100 существующих элементов: " << duration.count() << " микросекунд" << endl;
        cout << "Найдено элементов: " << foundCount << endl;
        // Тест поиска несуществующих элементов
        start = high_resolution_clock::now();
        int notFoundCount = 0;
        for (int i = 0; i < 100; i++) {
            if (!group.contains("nonexistent_" + to_string(i))) {
                notFoundCount++;
            }
        }
        end = high_resolution_clock::now();
        duration = duration_cast<microseconds>(end - start);
        cout << "Поиск 100 несуществующих элементов: " << duration.count() << " микросекунд" << endl;
        cout << "Не найдено элементов: " << notFoundCount << endl;
    }
    void testPerformanceUnderLoad() {
        cout << "\n--- Тест: Производительность под нагрузкой ---" << endl;
        clearGroup();
        auto start = high_resolution_clock::now();
        for (int cycle = 0; cycle < 20; cycle++) {
            for (int i = 0; i < 50; i++) {
                group.addStudentToStart("cycle_start_" + to_string(cycle) + "_" + to_string(i));
            }
            for (int i = 0; i < 50; i++) {
                group.addStudentToEnd("cycle_end_" + to_string(cycle) + "_" + to_string(i));
            }
            for (int i = 0; i < 50; i++) {
                if (!group.isEmpty()) {
                    group.deleteStudentFromStart();
                }
            }
            for (int i = 0; i < 50; i++) {
                if (!group.isEmpty()) {
                    group.deleteStudentFromEnd();
                }
            }
        }
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        cout << "Циклические операции (20 циклов): " << duration.count() << " микросекунд" << endl;
        cout << "Финальный размер: " << (!group.isEmpty() ? "не пуст" : "пуст") << endl;
        clearGroup();
        fillGroup(1000);
        start = high_resolution_clock::now();
        for (int i = 0; i < 200; i++) {
            if (i % 4 == 0) {
                group.addStudentAfter("student_500", "insert_after_" + to_string(i));
            } else if (i % 4 == 1) {
                group.addStudentBefore("student_500", "insert_before_" + to_string(i));
            } else if (i % 4 == 2) {
                group.deleteStudentAfter("student_500");
            } else {
                group.deleteStudentBefore("student_500");
            }
        }
        end = high_resolution_clock::now();
        auto alternateDuration = duration_cast<microseconds>(end - start);
        cout << "\nИнтенсивные операции в середине списка (200 операций): " << alternateDuration.count() << " микросекунд" << endl;
    }
public:
    StudentGroupBenchmark() = default;
    
    void runAllTests() {
        cout << "=== БЕНЧМАРК ТЕСТЫ ДЛЯ ДВУСВЯЗНОГО СПИСКА СТУДЕНТОВ ===" << endl;
        
        testAddOperations();
        testDeleteOperations();
        testMemoryOperations();
        testSearchOperations();
        testPerformanceUnderLoad();
        
        cout << "=== ВСЕ ТЕСТЫ ДВУСВЯЗНОГО СПИСКА ЗАВЕРШЕНЫ ===" << endl;
    }
};
int main() {
    StudentGroupBenchmark benchmark;
    benchmark.runAllTests();
    return 0;
}
