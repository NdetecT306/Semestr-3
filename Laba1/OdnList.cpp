#include "head.h"
using namespace std;
void CreateList(Spisok*& List, string str) {//Создание
    List = new Spisok{str, nullptr };
}
void AddChelPosle(Spisok* ptr, string str) {//Добавить человека в односвязный список после ptr
    Spisok* element = new Spisok;
    element->name = str;
    element->place =ptr->place;
    ptr->place = element;
}
void AddChelDo(Spisok*& head, Spisok* ptr, string str) {//Добавить человека в односвязный список до ptr
    Spisok* element = new Spisok;
    element->name = str;
    if (head == ptr){
        element->place = head;
        head = element;
    }
    else {
        Spisok* per = head;
        while (per && per->place != ptr) {
            per = per->place;
        }
        if (per) {
            element->place = ptr;
            per->place = element;
        }
        else {
            delete element;
        }
    }
}
void AddToEnd(Spisok*& ptr, string n) {//Добавить в конец односвязного списка
    Spisok* element = new Spisok;
    element->name = n;
    element->place = nullptr;
    Spisok* temp = ptr;
    while (temp->place != nullptr){
        temp = temp->place;
    }
    temp->place = element;
}
void AddToBegin(Spisok*& ptr, string n) {//Добавить в начало односвязного списка
    Spisok* element = new Spisok;
    element->name = n;
    element->place = ptr;
    ptr = element;
}
void print(Spisok* ptr) {//Печать односвязного списка - печатаем элемент и сдвигаемся на следующий указатель, пока не наткнемся на nullptr
    while (ptr != nullptr){
        cout << ptr->name << " ";
        ptr = ptr->place;
    }
    cout << endl;
}
int SizeList(Spisok* ptr) {//Размер односвязного списка - сменяем указатели, пока не наткнемся на nullptr
    int count = 0;
    while (ptr != nullptr){
        count++;
        ptr = ptr->place;
    }
    return count;
}
bool poisk(Spisok* ptr, string str){ //Поиск элемента по значению (есть или нет)
    Spisok* element = ptr;
    while (element != nullptr) {
        if (element->name == str) return true;
        element = element->place;
    }
    return false;
}
void deletePoZnach(Spisok*& ptr, string str){ //Удаление по значению
    if (ptr == nullptr) return;
    if(!poisk(ptr, str)) cout << "Нельзя удалить несуществующий элемент" << endl;
    else {
        Spisok* element = ptr;
        Spisok* prev = nullptr;
        while (element != nullptr && element->name != str) {
            prev = element;
            element = element->place;
        }
        if (prev == nullptr) {
            ptr = element->place;
        }
        else {
            prev->place = element->place;
        }
        delete element;
    }
}
void deleteToBegin(Spisok*& ptr) {//Удалить начало односвязного списка
    if (ptr == nullptr) return;
    if (ptr->place == nullptr) {
        delete ptr;
        ptr = nullptr;
        return;
    }
    Spisok* element = ptr;
    ptr = ptr->place;
    delete element;
}
void deleteToEnd(Spisok*& ptr) {//Удалить конец односвязного списка
    if (ptr == nullptr) return;
    if (ptr->place == nullptr) {
        delete ptr;
        ptr = nullptr;
        return;
    }
    Spisok* temp = ptr;
    while (temp->place->place != nullptr) {
        temp = temp->place;
    }
    delete temp->place;
    temp->place = nullptr;
}
Spisok* FindElement(Spisok* ptr, string str) { //Поиск элемента (вернуть указатель)
    Spisok* element = ptr;
    while (element != nullptr) {
        if (element->name == str) return element;
        element = element->place;
    }
    return nullptr;
}
void deleteChelDo(Spisok*& ptr, Spisok* target) { //Удалить до заданного значения
    if (ptr == nullptr || target == nullptr) return;
    if (ptr == target) return;
    Spisok* prevPrev = nullptr;
    Spisok* prev = ptr;
    Spisok* element = ptr->place;
    while (element != nullptr && element != target) {
        prevPrev = prev;
        prev = element;
        element = element->place;
    }
    if (element != target) {
        return;
    }
    if (prevPrev == nullptr) {
        deleteToBegin(ptr);
        ptr = element;
    }
    else {
        prevPrev->place = prev->place;
        delete prev;
    }
}
void deleteChelPosle(Spisok* ptr) {// Удаляет после заданного значения
    if (ptr == nullptr || ptr->place == nullptr) return;
    Spisok* element = ptr->place;
    ptr->place = element->place;
    delete element;
}
