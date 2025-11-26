#include<iostream>
#include<string>
using namespace std;
class OdSpis {
private:
    string name;
    OdSpis* place;
public:
    // Конструктор по умолчанию
    OdSpis(string n = "", OdSpis* p = nullptr) : name(n), place(p) {}
    void AddChelPosle(OdSpis* ptr, string str);
    void AddChelDo(OdSpis*& head, OdSpis* ptr, string str);
    void AddToEnd(OdSpis*& ptr, string n);
    void AddToBegin(OdSpis*& ptr, string n);
    void print(OdSpis* ptr);
    int SizeList(OdSpis* ptr);
    bool poisk(OdSpis* ptr, string str);
    void deletePoZnach(OdSpis*& ptr, string str);
    void deleteToBegin(OdSpis*& ptr);
    void deleteToEnd(OdSpis*& ptr);
    OdSpis* FindElement(OdSpis* ptr, string str);
    void deleteChelDo(OdSpis*& ptr, OdSpis* target);
    void deleteChelPosle(OdSpis* ptr);
    void cleanupList(OdSpis*& list);
    OdSpis* getNext() const { return place; }
    string getName() const { return name; }
    void setNext(OdSpis* next) { place = next; }
    ~OdSpis() {};
};

