//ЗАДАНИЕ 4
#include<iostream>
using namespace std;
struct Massiv {
    int* num;
    int size;
    int C;
};
void createMas(Massiv& A, int C) { // Создать массив
    A.num = new int[C];
    A.size = 0; // изначально элементов нет
    A.C = C;
}
void addMasAtEnd(Massiv& A, int znach) { // Добавить в конец
    if (A.size >= A.C) {
        cout << "Массив заполнен." << endl;
        return;
    }
    A.num[A.size] = znach;
    A.size++;
}
void readMas(const Massiv& A) { // Чтение
    if (A.size == 0) {
        cout << "Массив пустой, Ваша честь." << endl;
        return;
    }
    for (int i = 0; i < A.size; i++) {
        cout << A.num[i] << " ";
    }
    cout << endl;
}
void poiskPodMas(Massiv& A, int size, int target) {
    for (int start = 0; start < size; start++) {
        int sum = 0;
        for (int end = start; end < size; end++) {
            sum += A.num[end];
            if (sum == target) {
                cout << "[";
                for (int i = start; i <= end; i++) {
                    cout << A.num[i];
                    if (i < end) cout << ", ";
                }
                cout << "]" << endl;
            }
        }
    }
}
int main() {
    setlocale(LC_ALL, "rus");
    Massiv OurMassiv;
    cout << "Введите количество элементов: ";
    int carasity;
    cin >> carasity;
    if (carasity <= 0) {
        cout << "Невозможное количество элементов. Прекращаю работу.";
        return 0;
    }
    createMas(OurMassiv, carasity);
    cout << "Введите " << carasity << " элементов массива:" << endl;
    for (int i = 0; i < carasity; i++) {
        int element;
        cin >> element;
        addMasAtEnd(OurMassiv, element);
    }
    cout << "Цель: ";
    int point;
    cin >> point;
    poiskPodMas(OurMassiv, carasity, point);
    return 0;
}
