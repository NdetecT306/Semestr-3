#include <iostream>
#include <string>
#include <chrono>
#include <random>
#include "stack.h"
using namespace std;
using namespace std::chrono;
class StackBenchmark {
private:
    Stack* stack;
public:
    StackBenchmark() : stack(nullptr) {}
    ~StackBenchmark() {
        clearStack();
    }
    void clearStack() {
        Stack::clearStack(stack);
    }
    void testPushOperations() {
        cout << "\nPUSH" << endl;
        clearStack();
        auto start = high_resolution_clock::now();
        for (int i = 0; i < 1000; i++) {
            Stack::SPUSH(stack, "element_" + to_string(i));
        }
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        cout << "Добавление 1000 элементов: " << duration.count() << " микросекунд" << endl;
        cout << "Размер стека: " << Stack::countStack(stack) << endl;
        cout << "Среднее время на операцию: " << duration.count() / 1000.0 << " микросекунд" << endl;
        if (stack != nullptr) {
            cout << "Вершина стека: " << stack->getItem() << endl;
        }
    }
    void testPopOperations() {
        cout << "\nPOP/DELETE" << endl;
        int initialSize = Stack::countStack(stack);
        auto start = high_resolution_clock::now();
        int popCount = 0;
        for (int i = 0; i < 100; i++) {
            string value = Stack::SPOP(stack);
            if (!value.empty()) {
                popCount++;
            }
        }
        auto end = high_resolution_clock::now();
        auto popDuration = duration_cast<microseconds>(end - start);
        cout << "Извлечение 100 элементов (SPOP): " << popDuration.count() << " микросекунд" << endl;
        cout << "Успешно извлечено: " << popCount << " элементов" << endl;
        start = high_resolution_clock::now();
        int deleteCount = 0;
        for (int i = 0; i < 100; i++) {
            if (stack != nullptr) {
                Stack::SDELETE(stack);
                deleteCount++;
            }
        }
        end = high_resolution_clock::now();
        auto deleteDuration = duration_cast<microseconds>(end - start);
        cout << "Удаление 100 элементов (SDELETE): " << deleteDuration.count() << " микросекунд" << endl;
        cout << "Успешно удалено: " << deleteCount << " элементов" << endl;
        cout << "Было: " << initialSize << ", осталось: " << Stack::countStack(stack) << endl;
    }
    void testMixedOperations() {
        cout << "\nСмешанные операции" << endl;
        clearStack();
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> opDist(0, 1); 
        auto start = high_resolution_clock::now();
        int operations = 2000;
        int pushCount = 0, popCount = 0;
        for (int i = 0; i < operations; i++) {
            int operation = opDist(gen);
            if (operation == 0) {
                Stack::SPUSH(stack, "mixed_" + to_string(i));
                pushCount++;
            } else {
                if (stack != nullptr) {
                    Stack::SDELETE(stack);
                    popCount++;
                }
            }
        }
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(end - start);
        cout << "Смешанные операции (" << operations << " операций) за: " << duration.count() << " миллисекунд" << endl;
        cout << "PUSH операций: " << pushCount << ", POP операций: " << popCount << endl;
        cout << "Финальный размер стека: " << Stack::countStack(stack) << endl;
        if (stack != nullptr) {
            cout << "Вершина стека: " << stack->getItem() << endl;
        }
    }
    void testMemoryOperations() {
        cout << "\nMemory" << endl;
        clearStack();
        auto start = high_resolution_clock::now();
        for (int i = 0; i < 5000; i++) {
            Stack::SPUSH(stack, "large_" + to_string(i));
        }
        auto endCreate = high_resolution_clock::now();
        auto createDuration = duration_cast<microseconds>(endCreate - start);
        cout << "Создание стека (5000 элементов): " << createDuration.count() << " микросекунд" << endl;
        cout << "Размер перед очисткой: " << Stack::countStack(stack) << endl;
        start = high_resolution_clock::now();
        clearStack();
        auto endClear = high_resolution_clock::now();
        auto clearDuration = duration_cast<microseconds>(endClear - start);
        cout << "Полная очистка стека: " << clearDuration.count() << " микросекунд" << endl;
        cout << "Размер после очистки: " << Stack::countStack(stack) << endl;
        cout << "Указатель стека: " << (stack == nullptr ? "null" : "not null") << endl;
    }
    void runAllTests() {
        cout << "=== БЕНЧМАРК ТЕСТЫ ДЛЯ СТЕКА (STACK) ===" << endl;
        testPushOperations();
        testPopOperations();
        testMixedOperations();
        testMemoryOperations();
        cout << "=== ВСЕ ТЕСТЫ СТЕКА ЗАВЕРШЕНЫ ===" << endl;
    }
    
};
int main() {
    StackBenchmark benchmark;
    benchmark.runAllTests();
    return 0;
}
