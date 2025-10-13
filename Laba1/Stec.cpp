#include "head.h"
//СТЕК
void addStack(Garden*& ptr, const string& str) { //Добавление элемента
    Garden* element = new Garden;
    element->item = str;
    element->point = ptr; 
    ptr = element;        
}
void deleteStack(Garden*& ptr) { //Удаление элемента
    if (ptr != nullptr) {
        Garden* element = ptr;
        ptr = ptr->point; 
        delete element;     
    }
}
void readStack(Garden* ptr) { //Чтение стека
    if (ptr == nullptr) {
        return;
    }
    cout << ptr->item << " ";
    readStack(ptr->point);
}
string popStack(Garden*& ptr) { //Команда по чтению и удаления из таблицы
    if (ptr == nullptr) {
        return "STACK_EMPTY";
    }
    string item = ptr->item;
    Garden* element = ptr;
    ptr = ptr->point;
    delete element;
    return item;
}
int countStack(Garden* ptr) { //Подсчет элементов в стеке
    int count = 0;
    Garden* element = ptr;
    while (element != nullptr) {
        count++;
        element = element->point;
    }
    return count;
}

