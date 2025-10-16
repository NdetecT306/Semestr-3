#include "head.h"
//МАССИВ
void createMas(Numbers& A, int C) {
    A.num = new string[C];
    A.size = 0; // изначально элементов нет
    A.C = C;
}
void addMasAtInd(Numbers& A, int ind, string znach) {// Добавить элемент по индексу
    if (ind < 0 || ind >= A.C) {
        cout << "Индекс вне диапазона емкости массива." << endl;
        return;
    }
    if (ind >= A.size) {
        for (int i = A.size; i < ind; i++) {
            A.num[i] = "";
        }
        A.size = ind + 1;
    }
    A.num[ind] = znach;
}
void addMasAtEnd(Numbers& A, string znach) {// Добавить элемент в конец
    if (A.size >= A.C) {
        cout << "Массив заполнен." << endl;
        return;
    }
    A.num[A.size] = znach;
    A.size++;
}
void poiskMasPoInd(Numbers& A, int ind) {// Получить элемент по индексу (вывод)
    if (ind < 0 || ind >= A.size) {
        cout << "Выход за границы массива." << endl;
        return;
    }
    cout << A.num[ind] << endl;
}
void deleteMasPoInd(Numbers& A, int ind) {// Удалить элемент по индексу
    if (ind < 0 || ind >= A.size) {
        return; 
    }
    for (int i = ind; i < A.size - 1; i++) {
        A.num[i] = A.num[i + 1];
    }
    A.size--; // Уменьшаем размер
}
void ZamenaMas(Numbers& A, int ind, string znach) {// Заменить элемент по индексу
    if (ind < 0 || ind >= A.size) {
        cout << "Нельзя заменить то что не существует." << endl;
        return;
    }
    A.num[ind] = znach;
}
void RazmerMas(Numbers& A) {// Получить размер массива
    cout << A.size << endl;
}
void readMas(Numbers& A) {// Вывести все элементы массива
    if (A.size == 0) {
        cout << "Массив пустой, Ваша честь." << endl;
        return;
    }
    for (int i = 0; i < A.size; i++) {
        cout << A.num[i] << " ";
    }
    cout << endl;
}
