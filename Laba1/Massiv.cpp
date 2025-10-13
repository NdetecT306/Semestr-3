#include "head.h"
//МАССИВ
void createMas(Numbers& A, int C) {
    A.num = new string[C];
    A.size = 0; // изначально элементов нет
    A.C = C;
}
// Добавить элемент по индексу
void addMasAtInd(Numbers& A, int ind, string znach) {
    if (ind < 0 || ind >= A.C) {
        cout << "Индекс вне диапазона емкости массива." << endl;
        return;
    }
    if (ind >= A.size) {
        // Заполняем промежуточные элементы пустыми строками, если есть пропуск
        for (int i = A.size; i < ind; i++) {
            A.num[i] = "";
        }
        A.size = ind + 1;
    }
    A.num[ind] = znach;
}
// Добавить элемент в конец
void addMasAtEnd(Numbers& A, string znach) {
    if (A.size >= A.C) {
        cout << "Массив заполнен." << endl;
        return;
    }
    A.num[A.size] = znach;
    A.size++;
}
// Получить элемент по индексу (вывод)
void poiskMasPoInd(Numbers& A, int ind) {
    if (ind < 0 || ind >= A.size) {
        cout << "Выход за границы массива." << endl;
        return;
    }
    cout << A.num[ind] << endl;
}
// Удалить элемент по индексу
void deleteMasPoInd(Numbers& A, int ind) {
    if (ind < 0 || ind >= A.size) {
        return; // Некорректный индекс
    }
    for (int i = ind; i < A.size - 1; i++) {
        A.num[i] = A.num[i + 1];
    }
    A.size--; // Уменьшаем размер
}
// Заменить элемент по индексу
void ZamenaMas(Numbers& A, int ind, string znach) {
    if (ind < 0 || ind >= A.size) {
        cout << "Нельзя заменить то что не существует." << endl;
        return;
    }
    A.num[ind] = znach;
}
// Получить размер массива
void RazmerMas(Numbers& A) {
    cout << A.size << endl;
}
// Вывести все элементы массива
void readMas(Numbers& A) {
    if (A.size == 0) {
        cout << "Массив пустой, Ваша честь." << endl;
        return;
    }
    for (int i = 0; i < A.size; i++) {
        cout << A.num[i] << " ";
    }
    cout << endl;
}
