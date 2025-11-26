#include <iostream>
#include <string>
#include <chrono>
#include <random>
#include "queue.h"
using namespace std;
using namespace std::chrono;
class QueueBenchmark {
private:
    Queue* head;
    Queue* tail;
public:
    QueueBenchmark() : head(nullptr), tail(nullptr) {}
    void clearQueue() {
        Queue::clearQueue(head, tail);
    }
    void testPushOperations() {
        cout << "\nPUSH" << endl;
        clearQueue();
        auto start = high_resolution_clock::now();
        for (int i = 0; i < 1000; i++) {
            Queue::QPUSH(head, tail, "element_" + to_string(i));
        }
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        cout << "Добавление 1000 элементов: " << duration.count() << " микросекунд" << endl;
        cout << "Размер очереди: " << Queue::countQueue(head) << endl;
        cout << "Среднее время на операцию: " << duration.count() / 1000.0 << " микросекунд" << endl;
        if (head && tail) {
            cout << "Head: " << head->getToy() << ", Tail: " << tail->getToy() << endl;
        }
    }
    void testPopOperations() {
        cout << "\nPOP/DELETE" << endl;
        int initialSize = Queue::countQueue(head);
        auto start = high_resolution_clock::now();
        int popCount = 0;
        for (int i = 0; i < 100; i++) {
            string value = Queue::QPOP(head, tail);
            if (value != "Очередь пуста") {
                popCount++;
            }
        }
        auto end = high_resolution_clock::now();
        auto popDuration = duration_cast<microseconds>(end - start);
        cout << "Извлечение 100 элементов (QPOP): " << popDuration.count() << " микросекунд" << endl;
        cout << "Успешно извлечено: " << popCount << " элементов" << endl;
        start = high_resolution_clock::now();
        int deleteCount = 0;
        for (int i = 0; i < 100; i++) {
            if (head != nullptr) {
                Queue::QDELETE(head, tail);
                deleteCount++;
            }
        }
        end = high_resolution_clock::now();
        auto deleteDuration = duration_cast<microseconds>(end - start);
        cout << "Удаление 100 элементов (QDELETE): " << deleteDuration.count() << " микросекунд" << endl;
        cout << "Успешно удалено: " << deleteCount << " элементов" << endl;
        cout << "Было: " << initialSize << ", осталось: " << Queue::countQueue(head) << endl;
    }
    void testMixedOperations() {
        cout << "\nСмешанные операции" << endl;
        clearQueue();
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> opDist(0, 1);  // 0: PUSH, 1: POP
        auto start = high_resolution_clock::now();
        int operations = 2000;
        int pushCount = 0, popCount = 0;
        for (int i = 0; i < operations; i++) {
            int operation = opDist(gen);
            if (operation == 0) {
                Queue::QPUSH(head, tail, "mixed_" + to_string(i));
                pushCount++;
            } else {
                if (head != nullptr) {
                    Queue::QDELETE(head, tail);
                    popCount++;
                }
            }
        }
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(end - start);
        cout << "Смешанные операции (" << operations << " операций) за: " << duration.count() << " миллисекунд" << endl;
        cout << "PUSH операций: " << pushCount << ", POP операций: " << popCount << endl;
        cout << "Финальный размер очереди: " << Queue::countQueue(head) << endl;
        if (head && tail) {
            cout << "Head: " << head->getToy() << ", Tail: " << tail->getToy() << endl;
        }
    }
    void testMemoryOperations() {
        cout << "\nMemory" << endl;
        clearQueue();
        auto start = high_resolution_clock::now();
        for (int i = 0; i < 5000; i++) {
            Queue::QPUSH(head, tail, "large_" + to_string(i));
        }
        auto endCreate = high_resolution_clock::now();
        auto createDuration = duration_cast<microseconds>(endCreate - start);
        cout << "Создание очереди (5000 элементов): " << createDuration.count() << " микросекунд" << endl;
        cout << "Размер перед очисткой: " << Queue::countQueue(head) << endl;
        start = high_resolution_clock::now();
        clearQueue();
        auto endClear = high_resolution_clock::now();
        auto clearDuration = duration_cast<microseconds>(endClear - start);
        cout << "Полная очистка очереди: " << clearDuration.count() << " микросекунд" << endl;
        cout << "Размер после очистки: " << Queue::countQueue(head) << endl;
        cout << "Head: " << (head == nullptr ? "null" : "not null") << endl;
        cout << "Tail: " << (tail == nullptr ? "null" : "not null") << endl;
    }
    ~QueueBenchmark() {
        clearQueue();
    }
    void runAllTests() {
        cout << "=== БЕНЧМАРК ТЕСТЫ ДЛЯ ОЧЕРЕДИ (QUEUE) ===" << endl;     
        testPushOperations();
        testPopOperations();
        testMixedOperations();
        testMemoryOperations();
        cout << "=== ВСЕ ТЕСТЫ ОЧЕРЕДИ ЗАВЕРШЕНЫ ===" << endl;
    }
};

int main() {
    QueueBenchmark benchmark;
    benchmark.runAllTests();
    return 0;
}
