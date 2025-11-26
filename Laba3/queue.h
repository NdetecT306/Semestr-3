#include <iostream>
#include <string>
using namespace std;

class Queue {
private:
    string toy;
    Queue* site;   
public:
    Queue() : toy(""), site(nullptr) {}
    Queue(string str) : toy(str), site(nullptr) {}
    
    static void QPUSH(Queue*& head, Queue*& tail, string str);
    static void QDELETE(Queue*& head, Queue*& tail);
    static string QPOP(Queue*& head, Queue*& tail);
    static void readQueue(Queue* ptr);
    static int countQueue(Queue* ptr);
    static void clearQueue(Queue*& head, Queue*& tail);
    
    // Геттеры для доступа к приватным полям
    string getToy() const { return toy; }
    Queue* getSite() const { return site; }
    
    ~Queue();
};
