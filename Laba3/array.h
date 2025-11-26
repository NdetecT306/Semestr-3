#include <iostream>
#include <string>
using namespace std;

class Array {
private:
    string* num;
    int size;
    int capacity;

public:
    Array(); // Конструктор по умолчанию
    Array(int c);
    void addMasAtEnd(string znach);
    void readMas();
    void addMasAtInd(int ind, string znach);
    void poiskMasPoInd(int ind);
    void deleteMasPoInd(int ind);
    void ZamenaMas(int ind, string znach);
    void RazmerMas();
    int getCapacity() const { return capacity; }
    int getSize() const { return size; }
    string getElementAt(int index) const { 
        if (index >= 0 && index < size) 
            return num[index];
        return "";
    }
    ~Array();
};
