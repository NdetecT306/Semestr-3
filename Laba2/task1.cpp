// ЗАДАНИЕ 1
enum DIR { Right, Left };
struct Pair
{
    int* num;
    DIR direction;
};
struct Massiv
{
    Pair* asteroid;
    int size;
    int C;
};
void createMas(Massiv& A, int C) { //Создать массив
    A.asteroid = new Pair[C];
    A.size = 0; // изначально элементов нет
    A.C = C;
}
Pair AddPair(int znach, DIR dir) {
    Pair newPair;
    newPair.num = new int(znach);
    newPair.direction = dir;
    return newPair;
}
void addMasAtEnd(Massiv& A, int znach, DIR dir) { //Добавить в конец
    if (A.size >= A.C) {
        cout << "Массив заполнен." << endl;
        return;
    }
    A.asteroid[A.size] = AddPair(znach, dir);
    A.size++;
}
void poiskMasPoInd(Massiv& A, int ind) { //Получение по индексу
    if (ind < 0 || ind >= A.size) {
        cout << "Выход за границы массива." << endl;
        return;
    }
    else {
        cout << *A.asteroid[ind].num  << A.asteroid[ind].direction << endl;
    }
}
void deleteMasPoInd(Massiv& A, int ind) { //Удалить по индексу
    if (ind < 0 || ind >= A.size) {
        cout << "Элемента за границами массива быть не может." << endl;
        return;
    }
    else {
        delete A.asteroid[ind].num;
        for (int i = ind; i < A.size - 1; i++) {
            A.asteroid[i] = A.asteroid[i + 1];
        }
        A.size--;
    }
}
void ZamenaMas(Massiv& A, int ind, int znach, DIR dir) { 
    if (ind < 0 || ind >= A.size) {
        cout << "Нельзя заменить то что не существует." << endl;
        return;
    }
    else {
        delete A.asteroid[ind].num;
        A.asteroid[ind].num = new int(znach);
        A.asteroid[ind].direction = dir;
    }
}
void readMas(Massiv& A) { //Чтение
    if (A.size == 0) {
        cout << "Массив пустой, Ваша честь.";
        return;
    }
    for (int i = 0; i < A.size; i++){
        cout << *A.asteroid[i].num << " ";
        if (A.asteroid[i].direction == Right) {
            cout << "right";
        }
        else {
            cout << "left";
        }
        cout << endl;
    }
    cout << endl;
}
void Transformation(Massiv& A) {
    bool crash;
    do {
        crash = false;
        int i = 0;
        while (i < A.size - 1) {
            if (A.asteroid[i].direction != A.asteroid[i + 1].direction) {
                crash = true;
                if (*A.asteroid[i].num > *A.asteroid[i + 1].num) {
                    int newSize = *A.asteroid[i].num - *A.asteroid[i + 1].num;
                    ZamenaMas(A, i, newSize, A.asteroid[i].direction);
                    deleteMasPoInd(A, i + 1); 
                }
                else if (*A.asteroid[i].num < *A.asteroid[i + 1].num) {
                    int newSize = *A.asteroid[i + 1].num - *A.asteroid[i].num;
                    ZamenaMas(A, i + 1, newSize, A.asteroid[i+1].direction);
                    deleteMasPoInd(A, i);
                }
                else {
                    deleteMasPoInd(A, i + 1);
                    deleteMasPoInd(A, i);
                }
            }
            else {
                i++; 
            }
        }
    } while (crash && A.size > 1);
}
int main()
{
    setlocale(LC_ALL, "rus");
    Massiv OurMassiv;
    cout << "Введите количество астероидов: ";
    int carasity;
    cin >> carasity;
    if (carasity <= 0) {
        cout << "Невозможное количество астероидов. Прекращаем работу.";
        return 0;
    }
    createMas(OurMassiv, carasity);
    cout << "Введите все астероиды и их направления (right или left)." << endl;
    int size;
    string dir;
    for (int i = 0; i < carasity; i++){
        int num = i + 1;
        cout << "Размер астероида " << num <<": ";
        cin >> size;
        cout << "Направление астероида " << num << ": ";
        cin >> dir;
        DIR direction;
        if (dir == "right") {
            direction = Right;
        }
        else if (dir == "left") {
            direction = Left;
        }
        else {
            cout << "Неверное направление! Прекращаю работу." << endl;
            return 0;
        }
        addMasAtEnd(OurMassiv, size, direction);
    }
    cout << "Ваш введенный массив: " << endl;
    readMas(OurMassiv);
    Transformation(OurMassiv);
    cout << "После изменений: " << endl;
    readMas(OurMassiv);
}
