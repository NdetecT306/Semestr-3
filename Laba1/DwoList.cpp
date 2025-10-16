#include "head.h"
//ДВУСВЯЗНЫЙ
void CreateGroup(Group*& List, Group*& tail, string str) {//Создание
    List = new Group{str, nullptr, nullptr};
    tail = List;
}
void AddStudPosle(Group* ptr, Group*& tail, string n) { //Добавить студента после
    Group* element = new Group;
    element->student = n;
    element->next = ptr->next;
    element->prev = ptr;
    if (ptr->next != nullptr) {
        ptr->next->prev = element;
    }
    ptr->next = element;
    if (ptr == tail) {
        tail = element;
    }
}
void AddStudDo(Group* ptr, Group*& tail, string n) { //Добавить студента до
    if (ptr == nullptr) return; 
    Group* element = new Group;
    element->student = n;
    element->next = ptr;
    element->prev = ptr->prev;
    if (ptr->prev != nullptr) {
        ptr->prev->next = element;
    }
    ptr->prev = element;
}
void deleteStudPosle(Group* ptr, Group*& tail, string n) { //Удалить студента после
    if (ptr == nullptr || ptr->next == nullptr) return; 
    Group* element = ptr->next;
    if (element->next != nullptr) {
        element->next->prev = ptr;
    }
    ptr->next = element->next;
    if (element == tail) {
        tail = ptr;
    }
    delete element;
}
void deleteStudDo(Group* ptr, Group*& tail, string n) { //Удалить студента до
    if (ptr == nullptr || ptr->prev == nullptr) return; 
    Group* element = ptr->prev;
    if (element->prev == nullptr) {
        ptr->prev = nullptr;
    } 
    else {
        element->prev->next = ptr;
        ptr->prev = element->prev;
    }
    if (element == tail) {
        tail = ptr->prev;
    }
    delete element;
}
void AddStudToStart(Group*& head, Group*& tail, string n) { //Добавить в голову
    Group* element = new Group;
    element->student = n;
    element->next = head;
    element->prev = nullptr;
    if (head != nullptr) {
        head->prev = element;
    }
    head = element;
    if (tail == nullptr) {
        tail = element;
    }
}
void AddStudToEnd(Group*& head, Group*& tail, string n) { //Добавить в конец
    Group* element = new Group;
    element->student = n;
    element->next = nullptr;
    if (head == nullptr) {
        element->prev = nullptr;
        head = element;
        tail = element;
        return;
    }
    element->prev = tail;
    tail->next = element;
    tail = element; 
}
void DeleteStudFromStart(Group*& head, Group*& tail) { //Удалить голову
    if (head == nullptr) return;
    Group* element = head;
    head = head->next;
    if (head != nullptr) {
        head->prev = nullptr;
    }
    if (element == tail) {
        tail = nullptr;
    }
    delete element;
}
void DeleteStudFromEnd(Group*& head, Group*& tail) { //Удалить конец
    if (head == nullptr) return;
    if (head->next == nullptr) {
        delete head;
        head = nullptr;
        tail = nullptr;
        return;
    }
    Group* element = tail;
    tail = tail->prev;
    tail->next = nullptr;
    delete element;
}
void printGroup(Group* ptr) {
    while (ptr != nullptr) {
        cout << ptr->student << " ";
        ptr = ptr->next;
    }
    cout << endl;
}
void otherPrintGroup(Group* ptr) {
    if (ptr == nullptr) {
        return;
    }
    otherPrintGroup(ptr->next); 
    cout << ptr->student << " "; 
}
bool poiskGroup(Group* ptr, string str) { //Поиск элемента по значению (есть или нет)
    Group* element = ptr;
    while (element != nullptr) {
        if (element->student == str) return true;
        element = element->next;
    }
    return false;
}
void deleteGroupPoZnach(Group*& ptr, Group*& tail, const string& str) { //Удаление по значению
    if (ptr == nullptr) return; 
    if (!poiskGroup(ptr, str)) {
        cout << "Нельзя удалить несуществующий элемент" << endl;
        return;
    }
    Group* element = ptr;
    while (element != nullptr && element->student != str) {
        element = element->next;
    }
    if (element == nullptr) {
        cout << "Элемент не найден" << endl;
        return;
    }
    if (element == ptr) {
        ptr = element->next;
        if (ptr != nullptr) {
            ptr->prev = nullptr;
        }
    } 
    else {
        if (element->prev != nullptr)
            element->prev->next = element->next;
        if (element->next != nullptr)
            element->next->prev = element->prev;
    }
    if (element == tail) {
        tail = element->prev;
    }
    delete element; 
}

