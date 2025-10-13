#include "head.h"
//ОЧЕРЕДЬ
void addQueue(Toys*& head, Toys*& tail, string str) { //Добавление в очередь
    Toys* element = new Toys;
    element->toy = str;
    element->site = nullptr;
    if (head == nullptr) {
        head = element;
        tail = element;
    }
    else {
        tail->site = element;
        tail = element;
    }
}
void deleteQueue(Toys*& ptr) { //Удаление из очереди
    if (ptr == nullptr) {
        cout << "Очередь пуста" << endl;
        return;
    }
    Toys* element = ptr;
    ptr = ptr->site;
    delete element;
}
string QPOP(Toys*& ptr) {// Извлечение элемента из очереди с возвратом значения
    if (ptr == nullptr) {
        return "Очередь пуста";
    }
    Toys* element = ptr;
    string item = element->toy;
    ptr = ptr->site;
    delete element;
    return item;
}
void readQueue(Toys* ptr) { //Чтение очереди
    if (ptr == nullptr) {
        cout << "Очередь пуста" << endl;
        return;
    }
    Toys* element = ptr;
    while (element != nullptr) {
        cout << element->toy << " ";
        element = element->site;
    }
    cout << endl;
}
void readQueueRecursive(Toys* ptr) {
    if (ptr == nullptr) {
        return;
    }
    cout << ptr->toy << " ";
    readQueueRecursive(ptr->site);
}
int countQueue(Toys* ptr) { //Подсчет элементов
    int count = 0;
    Toys* element = ptr;
    while (element != nullptr) {
        count++;
        element = element->site;
    }
    return count;
}
