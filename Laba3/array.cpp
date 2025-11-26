#include "array.h"
Array::Array() {
    capacity = 0;
    size = 0;
    num = nullptr;
}
Array::Array(int c) {
    capacity = c;
    size = 0;
    num = new string[capacity];
}
void Array::addMasAtEnd(string znach) {
    if (size >= capacity) {
        cout << "Массив заполнен." << endl;
        return;
    }
    num[size] = znach;
    size++;
}
void Array::readMas() {
    if (size == 0) {
        cout << "Массив пустой, Ваша честь." << endl;
        return;
    }
    for (int i = 0; i < size; i++) {
        cout << num[i] << " ";
    }
    cout << endl;
}
void Array::addMasAtInd(int ind, string znach) {
    if (ind < 0 || ind >= capacity) {
        cout << "Индекс вне диапазона емкости массива." << endl;
        return;
    }
    if (ind > size) {
        cout << "Индекс превышает текущий размер массива." << endl;
        return;
    }
    for (int i = size; i > ind; i--) {
        num[i] = num[i - 1];
    }
    num[ind] = znach;
    size++;
}
void Array::poiskMasPoInd(int ind) {
    if (ind < 0 || ind >= size) {
        cout << "Выход за границы массива." << endl;
        return;
    }
    cout << num[ind] << endl;
}
void Array::deleteMasPoInd(int ind) {
    if (ind < 0 || ind >= size) {
        cout << "Индекс вне границ массива." << endl;
        return;
    }
    for (int i = ind; i < size - 1; i++) {
        num[i] = num[i + 1];
    }
    size--;
}
void Array::ZamenaMas(int ind, string znach) {
    if (ind < 0 || ind >= size) {
        cout << "Нельзя заменить то что не существует." << endl;
        return;
    }
    num[ind] = znach;
}
void Array::RazmerMas() {
    cout << size << endl;
}
Array::~Array() {
    delete[] num;
}
