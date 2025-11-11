#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE MyTests
#include <boost/test/unit_test.hpp>
#include "stack.h"
#include "queue.h"
#include <sstream>
#include <iostream>
using namespace std;

BOOST_AUTO_TEST_CASE(AllStackPrograms)
{
    Stack* s = nullptr;
    cout << "Сейчас стек пуст" << endl;
    Stack::readStack(s);
    Stack::SDELETE(s);
    BOOST_CHECK_EQUAL(Stack::countStack(s), 0);
    BOOST_CHECK_EQUAL(Stack::SPOP(s), "");
    Stack::SPUSH(s, "First");
    BOOST_CHECK_EQUAL(Stack::countStack(s), 1);
    Stack::readStack(s);
    cout << endl;
    Stack::SPUSH(s, "Second");
    BOOST_CHECK_EQUAL(Stack::countStack(s), 2);
    Stack::readStack(s);
    cout << endl;
    Stack::SPUSH(s, "Third");
    BOOST_CHECK_EQUAL(Stack::countStack(s), 3);
    Stack::readStack(s);
    cout << endl;
    Stack::SDELETE(s);
    BOOST_CHECK_EQUAL(Stack::countStack(s), 2);
    Stack::readStack(s);
    cout << endl;
    BOOST_CHECK_EQUAL(Stack::SPOP(s), "Second");
    BOOST_CHECK_EQUAL(Stack::countStack(s), 1);
    Stack::readStack(s);
    cout << endl;
    BOOST_CHECK_EQUAL(Stack::SPOP(s), "First");
    BOOST_CHECK_EQUAL(Stack::countStack(s), 0);
    Stack::readStack(s);
    cout << endl;
    Stack::SPUSH(s, "A");
    Stack::SPUSH(s, "B");
    Stack::readStack(s);
    cout << endl;
    BOOST_CHECK_EQUAL(Stack::SPOP(s), "B"); 
    BOOST_CHECK_EQUAL(Stack::SPOP(s), "A");
    BOOST_CHECK_EQUAL(Stack::countStack(s), 0);
    Stack::readStack(s);
    cout << endl;
    Stack::SPUSH(s, "El1");
    Stack::SPUSH(s, "El2");
    Stack::SPUSH(s, "El3");
    Stack::SPUSH(s, "El4");
    Stack::readStack(s);
    cout << endl;
    BOOST_CHECK_EQUAL(Stack::countStack(s), 4);
    Stack::clearStack(s);
    Stack::readStack(s); 
    cout << endl;
    BOOST_CHECK_EQUAL(Stack::countStack(s), 0);
    BOOST_CHECK_EQUAL(Stack::SPOP(s), "");
    Stack::clearStack(s); // Не должно быть ошибок
    Stack::readStack(s);  // Должен вывести сообщение о пустом стеке
    cout << endl;
    Stack::SPUSH(s, "NewElement");
    BOOST_CHECK_EQUAL(Stack::countStack(s), 1);
    BOOST_CHECK_EQUAL(Stack::SPOP(s), "NewElement");
    BOOST_CHECK_EQUAL(Stack::countStack(s), 0);
}
BOOST_AUTO_TEST_CASE(AllQueuePrograms)
{
    Queue* head = nullptr;
    Queue* tail = nullptr;
    cout << "Сейчас очередь пуста" << endl;
    Queue::readQueue(head);
    Queue::QDELETE(head);
    BOOST_CHECK_EQUAL(Queue::countQueue(head), 0);
    BOOST_CHECK_EQUAL(Queue::QPOP(head), "Очередь пуста");
    // Добавляем элементы в очередь
    Queue::QPUSH(head, tail, "First");
    BOOST_CHECK_EQUAL(Queue::countQueue(head), 1);
    Queue::readQueue(head);
    cout << endl;
    Queue::QPUSH(head, tail, "Second");
    BOOST_CHECK_EQUAL(Queue::countQueue(head), 2);
    Queue::readQueue(head);
    cout << endl;
    
    Queue::QPUSH(head, tail, "Third");
    BOOST_CHECK_EQUAL(Queue::countQueue(head), 3);
    Queue::readQueue(head);
    cout << endl;
    
    // Удаляем один элемент (должен удалиться "First")
    Queue::QDELETE(head);
    BOOST_CHECK_EQUAL(Queue::countQueue(head), 2);
    Queue::readQueue(head);
    cout << endl;
    
    // Извлекаем элементы (очередь FIFO - First In First Out)
    BOOST_CHECK_EQUAL(Queue::QPOP(head), "Second");  // Теперь первый элемент "Second"
    BOOST_CHECK_EQUAL(Queue::countQueue(head), 1);
    Queue::readQueue(head);
    cout << endl;
    
    BOOST_CHECK_EQUAL(Queue::QPOP(head), "Third");   // Последний элемент "Third"
    BOOST_CHECK_EQUAL(Queue::countQueue(head), 0);
    Queue::readQueue(head);
    cout << endl;
    
    // Добавляем новые элементы
    Queue::QPUSH(head, tail, "A");
    Queue::QPUSH(head, tail, "B");
    Queue::readQueue(head);
    cout << endl;
    
    // Проверяем порядок FIFO
    BOOST_CHECK_EQUAL(Queue::QPOP(head), "A");  // Первым уходит "A"
    BOOST_CHECK_EQUAL(Queue::QPOP(head), "B");  // Затем "B"
    BOOST_CHECK_EQUAL(Queue::countQueue(head), 0);
    Queue::readQueue(head);
    cout << endl;
    
    // Добавляем несколько элементов
    Queue::QPUSH(head, tail, "El1");
    Queue::QPUSH(head, tail, "El2");
    Queue::QPUSH(head, tail, "El3");
    Queue::QPUSH(head, tail, "El4");
    Queue::readQueue(head);
    cout << endl;
    
    BOOST_CHECK_EQUAL(Queue::countQueue(head), 4);
    
    // Очищаем очередь
    while (head != nullptr) {
        Queue::QDELETE(head);
    }
    
    BOOST_CHECK_EQUAL(Queue::countQueue(head), 0);
    BOOST_CHECK_EQUAL(Queue::QPOP(head), "Очередь пуста");
    Queue::readQueue(head);  // Должен вывести сообщение о пустой очереди
    cout << endl;
    
    // Финальная проверка
    Queue::QPUSH(head, tail, "NewElement");
    BOOST_CHECK_EQUAL(Queue::countQueue(head), 1);
    BOOST_CHECK_EQUAL(Queue::QPOP(head), "NewElement");
    BOOST_CHECK_EQUAL(Queue::countQueue(head), 0);
}
