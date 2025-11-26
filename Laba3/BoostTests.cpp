#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE MyTests
#include <boost/test/unit_test.hpp>
#include <boost/timer/timer.hpp>
#include <chrono>
#include <random>
#include "stack.h"
#include "queue.h"
#include "array.h"
#include "odspis.h"
#include "hash.h"
#include "tree.h"
#include "dwospis.h"
#include <sstream>
#include <iostream>
using namespace std;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////Стек
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
    Stack::clearStack(s); 
    Stack::readStack(s); 
    cout << endl;
    Stack::SPUSH(s, "NewElement");
    BOOST_CHECK_EQUAL(Stack::countStack(s), 1);
    BOOST_CHECK_EQUAL(Stack::SPOP(s), "NewElement");
    BOOST_CHECK_EQUAL(Stack::countStack(s), 0);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////МАССИВ
BOOST_AUTO_TEST_CASE(AllArrayPrograms)
{
    {
        Array* arr = new Array(5);
        BOOST_CHECK_NO_THROW(arr->readMas()); // Должен вывести сообщение о пустом массиве
        BOOST_CHECK_NO_THROW(arr->RazmerMas()); // Размер должен быть 0
        BOOST_CHECK_NO_THROW(arr->getCapacity()); // Емкость должна быть 5
        BOOST_CHECK_NO_THROW(arr->deleteMasPoInd(0)); // Должна быть ошибка
        BOOST_CHECK_NO_THROW(arr->poiskMasPoInd(0)); // Должна быть ошибка
        arr->addMasAtEnd("First");
        BOOST_CHECK_NO_THROW(arr->RazmerMas());
        BOOST_CHECK_NO_THROW(arr->readMas());
        arr->addMasAtEnd("Second");
        BOOST_CHECK_NO_THROW(arr->RazmerMas());
        BOOST_CHECK_NO_THROW(arr->readMas());
        arr->addMasAtEnd("Third");
        BOOST_CHECK_NO_THROW(arr->RazmerMas());
        BOOST_CHECK_NO_THROW(arr->readMas());
        delete arr;
    }
    {
        Array* arr = new Array(5);
        arr->addMasAtEnd("First");
        arr->addMasAtEnd("Second");
        arr->addMasAtEnd("Third");
        BOOST_CHECK_NO_THROW(arr->addMasAtInd(1, "Inserted"));
        BOOST_CHECK_NO_THROW(arr->RazmerMas());
        BOOST_CHECK_NO_THROW(arr->readMas());
        BOOST_CHECK_NO_THROW(arr->poiskMasPoInd(2)); 
        BOOST_CHECK_NO_THROW(arr->ZamenaMas(0, "Replaced"));
        BOOST_CHECK_NO_THROW(arr->RazmerMas());
        BOOST_CHECK_NO_THROW(arr->readMas());
        delete arr;
    }
    {
        Array* arr = new Array(5);
        arr->addMasAtEnd("First");
        arr->addMasAtEnd("Inserted");
        arr->addMasAtEnd("Second");
        BOOST_CHECK_NO_THROW(arr->poiskMasPoInd(10)); // Должна быть ошибка
        BOOST_CHECK_NO_THROW(arr->deleteMasPoInd(-1)); // Должна быть ошибка
        BOOST_CHECK_NO_THROW(arr->ZamenaMas(5, "Invalid")); // Должна быть ошибка
        BOOST_CHECK_NO_THROW(arr->deleteMasPoInd(1)); // Удаляем "Inserted"
        BOOST_CHECK_NO_THROW(arr->RazmerMas());
        BOOST_CHECK_NO_THROW(arr->readMas());
        delete arr;
    }
    {
        Array* arr = new Array(3);
        arr->addMasAtEnd("First");
        arr->addMasAtEnd("Second");
        BOOST_CHECK_NO_THROW(arr->addMasAtEnd("Third"));
        BOOST_CHECK_NO_THROW(arr->RazmerMas());
        BOOST_CHECK_NO_THROW(arr->readMas());
        BOOST_CHECK_NO_THROW(arr->addMasAtEnd("Fourth")); // Должна быть ошибка переполнения
        BOOST_CHECK_NO_THROW(arr->RazmerMas()); // Размер не должен измениться
        delete arr;
    }
    {
        Array* arr = new Array(3);
        arr->addMasAtEnd("First");
        arr->addMasAtEnd("Second");
        arr->addMasAtEnd("Third");
        BOOST_CHECK_NO_THROW(arr->deleteMasPoInd(0));
        BOOST_CHECK_NO_THROW(arr->RazmerMas());
        BOOST_CHECK_NO_THROW(arr->deleteMasPoInd(0));
        BOOST_CHECK_NO_THROW(arr->RazmerMas());
        BOOST_CHECK_NO_THROW(arr->deleteMasPoInd(0));
        BOOST_CHECK_NO_THROW(arr->RazmerMas());
        BOOST_CHECK_NO_THROW(arr->addMasAtEnd("NewStart"));
        BOOST_CHECK_NO_THROW(arr->RazmerMas());
        BOOST_CHECK_NO_THROW(arr->readMas());
        delete arr;
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////ОЧЕРЕДЬ
BOOST_AUTO_TEST_CASE(AllQueuePrograms)
{
    Queue* head = nullptr;
    Queue* tail = nullptr;
    cout << "Сейчас очередь пуста: ";
    Queue::readQueue(head);
    Queue::QDELETE(head, tail);
    BOOST_CHECK_EQUAL(Queue::countQueue(head), 0);
    BOOST_CHECK_EQUAL(Queue::QPOP(head, tail), "Очередь пуста");
    Queue::QPUSH(head, tail, "First");
    BOOST_CHECK_EQUAL(Queue::countQueue(head), 1);
    cout << "После добавления First: ";
    Queue::readQueue(head);
    Queue::QPUSH(head, tail, "Second");
    BOOST_CHECK_EQUAL(Queue::countQueue(head), 2);
    cout << "После добавления Second: ";
    Queue::readQueue(head);
    Queue::QPUSH(head, tail, "Third");
    BOOST_CHECK_EQUAL(Queue::countQueue(head), 3);
    cout << "После добавления Third: ";
    Queue::readQueue(head);
    Queue::QDELETE(head, tail);
    BOOST_CHECK_EQUAL(Queue::countQueue(head), 2);
    cout << "После удаления первого элемента: ";
    Queue::readQueue(head);
    BOOST_CHECK_EQUAL(Queue::QPOP(head, tail), "Second");
    BOOST_CHECK_EQUAL(Queue::countQueue(head), 1);
    cout << "После извлечения Second: ";
    Queue::readQueue(head);
    BOOST_CHECK_EQUAL(Queue::QPOP(head, tail), "Third");
    BOOST_CHECK_EQUAL(Queue::countQueue(head), 0);
    cout << "После извлечения Third: ";
    Queue::readQueue(head);
    Queue::QPUSH(head, tail, "A");
    Queue::QPUSH(head, tail, "B");
    cout << "Добавлены A и B: ";
    Queue::readQueue(head);
    BOOST_CHECK_EQUAL(Queue::QPOP(head, tail), "A");
    BOOST_CHECK_EQUAL(Queue::QPOP(head, tail), "B");
    BOOST_CHECK_EQUAL(Queue::countQueue(head), 0);
    cout << "После извлечения A и B: ";
    Queue::readQueue(head);
    Queue::QPUSH(head, tail, "El1");
    Queue::QPUSH(head, tail, "El2");
    Queue::QPUSH(head, tail, "El3");
    Queue::QPUSH(head, tail, "El4");
    cout << "Добавлены 4 элемента: ";
    Queue::readQueue(head);
    BOOST_CHECK_EQUAL(Queue::countQueue(head), 4);
    Queue::clearQueue(head, tail);
    BOOST_CHECK_EQUAL(Queue::countQueue(head), 0);
    BOOST_CHECK_EQUAL(Queue::QPOP(head, tail), "Очередь пуста");
    cout << "После очистки: ";
    Queue::readQueue(head);
    Queue::QPUSH(head, tail, "NewElement");
    BOOST_CHECK_EQUAL(Queue::countQueue(head), 1);
    BOOST_CHECK_EQUAL(Queue::QPOP(head, tail), "NewElement");
    BOOST_CHECK_EQUAL(Queue::countQueue(head), 0);
    Queue::clearQueue(head, tail);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////ОДНОСВЯЗНЫЙ СПИСОК
BOOST_AUTO_TEST_SUITE(OdSpisTestSuite)
BOOST_AUTO_TEST_CASE(ConstructorTest) { //Конструктор
    OdSpis* list = new OdSpis("First");
    BOOST_CHECK_EQUAL(list->SizeList(list), 1);
    BOOST_CHECK_EQUAL(list->getName(), "First");
    list->cleanupList(list);
}
BOOST_AUTO_TEST_CASE(AddToBeginTest) { //AddToBegin
    OdSpis* list = new OdSpis("First");
    list->AddToBegin(list, "NewFirst");
    BOOST_CHECK_EQUAL(list->SizeList(list), 2);
    BOOST_CHECK_EQUAL(list->poisk(list, "NewFirst"), true);
    BOOST_CHECK_EQUAL(list->poisk(list, "First"), true);
    BOOST_CHECK_EQUAL(list->getName(), "NewFirst");
    list->cleanupList(list);
}
BOOST_AUTO_TEST_CASE(AddToEndTest) { //AddToEnd
    OdSpis* list = new OdSpis("First");
    list->AddToEnd(list, "Second");
    list->AddToEnd(list, "Third");
    BOOST_CHECK_EQUAL(list->SizeList(list), 3);
    BOOST_CHECK_EQUAL(list->poisk(list, "First"), true);
    BOOST_CHECK_EQUAL(list->poisk(list, "Second"), true);
    BOOST_CHECK_EQUAL(list->poisk(list, "Third"), true);
    list->cleanupList(list);
}
BOOST_AUTO_TEST_CASE(AddChelDoTest) { //AddChelDo
    OdSpis* list = new OdSpis("Second");
    list->AddToEnd(list, "Third");
    OdSpis* target = list->FindElement(list, "Second");
    BOOST_REQUIRE(target != nullptr);
    list->AddChelDo(list, target, "First");
    BOOST_CHECK_EQUAL(list->SizeList(list), 3);
    BOOST_CHECK_EQUAL(list->poisk(list, "First"), true);
    BOOST_CHECK_EQUAL(list->getName(), "First"); 
    BOOST_CHECK_EQUAL(list->getName(), "First");
    BOOST_CHECK_EQUAL(list->getNext()->getName(), "Second");
    BOOST_CHECK_EQUAL(list->getNext()->getNext()->getName(), "Third");
    list->cleanupList(list);
}
BOOST_AUTO_TEST_CASE(AddChelPosleTest) { //AddPosle
    OdSpis* list = new OdSpis("First");
    list->AddToEnd(list, "Third");
    OdSpis* target = list->FindElement(list, "First");
    BOOST_REQUIRE(target != nullptr);
    list->AddChelPosle(target, "Second");
    BOOST_CHECK_EQUAL(list->SizeList(list), 3);
    BOOST_CHECK_EQUAL(list->poisk(list, "Second"), true);
    // Проверяем порядок
    BOOST_CHECK_EQUAL(target->getNext()->getName(), "Second");
    list->cleanupList(list);
}
BOOST_AUTO_TEST_CASE(AccessMethodsTest) { 
    OdSpis* node1 = new OdSpis("First");
    OdSpis* node2 = new OdSpis("Second");
    node1->setNext(node2);
    BOOST_CHECK_EQUAL(node1->getName(), "First");
    BOOST_CHECK_EQUAL(node1->getNext(), node2);
    BOOST_CHECK_EQUAL(node2->getName(), "Second");
    BOOST_CHECK_EQUAL(node2->getNext(), nullptr);
    delete node1;
    delete node2;
}
BOOST_AUTO_TEST_CASE(FindElementTest) { //Find
    OdSpis* list = new OdSpis("First");
    list->AddToEnd(list, "Second");
    list->AddToEnd(list, "Third");
    OdSpis* found = list->FindElement(list, "Second");
    BOOST_REQUIRE(found != nullptr);
    BOOST_CHECK_EQUAL(found->getName(), "Second");
    OdSpis* notFound = list->FindElement(list, "NonExistent");
    BOOST_CHECK(notFound == nullptr);
    list->cleanupList(list);
}
BOOST_AUTO_TEST_CASE(DeletePoZnachTest) { //DeletePoZnach
    OdSpis* list = new OdSpis("First");
    list->AddToEnd(list, "Second");
    list->AddToEnd(list, "Third");
    BOOST_CHECK_EQUAL(list->SizeList(list), 3);
    list->deletePoZnach(list, "Second");
    BOOST_CHECK_EQUAL(list->SizeList(list), 2);
    BOOST_CHECK_EQUAL(list->poisk(list, "Second"), false);
    BOOST_CHECK_EQUAL(list->poisk(list, "First"), true);
    BOOST_CHECK_EQUAL(list->poisk(list, "Third"), true);
    list->cleanupList(list);
}
BOOST_AUTO_TEST_CASE(DeleteToBeginTest) { //DeleteToBegin
    OdSpis* list = new OdSpis("First");
    list->AddToEnd(list, "Second");
    list->AddToEnd(list, "Third");
    list->deleteToBegin(list);
    BOOST_CHECK_EQUAL(list->SizeList(list), 2);
    BOOST_CHECK_EQUAL(list->poisk(list, "First"), false);
    BOOST_CHECK_EQUAL(list->getName(), "Second");
    list->cleanupList(list);
}
BOOST_AUTO_TEST_CASE(DeleteToEndTest) { //DeleteToEnd
    OdSpis* list = new OdSpis("First");
    list->AddToEnd(list, "Second");
    list->AddToEnd(list, "Third");
    list->deleteToEnd(list);
    BOOST_CHECK_EQUAL(list->SizeList(list), 2);
    BOOST_CHECK_EQUAL(list->poisk(list, "Third"), false);
    BOOST_CHECK_EQUAL(list->poisk(list, "First"), true);
    BOOST_CHECK_EQUAL(list->poisk(list, "Second"), true);
    list->cleanupList(list);
}
BOOST_AUTO_TEST_CASE(AddChelDoNotFoundTest) { //Добавление за границей
    OdSpis* list = new OdSpis("First");
    list->AddToEnd(list, "Second");
    OdSpis* nonExistent = new OdSpis("NonExistent");
    list->AddChelDo(list, nonExistent, "NewElement");
    BOOST_CHECK_EQUAL(list->SizeList(list), 2);
    BOOST_CHECK_EQUAL(list->poisk(list, "NewElement"), false);
    delete nonExistent;
    list->cleanupList(list);
}
BOOST_AUTO_TEST_CASE(DeleteChelDoTest) { //DeleteChelDo
    OdSpis* list = new OdSpis("First");
    list->AddToEnd(list, "Second");
    list->AddToEnd(list, "Third");
    list->AddToEnd(list, "Fourth");
    OdSpis* target = list->FindElement(list, "Third");
    BOOST_REQUIRE(target != nullptr);
    list->deleteChelDo(list, target);
    BOOST_CHECK_EQUAL(list->SizeList(list), 3);
    BOOST_CHECK_EQUAL(list->poisk(list, "Second"), false);
    BOOST_CHECK_EQUAL(list->getName(), "First");
    BOOST_CHECK_EQUAL(list->getNext()->getName(), "Third");
    BOOST_CHECK_EQUAL(list->getNext()->getNext()->getName(), "Fourth");
    list->cleanupList(list);
}
BOOST_AUTO_TEST_CASE(DeleteChelPosleTest) { //DeketeChelPosle
    OdSpis* list = new OdSpis("First");
    list->AddToEnd(list, "Second");
    list->AddToEnd(list, "Third");
    list->AddToEnd(list, "Fourth");
    OdSpis* target = list->FindElement(list, "Second");
    BOOST_REQUIRE(target != nullptr);

    list->deleteChelPosle(target);
    BOOST_CHECK_EQUAL(list->SizeList(list), 3);
    BOOST_CHECK_EQUAL(list->poisk(list, "Third"), false);
    BOOST_CHECK_EQUAL(list->getName(), "First");
    BOOST_CHECK_EQUAL(list->getNext()->getName(), "Second");
    BOOST_CHECK_EQUAL(list->getNext()->getNext()->getName(), "Fourth");
    list->cleanupList(list);
}
BOOST_AUTO_TEST_CASE(ComplexAddDeleteAroundTest) { //Добавляем и удаляем
    OdSpis* list = new OdSpis("Start");
    list->AddToEnd(list, "End");
    OdSpis* target = list->FindElement(list, "End");
    BOOST_REQUIRE(target != nullptr);
    list->AddChelDo(list, target, "Middle");
    BOOST_CHECK_EQUAL(list->SizeList(list), 3);
    BOOST_CHECK_EQUAL(list->getName(), "Start");
    BOOST_CHECK_EQUAL(list->getNext()->getName(), "Middle");
    BOOST_CHECK_EQUAL(list->getNext()->getNext()->getName(), "End");
    list->deleteChelDo(list, target);
    BOOST_CHECK_EQUAL(list->SizeList(list), 2);
    BOOST_CHECK_EQUAL(list->poisk(list, "Middle"), false);
    OdSpis* start = list->FindElement(list, "Start");
    list->deleteChelPosle(start);
    BOOST_CHECK_EQUAL(list->SizeList(list), 1);
    BOOST_CHECK_EQUAL(list->poisk(list, "End"), false);
    BOOST_CHECK_EQUAL(list->getName(), "Start");
    list->cleanupList(list);
}
BOOST_AUTO_TEST_CASE(PrintEmptyListNoCrashTest) { //Пустой
    OdSpis* list = nullptr;
    OdSpis printer;
    BOOST_CHECK_NO_THROW(printer.print(list));
    list->cleanupList(list);
}
BOOST_AUTO_TEST_CASE(PrintPreservesSingleElementTest) { //Один
    OdSpis* list = new OdSpis("Single");
    string originalName = list->getName();
    OdSpis* originalNext = list->getNext();
    list->print(list);
    BOOST_CHECK_EQUAL(list->getName(), originalName);
    BOOST_CHECK_EQUAL(list->getNext(), originalNext);
    BOOST_CHECK_EQUAL(list->SizeList(list), 1);
    list->cleanupList(list);
}
BOOST_AUTO_TEST_CASE(PrintPreservesMultipleElementsTest) { //Н2 и более
    OdSpis* list = new OdSpis("First");
    list->AddToEnd(list, "Second");
    list->AddToEnd(list, "Third");
    int originalSize = list->SizeList(list);
    string firstName = list->getName();
    string secondName = list->getNext()->getName();
    string thirdName = list->getNext()->getNext()->getName();
    list->print(list);
    BOOST_CHECK_EQUAL(list->SizeList(list), originalSize);
    BOOST_CHECK_EQUAL(list->getName(), firstName);
    BOOST_CHECK_EQUAL(list->getNext()->getName(), secondName);
    BOOST_CHECK_EQUAL(list->getNext()->getNext()->getName(), thirdName);
    list->cleanupList(list);
}
BOOST_AUTO_TEST_CASE(PrintAfterAddOperationsTest) { //Работа после добал=вления
    OdSpis* list = new OdSpis("Initial");
    list->AddToBegin(list, "NewFirst");
    list->AddToEnd(list, "Last");
    BOOST_CHECK_NO_THROW(list->print(list));
    BOOST_CHECK_EQUAL(list->SizeList(list), 3);
    BOOST_CHECK_EQUAL(list->getName(), "NewFirst");
    BOOST_CHECK_EQUAL(list->getNext()->getName(), "Initial");
    BOOST_CHECK_EQUAL(list->getNext()->getNext()->getName(), "Last");
    list->cleanupList(list);
}
BOOST_AUTO_TEST_CASE(PrintAfterDeleteOperationsTest) { //После удаления
    OdSpis* list = new OdSpis("One");
    list->AddToEnd(list, "Two");
    list->AddToEnd(list, "Three");
    list->deletePoZnach(list, "Two");
    BOOST_CHECK_NO_THROW(list->print(list));
    BOOST_CHECK_EQUAL(list->SizeList(list), 2);
    BOOST_CHECK_EQUAL(list->getName(), "One");
    BOOST_CHECK_EQUAL(list->getNext()->getName(), "Three");
    list->cleanupList(list);
}
BOOST_AUTO_TEST_SUITE_END()
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////ХЭШ
BOOST_AUTO_TEST_SUITE(DoubleHashTableTest)
BOOST_AUTO_TEST_CASE(AddAndRetrieveTest)
{
    DoubleHashTable table;
    table.INIT_HASH_TABLE();
    BOOST_CHECK(table.ADD(1, "value1") == true);
    BOOST_CHECK(table.ADD(2, "value2") == true);
    BOOST_CHECK(table.ADD(3, "value3") == true);
    BOOST_CHECK(table.SET_AT(1, "value1") == true);
    BOOST_CHECK(table.SET_AT(2, "value2") == true);
    BOOST_CHECK(table.SET_AT(3, "value3") == true);
    BOOST_CHECK(table.contains(1, "value1") == true);
    BOOST_CHECK(table.contains(2, "value2") == true);
    BOOST_CHECK(table.SET_AT(1, "wrong_value") == false);
    BOOST_CHECK(table.SET_AT(999, "nonexistent") == false);
}
BOOST_AUTO_TEST_CASE(AddDuplicateTest)
{
    DoubleHashTable table;
    table.INIT_HASH_TABLE();
    BOOST_CHECK(table.ADD(1, "value1") == true);
    BOOST_CHECK(table.ADD(1, "value1") == false);
    BOOST_CHECK(table.ADD(1, "value2") == false); 
}
BOOST_AUTO_TEST_CASE(RemoveTest)
{
    DoubleHashTable table;
    table.INIT_HASH_TABLE();
    BOOST_CHECK(table.ADD(1, "value1") == true);
    BOOST_CHECK(table.SETDEL(1, "value1") == true);
    BOOST_CHECK(table.SET_AT(1, "value1") == false);
    BOOST_CHECK(table.contains(1, "value1") == false);
    table.ADD(2, "value2");
    BOOST_CHECK(table.remove(2, "value2") == true);
    BOOST_CHECK(table.contains(2, "value2") == false);
    BOOST_CHECK(table.remove(1, "value1") == false);
}
BOOST_AUTO_TEST_CASE(RemoveNonExistentTest)
{
    DoubleHashTable table;
    table.INIT_HASH_TABLE();
    BOOST_CHECK(table.SETDEL(999, "nonexistent") == false);
    BOOST_CHECK(table.remove(999, "nonexistent") == false);
    table.ADD(5, "correct_value");
    BOOST_CHECK(table.remove(5, "wrong_value") == false);
    BOOST_CHECK(table.contains(5, "correct_value") == true); 
}
BOOST_AUTO_TEST_CASE(GetAllElementsTest)
{
    DoubleHashTable table;
    table.INIT_HASH_TABLE();
    table.ADD(1, "value1");
    table.ADD(2, "value2");
    table.ADD(3, "value3");
    table.remove(2, "value2");
    int keys[10];
    string values[10];
    int count = 0;
    table.getAllElements(keys, values, count);
    BOOST_CHECK(count == 2);
    bool found1 = false, found3 = false, found2 = false;
    for (int i = 0; i < count; i++) {
        if (keys[i] == 1 && values[i] == "value1") found1 = true;
        if (keys[i] == 3 && values[i] == "value3") found3 = true;
        if (keys[i] == 2) found2 = true; 
    }
    BOOST_CHECK(found1 == true);
    BOOST_CHECK(found3 == true);
    BOOST_CHECK(found2 == false);
}
BOOST_AUTO_TEST_CASE(CollisionHandlingTest)
{
    DoubleHashTable table;
    table.INIT_HASH_TABLE();
    BOOST_CHECK(table.ADD(1, "value1") == true);
    BOOST_CHECK(table.ADD(257, "value257") == true); // 257 % 256 = 1
    BOOST_CHECK(table.ADD(513, "value513") == true); // 513 % 256 = 1
    BOOST_CHECK(table.contains(1, "value1") == true);
    BOOST_CHECK(table.contains(257, "value257") == true);
    BOOST_CHECK(table.contains(513, "value513") == true);
}
BOOST_AUTO_TEST_CASE(TableFullTest)
{
    DoubleHashTable table;
    table.INIT_HASH_TABLE();
    for (int i = 0; i < HASH_SIZE; i++) {
        BOOST_CHECK(table.ADD(i, "value" + to_string(i)) == true);
    }
    BOOST_CHECK(table.ADD(1000, "overflow") == false);
    BOOST_CHECK(table.remove(100, "value100") == true);
    BOOST_CHECK(table.ADD(1000, "overflow") == true);
}
BOOST_AUTO_TEST_CASE(InsertWrapperTest)
{
    DoubleHashTable table;
    table.INIT_HASH_TABLE();
    BOOST_CHECK(table.insert(5, "test_value") == true);
    BOOST_CHECK(table.contains(5, "test_value") == true);
    BOOST_CHECK(table.insert(5, "test_value") == false);
}
BOOST_AUTO_TEST_CASE(TableSizeTest)
{
    DoubleHashTable table;
    BOOST_CHECK(table.size() == HASH_SIZE);
    BOOST_CHECK(table.size() == 256);
}
BOOST_AUTO_TEST_CASE(ComplexScenarioTest)
{
    DoubleHashTable table;
    table.INIT_HASH_TABLE();
    table.ADD(1, "one");
    table.ADD(2, "two");
    table.ADD(3, "three");
    table.remove(2, "two");
    BOOST_CHECK(table.contains(1, "one") == true);
    BOOST_CHECK(table.contains(2, "two") == false);
    BOOST_CHECK(table.contains(3, "three") == true);
    BOOST_CHECK(table.ADD(4, "four") == true);
    int keys[10];
    string values[10];
    int count = 0;
    table.getAllElements(keys, values, count);
    BOOST_CHECK(count == 3); // 1, 3, 4
    BOOST_CHECK(table.ADD(2, "new_two") == true);
    table.getAllElements(keys, values, count);
    BOOST_CHECK(count == 4); // 1, 2, 3, 4
}
BOOST_AUTO_TEST_CASE(EmptyTableTest)
{
    DoubleHashTable table;
    table.INIT_HASH_TABLE();
    int keys[10];
    string values[10];
    int count = 0;
    table.getAllElements(keys, values, count);
    BOOST_CHECK(count == 0);
    BOOST_CHECK(table.remove(1, "value") == false);
    BOOST_CHECK(table.contains(1, "value") == false);
}
BOOST_AUTO_TEST_CASE(ContainsKeyTest)
{
    DoubleHashTable table;
    table.INIT_HASH_TABLE();
    table.ADD(1, "value1");
    table.ADD(2, "value2");
    table.ADD(3, "value3");
    BOOST_CHECK(table.containsKey(1) == true);
    BOOST_CHECK(table.containsKey(2) == true);
    BOOST_CHECK(table.containsKey(3) == true);
    BOOST_CHECK(table.containsKey(999) == false);
    BOOST_CHECK(table.containsKey(0) == false);
    table.remove(2, "value2");
    BOOST_CHECK(table.containsKey(2) == false);
    BOOST_CHECK(table.containsKey(1) == true);
    BOOST_CHECK(table.containsKey(3) == true);
}

BOOST_AUTO_TEST_CASE(ElementCountTest)
{
    DoubleHashTable table;
    table.INIT_HASH_TABLE();
    BOOST_CHECK(table.elementCount() == 0);
    table.ADD(1, "value1");
    BOOST_CHECK(table.elementCount() == 1);
    table.ADD(2, "value2");
    BOOST_CHECK(table.elementCount() == 2);
    table.ADD(3, "value3");
    BOOST_CHECK(table.elementCount() == 3);
    table.remove(2, "value2");
    BOOST_CHECK(table.elementCount() == 2);
    table.ADD(4, "value4");
    BOOST_CHECK(table.elementCount() == 3);
    table.remove(1, "value1");
    table.remove(3, "value3");
    table.remove(4, "value4");
    BOOST_CHECK(table.elementCount() == 0);
}
BOOST_AUTO_TEST_CASE(ContainsKeyWithCollisionsTest)
{
    DoubleHashTable table;
    table.INIT_HASH_TABLE();
    BOOST_CHECK(table.ADD(1, "value1") == true);
    BOOST_CHECK(table.ADD(257, "value257") == true); // 257 % 256 = 1
    BOOST_CHECK(table.ADD(513, "value513") == true); // 513 % 256 = 1
    BOOST_CHECK(table.containsKey(1) == true);
    BOOST_CHECK(table.containsKey(257) == true);
    BOOST_CHECK(table.containsKey(513) == true);
    BOOST_CHECK(table.elementCount() == 3);
}
BOOST_AUTO_TEST_CASE(ElementCountVsGetAllElementsTest)
{
    DoubleHashTable table;
    table.INIT_HASH_TABLE();
    table.ADD(10, "ten");
    table.ADD(20, "twenty");
    table.ADD(30, "thirty");
    int keys[10];
    string values[10];
    int count = 0;
    table.getAllElements(keys, values, count);
    BOOST_CHECK(table.elementCount() == count);
    BOOST_CHECK(table.elementCount() == 3);
    BOOST_CHECK(count == 3);
    table.remove(20, "twenty");
    table.getAllElements(keys, values, count);
    BOOST_CHECK(table.elementCount() == count);
    BOOST_CHECK(table.elementCount() == 2);
    BOOST_CHECK(count == 2);
}
BOOST_AUTO_TEST_SUITE_END()
////////////////////////////////////////////////////////////////////////////////////////////////////////////////// ДЕРЕВО
BOOST_AUTO_TEST_SUITE(RedBlackTreeTests)
BOOST_AUTO_TEST_CASE(CreateRootTest) //CreateRoot
{
    Tree tree;
    Tree* root = tree.CreateRoot(10);
    BOOST_CHECK(root != nullptr);
    BOOST_CHECK_EQUAL(root->getValue(), 10);
    BOOST_CHECK(root->getColor() == Tree::BLACK);
    BOOST_CHECK(root->getLeft() == nullptr);
    BOOST_CHECK(root->getRight() == nullptr);
    BOOST_CHECK(root->getParent() == nullptr);
    delete root;
}
BOOST_AUTO_TEST_CASE(CreateNodeTest) //CreateNode
{
    Tree tree;
    Tree* node = tree.CreateNode(5);
    BOOST_CHECK(node != nullptr);
    BOOST_CHECK_EQUAL(node->getValue(), 5);
    BOOST_CHECK(node->getColor() == Tree::RED);
    BOOST_CHECK(node->getLeft() == nullptr);
    BOOST_CHECK(node->getRight() == nullptr);
    BOOST_CHECK(node->getParent() == nullptr);
    delete node;
}
BOOST_AUTO_TEST_CASE(AddLeafTest)//AddLeaf
{
    Tree tree;
    Tree* root = nullptr;
    root = tree.AddLeaf(root, 10);
    BOOST_CHECK(root != nullptr);
    BOOST_CHECK_EQUAL(root->getValue(), 10);
    BOOST_CHECK(root->getColor() == Tree::BLACK);
    root = tree.AddLeaf(root, 5);
    BOOST_CHECK(tree.ifZnachTreeExist(root, 5));
    root = tree.AddLeaf(root, 15);
    BOOST_CHECK(tree.ifZnachTreeExist(root, 15));
    BOOST_CHECK(root->getLeft() != nullptr);
    BOOST_CHECK(root->getRight() != nullptr);
    tree.deleteNode(root, 10);
    tree.deleteNode(root, 5);
    tree.deleteNode(root, 15);
}
BOOST_AUTO_TEST_CASE(SearchTest)//Search
{
    Tree tree;
    Tree* root = tree.CreateRoot(10);
    tree.AddLeaf(root, 5);
    tree.AddLeaf(root, 15);
    tree.AddLeaf(root, 3);
    tree.AddLeaf(root, 7);
    BOOST_CHECK(tree.ifZnachTreeExist(root, 10));
    BOOST_CHECK(tree.ifZnachTreeExist(root, 5));
    BOOST_CHECK(tree.ifZnachTreeExist(root, 15));
    BOOST_CHECK(tree.ifZnachTreeExist(root, 3));
    BOOST_CHECK(tree.ifZnachTreeExist(root, 7));
    BOOST_CHECK(!tree.ifZnachTreeExist(root, 100)); 
    tree.deleteNode(root, 10);
    tree.deleteNode(root, 5);
    tree.deleteNode(root, 15);
    tree.deleteNode(root, 3);
    tree.deleteNode(root, 7);
}
BOOST_AUTO_TEST_CASE(MinMaxTest)//MinMax
{
    Tree tree;
    Tree* root = tree.CreateRoot(10);
    tree.AddLeaf(root, 5);
    tree.AddLeaf(root, 15);
    tree.AddLeaf(root, 1);
    tree.AddLeaf(root, 20);
    Tree* minNode = tree.findMinimum(root);
    Tree* maxNode = tree.findMax(root);
    BOOST_CHECK_EQUAL(minNode->getValue(), 1);
    BOOST_CHECK_EQUAL(maxNode->getValue(), 20);
    tree.deleteNode(root, 10);
    tree.deleteNode(root, 5);
    tree.deleteNode(root, 15);
    tree.deleteNode(root, 1);
    tree.deleteNode(root, 20);
}
BOOST_AUTO_TEST_CASE(DeleteLeafTest)//DeleteLeaf
{
    Tree tree;
    Tree* root = tree.CreateRoot(10);
    tree.AddLeaf(root, 5);
    BOOST_CHECK(tree.ifZnachTreeExist(root, 5));
    bool result = tree.deleteLeaf(root, 5);
    BOOST_CHECK(result);
    BOOST_CHECK(!tree.ifZnachTreeExist(root, 5));
    result = tree.deleteLeaf(root, 100);
    BOOST_CHECK(!result);
    tree.deleteNode(root, 10);
}
BOOST_AUTO_TEST_CASE(UniversalDeleteTest) //Universal
{
    Tree tree;
    Tree* root = tree.CreateRoot(10);
    tree.AddLeaf(root, 5);
    tree.AddLeaf(root, 15);
    tree.AddLeaf(root, 3);
    tree.AddLeaf(root, 7);
    BOOST_CHECK(tree.deleteNode(root, 3));
    BOOST_CHECK(!tree.ifZnachTreeExist(root, 3));
    BOOST_CHECK(tree.deleteNode(root, 5));
    BOOST_CHECK(!tree.ifZnachTreeExist(root, 5));
    BOOST_CHECK(!tree.deleteNode(root, 100));
    tree.deleteNode(root, 10);
    tree.deleteNode(root, 7);
    tree.deleteNode(root, 15);
}
BOOST_AUTO_TEST_CASE(TraversalTest) //Обходы
{
    Tree tree;
    Tree* root = tree.CreateRoot(10);
    tree.AddLeaf(root, 5);
    tree.AddLeaf(root, 15);
    BOOST_TEST_CHECKPOINT("Testing symmetrical traversal");
    tree.symmetrical(root);
    BOOST_TEST_CHECKPOINT("Testing BFS traversal");
    tree.BFS(root);
    BOOST_TEST_CHECKPOINT("Testing top-to-bottom traversal");
    tree.fromTopToBottom(root);
    BOOST_TEST_CHECKPOINT("Testing bottom-to-top traversal");
    tree.fromBottomToTop(root);
    tree.deleteNode(root, 10);
    tree.deleteNode(root, 5);
    tree.deleteNode(root, 15);
}
BOOST_AUTO_TEST_CASE(HeightTest) //Высота
{
    Tree tree;
    Tree* root = tree.CreateRoot(10);
    BOOST_CHECK_EQUAL(tree.getHeight(root), 1);
    tree.AddLeaf(root, 5);
    BOOST_CHECK(tree.getHeight(root) >= 1);
    tree.AddLeaf(root, 15);
    BOOST_CHECK(tree.getHeight(root) >= 1);
    tree.deleteNode(root, 10);
    tree.deleteNode(root, 5);
    tree.deleteNode(root, 15);
}
BOOST_AUTO_TEST_CASE(FixDeleteTest) //FixDelete
{
    Tree tree;
    BOOST_TEST_CHECKPOINT("Testing Case 2 - both sibling children black (left child)");
    {
        Tree* root = tree.CreateRoot(20);
        tree.AddLeaf(root, 10);
        tree.AddLeaf(root, 30);
        tree.AddLeaf(root, 5);
        BOOST_CHECK(root->getValue() == 20);
        BOOST_CHECK(root->getLeft()->getValue() == 10);
        BOOST_CHECK(root->getRight()->getValue() == 30);
        BOOST_CHECK(root->getLeft()->getLeft()->getValue() == 5);
        bool result = tree.deleteNode(root, 5);
        BOOST_CHECK(result);
        BOOST_CHECK(!tree.ifZnachTreeExist(root, 5));
        BOOST_CHECK(tree.ifZnachTreeExist(root, 10));
        BOOST_CHECK(tree.ifZnachTreeExist(root, 20));
        BOOST_CHECK(tree.ifZnachTreeExist(root, 30));
        tree.deleteNode(root, 10);
        tree.deleteNode(root, 20);
        tree.deleteNode(root, 30);
    }
    BOOST_TEST_CHECKPOINT("Testing Case 4 - sibling's right child red (left child)");
    {
        Tree* root = tree.CreateRoot(20);
        tree.AddLeaf(root, 10);
        tree.AddLeaf(root, 30);
        tree.AddLeaf(root, 25);
        tree.AddLeaf(root, 35);
        bool result = tree.deleteNode(root, 10);
        BOOST_CHECK(result);
        BOOST_CHECK(!tree.ifZnachTreeExist(root, 10));
        BOOST_CHECK(tree.ifZnachTreeExist(root, 20));
        BOOST_CHECK(tree.ifZnachTreeExist(root, 30));
        BOOST_CHECK(tree.ifZnachTreeExist(root, 25));
        BOOST_CHECK(tree.ifZnachTreeExist(root, 35));
        tree.deleteNode(root, 20);
        tree.deleteNode(root, 25);
        tree.deleteNode(root, 30);
        tree.deleteNode(root, 35);
    }
    BOOST_TEST_CHECKPOINT("Testing Case 1 - sibling red (left child)");
    {
        Tree* root = tree.CreateRoot(10);
        tree.AddLeaf(root, 5);
        tree.AddLeaf(root, 20);
        tree.AddLeaf(root, 15);
        tree.AddLeaf(root, 30);
        tree.AddLeaf(root, 25);
        tree.AddLeaf(root, 35);
        bool result = tree.deleteNode(root, 5);
        BOOST_CHECK(result);
        BOOST_CHECK(tree.ifZnachTreeExist(root, 10));
        BOOST_CHECK(tree.ifZnachTreeExist(root, 20));
        tree.deleteNode(root, 10);
        tree.deleteNode(root, 15);
        tree.deleteNode(root, 20);
        tree.deleteNode(root, 25);
        tree.deleteNode(root, 30);
        tree.deleteNode(root, 35);
    }
    BOOST_TEST_CHECKPOINT("Testing Case 3 - sibling's right child black (left child)");
    {
        Tree* root = tree.CreateRoot(20);
        tree.AddLeaf(root, 10);
        tree.AddLeaf(root, 30);
        tree.AddLeaf(root, 5);
        tree.AddLeaf(root, 15);
        tree.AddLeaf(root, 25);
        bool result = tree.deleteNode(root, 25);
        BOOST_CHECK(result);
        result = tree.deleteNode(root, 30);
        BOOST_CHECK(result);
        BOOST_CHECK(tree.ifZnachTreeExist(root, 20));
        BOOST_CHECK(tree.ifZnachTreeExist(root, 10));
        BOOST_CHECK(tree.ifZnachTreeExist(root, 5));
        BOOST_CHECK(tree.ifZnachTreeExist(root, 15));
        tree.deleteNode(root, 5);
        tree.deleteNode(root, 10);
        tree.deleteNode(root, 15);
        tree.deleteNode(root, 20);
    }
    BOOST_TEST_CHECKPOINT("Testing right child cases with else block");
    {
        Tree* root = tree.CreateRoot(10);
        tree.AddLeaf(root, 5);
        tree.AddLeaf(root, 20);
        tree.AddLeaf(root, 15);
        tree.AddLeaf(root, 30);
        tree.AddLeaf(root, 25);
        tree.AddLeaf(root, 35);
        bool result = tree.deleteNode(root, 15);
        BOOST_CHECK(result);
        result = tree.deleteNode(root, 25);
        BOOST_CHECK(result);
        BOOST_CHECK(tree.ifZnachTreeExist(root, 10));
        BOOST_CHECK(tree.ifZnachTreeExist(root, 5));
        BOOST_CHECK(tree.ifZnachTreeExist(root, 20));
        BOOST_CHECK(tree.ifZnachTreeExist(root, 30));
        BOOST_CHECK(tree.ifZnachTreeExist(root, 35));
        tree.deleteNode(root, 5);
        tree.deleteNode(root, 10);
        tree.deleteNode(root, 20);
        tree.deleteNode(root, 30);
        tree.deleteNode(root, 35);
    }
    BOOST_TEST_CHECKPOINT("Testing root deletion with balancing and else block");
    {
        Tree* root = tree.CreateRoot(20);
        tree.AddLeaf(root, 10);
        tree.AddLeaf(root, 30);
        tree.AddLeaf(root, 5);
        tree.AddLeaf(root, 15);
        bool result = tree.deleteNode(root, 20); // Удаляем корень
        BOOST_CHECK(result);
        BOOST_CHECK(!tree.ifZnachTreeExist(root, 20));
        BOOST_CHECK(root->getColor() == Tree::BLACK);
        BOOST_CHECK(tree.ifZnachTreeExist(root, 10) || tree.ifZnachTreeExist(root, 30));
        BOOST_CHECK(tree.ifZnachTreeExist(root, 5));
        BOOST_CHECK(tree.ifZnachTreeExist(root, 15));
        if (tree.ifZnachTreeExist(root, 10)) tree.deleteNode(root, 10);
        if (tree.ifZnachTreeExist(root, 30)) tree.deleteNode(root, 30);
        tree.deleteNode(root, 5);
        tree.deleteNode(root, 15);
    }
    BOOST_TEST_CHECKPOINT("Testing black height property after deletions with else block");
    {
        Tree* root = tree.CreateRoot(50);
        for (int i = 0; i < 10; i++) {
            tree.AddLeaf(root, i * 10 + 10);
        }
        tree.deleteNode(root, 20);
        tree.deleteNode(root, 40);
        tree.deleteNode(root, 60);
        tree.deleteNode(root, 80);
        BOOST_CHECK(tree.ifZnachTreeExist(root, 50));
        BOOST_CHECK(tree.ifZnachTreeExist(root, 10));
        BOOST_CHECK(tree.ifZnachTreeExist(root, 30));
        BOOST_CHECK(tree.ifZnachTreeExist(root, 70));
        BOOST_CHECK(tree.ifZnachTreeExist(root, 90));
        int values[] = {10, 30, 50, 70, 90, 100};
        for (int val : values) {
            if (tree.ifZnachTreeExist(root, val)) {
                tree.deleteNode(root, val);
            }
        }
    }
}
BOOST_AUTO_TEST_CASE(FixDeleteComprehensiveTestWithElse)
{
    Tree tree;
    BOOST_TEST_CHECKPOINT("Comprehensive fixDelete test with multiple operations and else block");
    {
        Tree* root = tree.CreateRoot(100);
        int elements[] = {50, 150, 25, 75, 125, 175, 10, 40, 60, 90, 110, 140, 160, 190};
        for (int elem : elements) {
            tree.AddLeaf(root, elem);
        }
        int deletions[] = {10, 40, 110, 140, 25, 75, 125, 175, 50, 150};
        for (int del_val : deletions) {
            BOOST_TEST_CHECKPOINT(std::string("Deleting value: ") + std::to_string(del_val));
            bool result = tree.deleteNode(root, del_val);
            BOOST_CHECK(result);
            BOOST_CHECK(!tree.ifZnachTreeExist(root, del_val));
            for (int elem : elements) {
                if (elem != del_val && std::find(std::begin(deletions), std::end(deletions), elem) == std::end(deletions)) {
                    BOOST_CHECK_MESSAGE(
                        tree.ifZnachTreeExist(root, elem), 
                        "Element " << elem << " should exist after deleting " << del_val
                    );
                }
            }
        }
        int remaining[] = {100, 60, 90, 160, 190};
        for (int val : remaining) {
            if (tree.ifZnachTreeExist(root, val)) {
                tree.deleteNode(root, val);
            }
        }
    }
    BOOST_TEST_CHECKPOINT("Specific test for else block in fixDelete");
    {
        Tree* root = tree.CreateRoot(50);
        tree.AddLeaf(root, 30);
        tree.AddLeaf(root, 70);
        tree.AddLeaf(root, 20);
        tree.AddLeaf(root, 40);
        tree.AddLeaf(root, 60);
        tree.AddLeaf(root, 80);
        tree.AddLeaf(root, 10);
        tree.AddLeaf(root, 25);
        tree.AddLeaf(root, 35);
        tree.AddLeaf(root, 45);
        bool result = tree.deleteNode(root, 10);
        BOOST_CHECK(result);
        result = tree.deleteNode(root, 25);
        BOOST_CHECK(result);
        result = tree.deleteNode(root, 35);
        BOOST_CHECK(result);
        result = tree.deleteNode(root, 45);
        BOOST_CHECK(result);
        BOOST_CHECK(tree.ifZnachTreeExist(root, 50));
        BOOST_CHECK(tree.ifZnachTreeExist(root, 30));
        BOOST_CHECK(tree.ifZnachTreeExist(root, 70));
        BOOST_CHECK(tree.ifZnachTreeExist(root, 20));
        BOOST_CHECK(tree.ifZnachTreeExist(root, 40));
        BOOST_CHECK(tree.ifZnachTreeExist(root, 60));
        BOOST_CHECK(tree.ifZnachTreeExist(root, 80));
        int values[] = {20, 30, 40, 50, 60, 70, 80};
        for (int val : values) {
            if (tree.ifZnachTreeExist(root, val)) {
                tree.deleteNode(root, val);
            }
        }
    }
}
BOOST_AUTO_TEST_SUITE_END()
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////ДВУСВЯЗНЫЙ СПИСОК
BOOST_AUTO_TEST_CASE(test_constructor_and_basic_operations) {
    StudentGroup group("Alice");
    BOOST_CHECK(group.contains("Alice"));
    BOOST_CHECK(!group.isEmpty());
    BOOST_CHECK_EQUAL(group.getFirst(), "Alice");
    BOOST_CHECK_EQUAL(group.getLast(), "Alice");
}
BOOST_AUTO_TEST_CASE(AddTjEndTest) {
    StudentGroup group;
    group.addStudentToStart("Bob");
    group.addStudentToEnd("Charlie");
    BOOST_CHECK_EQUAL(group.getFirst(), "Bob");
    BOOST_CHECK_EQUAL(group.getLast(), "Charlie");
}
BOOST_AUTO_TEST_CASE(AddBeforeANDAfter) {
    StudentGroup group("Bob");
    group.addStudentToEnd("Charlie");
    group.addStudentBefore("Charlie", "Alice");
    group.addStudentAfter("Bob", "David");
    BOOST_CHECK(group.contains("Alice"));
    BOOST_CHECK(group.contains("David"));
}
BOOST_AUTO_TEST_CASE(AllDeleteTesets) {
    StudentGroup group;
    group.addStudentToEnd("Alice");
    group.addStudentToEnd("Bob");
    group.addStudentToEnd("Charlie");
    group.deleteStudentFromStart();
    BOOST_CHECK_EQUAL(group.getFirst(), "Bob");
    group.deleteStudentFromEnd();
    BOOST_CHECK_EQUAL(group.getLast(), "Bob");
    group.addStudentToEnd("Charlie");
    group.addStudentToEnd("David");
    group.deleteStudentAfter("Bob");
    BOOST_CHECK(!group.contains("Charlie"));
    group.addStudentBefore("David", "Charlie");
    group.deleteStudentBefore("David");
    BOOST_CHECK(!group.contains("Charlie"));
}
BOOST_AUTO_TEST_CASE(DeleteByValueTest) {
    StudentGroup group;
    group.addStudentToEnd("Alice");
    group.addStudentToEnd("Bob");
    group.addStudentToEnd("Charlie");
    group.deleteStudentByValue("Bob");
    BOOST_CHECK(!group.contains("Bob"));
    BOOST_CHECK(group.contains("Alice"));
    BOOST_CHECK(group.contains("Charlie"));
}
BOOST_AUTO_TEST_CASE(TestCopirov) {
    StudentGroup group1;
    group1.addStudentToEnd("Alice");
    group1.addStudentToEnd("Bob");
    StudentGroup group2 = group1;
    BOOST_CHECK(group2.contains("Alice"));
    BOOST_CHECK(group2.contains("Bob"));
    group1.deleteStudentByValue("Alice");
    BOOST_CHECK(group2.contains("Alice")); 
}
BOOST_AUTO_TEST_CASE(Operatori) {
    StudentGroup group1;
    group1.addStudentToEnd("Alice");
    group1.addStudentToEnd("Bob");
    StudentGroup group2;
    group2 = group1;
    BOOST_CHECK(group2.contains("Alice"));
    BOOST_CHECK(group2.contains("Bob"));
    group1.deleteStudentByValue("Alice");
    BOOST_CHECK(group2.contains("Alice")); 
}
BOOST_AUTO_TEST_CASE(ClearAndEmptyTests) {
    StudentGroup group;
    group.addStudentToEnd("Alice");
    group.addStudentToEnd("Bob");
    BOOST_CHECK(!group.isEmpty());
    group.clear();
    BOOST_CHECK(group.isEmpty());
}
BOOST_AUTO_TEST_CASE(PrintTest) {
    StudentGroup group;
    group.addStudentToEnd("Alice");
    group.addStudentToEnd("Bob");
    group.addStudentToEnd("Charlie");
    group.printForward();
    group.printReverse();
    BOOST_CHECK(true); 
}




















