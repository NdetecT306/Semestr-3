#include "stack.h"
void Stack::SPUSH(Stack*& ptr, const string& str) {
    Stack* element = new Stack;
    element->item = str;
    element->point = ptr; 
    ptr = element;        
}
void Stack::SDELETE(Stack*& ptr) {
    if (ptr != nullptr) {
        Stack* element = ptr;
        ptr = ptr->point; 
        delete element;     
    }
    else {
        cout << "Стек пустой. Удалять нечего.\n";
        return;  
    }
}
void Stack::readStack(Stack* ptr) {
    if (ptr==nullptr){ 
        cout << "Стек пустой. Читать нечего" << endl;
        return;
    } 
    Stack* element = ptr;
    while (element != nullptr) {
        cout << element->item << " ";
        element = element->point;
    }
}
string Stack::SPOP(Stack*& ptr) {
    if (ptr == nullptr) { 
        cout << "Стек пуст. Удалять нечего." << endl;
        return "";
    }
    string item = ptr->item;
    Stack* element = ptr;
    ptr = ptr->point;
    delete element;
    return item;
}
int Stack::countStack(Stack* ptr) {
    if(ptr == nullptr)
    {
        cout << "Стек пустой, мы не считаем пустоту." << endl;
        return 0;
    }
    int count = 0;
    Stack* element = ptr;
    while (element != nullptr) {
        count++;
        element = element->point;
    }
    return count;
}
void Stack::clearStack(Stack*& ptr) {
    while (ptr != nullptr) {
        SDELETE(ptr);
    }
}
Stack::~Stack() {
}
