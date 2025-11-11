#include "queue.h" 
void Queue::QPUSH(Queue*& head, Queue*& tail, string str) { 
    Queue* element = new Queue;
    element->toy = str;
    element->site = nullptr;
    if (head == nullptr) {
        head = element;
        tail = element;
    }
    else {
        tail->site = element;
        tail = element;
    }
}
void Queue::QDELETE(Queue*& ptr) { 
    if (ptr == nullptr) {
        cout << "Очередь пуста" << endl;
        return;
    }
    Queue* element = ptr;
    ptr = ptr->site;
    delete element;
}
string Queue::QPOP(Queue*& ptr) {
    if (ptr == nullptr) {
        return "Очередь пуста";
    }
    Queue* element = ptr;
    string item = element->toy;
    ptr = ptr->site;
    delete element;
    return item;
}
void Queue::readQueue(Queue* ptr) { 
    if (ptr == nullptr) {
        cout << "Очередь пуста" << endl;
        return;
    }
    Queue* element = ptr;
    while (element != nullptr) {
        cout << element->toy << " ";
        element = element->site;
    }
    cout << endl;
}
int Queue::countQueue(Queue* ptr) { 
    int count = 0;
    Queue* element = ptr;
    while (element != nullptr) {
        count++;
        element = element->site;
    }
    return count;
}
Queue::~Queue() {
    while (site != nullptr) {
        Queue* temp = site;
        site = site->site;
        delete temp;
    }
}
