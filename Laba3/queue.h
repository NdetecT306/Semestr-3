#include <iostream>
#include<string>
using namespace std;
class Queue {
private:
    string toy;
    Queue* site;   
public:
    Queue() : toy(""), site(nullptr) {}
    Queue(string str) : toy(str), site(nullptr) {}
    static void QPUSH(Queue*& head, Queue*& tail, string str);
    static void QDELETE(Queue*& ptr);
    static string QPOP(Queue*& ptr);
    static void readQueue(Queue* ptr);
    static int countQueue(Queue* ptr);
    ~Queue();
};
