#include <iostream>
#include <string>
using namespace std;
class Stack {
private:
    string item;
    Stack* point;  
public:
    // Статические методы
    static void SPUSH(Stack*& ptr, const string& str);
    static void SDELETE(Stack*& ptr);
    static void readStack(Stack* ptr);
    static string SPOP(Stack*& ptr);
    static int countStack(Stack* ptr);
    static void clearStack(Stack*& ptr);
    // Деструктор
    ~Stack();
};
