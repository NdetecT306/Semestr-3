#include"odspis.h"
    void OdSpis::AddChelPosle(OdSpis* ptr, string str) {//Добавить человека в односвязный список после ptr
        OdSpis* element = new OdSpis;
        element->name = str;
        element->place =ptr->place;
        ptr->place = element;
    }
    void OdSpis::AddChelDo(OdSpis*& head, OdSpis* ptr, string str) {//Добавить человека в односвязный список до ptr
        OdSpis* element = new OdSpis;
        element->name = str;
        if (head == ptr){
            element->place = head;
            head = element;
        }
        else {
            OdSpis* per = head;
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
    void OdSpis::AddToEnd(OdSpis*& ptr, string n) {//Добавить в конец односвязного списка
        OdSpis* element = new OdSpis;
        element->name = n;
        element->place = nullptr;
        OdSpis* temp = ptr;
        while (temp->place != nullptr){
            temp = temp->place;
        }
        temp->place = element;
    }
    void OdSpis::AddToBegin(OdSpis*& ptr, string n) {//Добавить в начало односвязного списка
        OdSpis* element = new OdSpis;
        element->name = n;
        element->place = ptr;
        ptr = element;
    }
    void OdSpis::print(OdSpis* ptr) {//Печать односвязного списка - печатаем элемент и сдвигаемся на следующий указатель, пока не наткнемся на nullptr
        while (ptr != nullptr){
            cout << ptr->name << " ";
            ptr = ptr->place;
        }
        cout << endl;
    }
    int OdSpis::SizeList(OdSpis* ptr) {//Размер односвязного списка - сменяем указатели, пока не наткнемся на nullptr
        int count = 0;
        while (ptr != nullptr){
            count++;
            ptr = ptr->place;
        }
        return count;
    }
    bool OdSpis::poisk(OdSpis* ptr, string str){ //Поиск элемента по значению (есть или нет)
        OdSpis* element = ptr;
        while (element != nullptr) {
            if (element->name == str) return true;
            element = element->place;
        }
        return false;
    }
    void OdSpis::deletePoZnach(OdSpis*& ptr, string str){ //Удаление по значению
        if (ptr == nullptr) return;
        if(!poisk(ptr, str)) cout << "Нельзя удалить несуществующий элемент" << endl;
        else {
            OdSpis* element = ptr;
            OdSpis* prev = nullptr;
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
    void OdSpis::deleteToBegin(OdSpis*& ptr) {//Удалить начало односвязного списка
        if (ptr == nullptr) return;
        if (ptr->place == nullptr) {
            delete ptr;
            ptr = nullptr;
            return;
        }
        OdSpis* element = ptr;
        ptr = ptr->place;
        delete element;
    }
    void OdSpis::deleteToEnd(OdSpis*& ptr) {//Удалить конец односвязного списка
        if (ptr == nullptr) return;
        if (ptr->place == nullptr) {
            delete ptr;
            ptr = nullptr;
            return;
        }
        OdSpis* temp = ptr;
        while (temp->place->place != nullptr) {
            temp = temp->place;
        }
        delete temp->place;
        temp->place = nullptr;
    }
    OdSpis* OdSpis::FindElement(OdSpis* ptr, string str) { //Поиск элемента (вернуть указатель)
        OdSpis* element = ptr;
        while (element != nullptr) {
            if (element->name == str) return element;
            element = element->place;
        }
        return nullptr;
    }
    void OdSpis::deleteChelDo(OdSpis*& ptr, OdSpis* target) { //Удалить до заданного значения
        if (ptr == nullptr || target == nullptr) return;
        if (ptr == target) return;
        OdSpis* prevPrev = nullptr;
        OdSpis* prev = ptr;
        OdSpis* element = ptr->place;
        while (element != nullptr && element != target) {
            prevPrev = prev;
            prev = element;
            element = element->place;
        }
        if (element != target) {
            return;
        }
        if (prevPrev == nullptr) {
            prev->place = nullptr;
            delete prev;
            ptr = element;
        }
        else {
            prevPrev->place = prev->place;
            prev->place = nullptr;
            delete prev;
        }
    }
    void OdSpis::deleteChelPosle(OdSpis* ptr) {// Удаляет после заданного значения
        if (ptr == nullptr || ptr->place == nullptr) return;
        OdSpis* element = ptr->place;
        ptr->place = element->place;
        element->place = nullptr;
        delete element;
    }
    void OdSpis::cleanupList(OdSpis*& list) {
    while(list != nullptr) {
        OdSpis* temp = list;
        list = list->place;
        delete temp;
    }
}
