// ЗАДАНИЕ 1
#include <iostream>
#include <string>
using namespace std;
enum DIR { Right, Left };
struct Pair
{
    int* num;
    DIR direction;
};
struct STACK {
    Pair asteroid;  
    STACK* point;   
};
Pair AddPair(int znach, DIR dir) {
    Pair newPair;
    newPair.num = new int(znach);
    newPair.direction = dir;
    return newPair;
}
void addStack(STACK*& ptr, int size, DIR direction) { 
    STACK* element = new STACK;
    element->asteroid = AddPair(size, direction);  
    element->point = ptr; 
    ptr = element;        
}
void deleteStack(STACK*& ptr) { 
    if (ptr != nullptr) {
        STACK* element = ptr;
        ptr = ptr->point; 
        delete element->asteroid.num;  
        delete element;     
    }
}
void readStack(STACK* ptr) {
    if (ptr == nullptr) {
        return;
    }
    readStack(ptr->point);
    cout << *ptr->asteroid.num << " ";
    if (ptr->asteroid.direction == Right) {
        cout << "right" << endl;
    } else {
        cout << "left" << endl;
    }
}
void Transformation(STACK*& A) {
    if (A == nullptr || A->point == nullptr) return;
    STACK* element = nullptr;
    STACK* current = A;
    while (current != nullptr) {
        addStack(element, *current->asteroid.num, current->asteroid.direction);
        current = current->point;
    }
    bool move;
    do {
        move = false;
        STACK* now = element;
        STACK* prev = nullptr;
        while (now != nullptr && now->point != nullptr) {
            STACK* next = now->point;
            if (now->asteroid.direction == Right && next->asteroid.direction == Left) {
                move = true;
                if (*now->asteroid.num > *next->asteroid.num) {
                    *now->asteroid.num = *now->asteroid.num - *next->asteroid.num;
                    now->point = next->point;
                    delete next->asteroid.num;
                    delete next;
                }
                else if (*now->asteroid.num < *next->asteroid.num) {
                    *next->asteroid.num = *next->asteroid.num - *now->asteroid.num;
                    if (prev == nullptr) {
                        element = next;
                        delete now->asteroid.num;
                        delete now;
                        now = element;
                    } else {
                        prev->point = next;
                        delete now->asteroid.num;
                        delete now;
                        now = next;
                    }
                }
                else {
                    if (prev == nullptr) {
                        element = next->point;
                        delete now->asteroid.num;
                        delete now;
                        delete next->asteroid.num;
                        delete next;
                        now = element;
                    } else {
                        prev->point = next->point;
                        delete now->asteroid.num;
                        delete now;
                        delete next->asteroid.num;
                        delete next;
                        now = prev->point;
                    }
                }
                break; 
            }
            else {
                prev = now;
                now = now->point;
            }
        }
    } while (move && element != nullptr && element->point != nullptr);
    while (A != nullptr) {
        deleteStack(A);
    }
    current = element;
    while (current != nullptr) {
        addStack(A, *current->asteroid.num, current->asteroid.direction);
        current = current->point;
    }
}
int main()
{
    setlocale(LC_ALL, "rus");
    STACK* OurStack = nullptr;  
    cout << "Введите количество астероидов: ";
    int capacity;
    cin >> capacity;
    if (capacity <= 0) {
        cout << "Невозможное количество астероидов. Прекращаем работу.";
        return 0;
    }
    cout << "Введите все астероиды и их направления (right или left)." << endl;
    for (int i = 0; i < capacity; i++){
        int size;
        string dir;
        int num = i + 1;
        cout << "Размер астероида " << num << ": ";
        cin >> size;
        cout << "Направление астероида " << num << ": ";
        cin >> dir;
        DIR direction;
        if (dir == "right" || dir == "Right" || dir == "r") {
            direction = Right;
        }
        else if (dir == "left" || dir == "Left" || dir == "l") {
            direction = Left;
        }
        else {
            cout << "Неверное направление! Прекращаю работу." << endl;
            return 0;
        }
        addStack(OurStack, size, direction);
    }
    cout << "Ваш введенный массив: " << endl;
    readStack(OurStack);
    cout << endl;
    Transformation(OurStack);
    cout << "После изменений: " << endl;
    readStack(OurStack);
    cout << endl;
    return 0;
}
