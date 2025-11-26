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

void Queue::QDELETE(Queue*& head, Queue*& tail) {  // Добавляем tail в параметры
    if (head == nullptr) {
        cout << "Очередь пуста" << endl;
        return;
    }
    Queue* element = head;
    head = head->site;
    if (head == nullptr) {  // Если очередь стала пустой, обнуляем tail
        tail = nullptr;
    }
    delete element;
}

string Queue::QPOP(Queue*& head, Queue*& tail) {  // Добавляем tail в параметры
    if (head == nullptr) {
        return "Очередь пуста";
    }
    Queue* element = head;
    string item = element->toy;
    head = head->site;
    if (head == nullptr) {  // Если очередь стала пустой, обнуляем tail
        tail = nullptr;
    }
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
void Queue::clearQueue(Queue*& head, Queue*& tail) {
    while (head != nullptr) {
        Queue::QDELETE(head, tail);
    }
}
Queue::~Queue() {
}
