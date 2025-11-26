#include "dwospis.h"

StudentGroup::StudentGroup() : head(nullptr), tail(nullptr) {}

StudentGroup::StudentGroup(const string& firstStudent) {
    head = new GroupNode(firstStudent);
    tail = head;
}

StudentGroup::~StudentGroup() {
    clear();
}

StudentGroup::StudentGroup(const StudentGroup& other) : head(nullptr), tail(nullptr) {
    GroupNode* current = other.head;
    while (current != nullptr) {
        addStudentToEnd(current->student);
        current = current->next;
    }
}

StudentGroup& StudentGroup::operator=(const StudentGroup& other) {
    if (this != &other) {
        clear();
        GroupNode* current = other.head;
        while (current != nullptr) {
            addStudentToEnd(current->student);
            current = current->next;
        }
    }
    return *this;
}

StudentGroup::GroupNode* StudentGroup::findNode(const string& student) const {
    GroupNode* current = head;
    while (current != nullptr) {
        if (current->student == student) return current;
        current = current->next;
    }
    return nullptr;
}

void StudentGroup::addStudentAfter(const string& target, const string& newStudent) {
    GroupNode* targetNode = findNode(target);
    if (targetNode == nullptr) return;
    
    GroupNode* newNode = new GroupNode(newStudent);
    newNode->next = targetNode->next;
    newNode->prev = targetNode;
    
    if (targetNode->next != nullptr) {
        targetNode->next->prev = newNode;
    }
    targetNode->next = newNode;
    
    if (targetNode == tail) {
        tail = newNode;
    }
}

void StudentGroup::addStudentBefore(const string& target, const string& newStudent) {
    GroupNode* targetNode = findNode(target);
    if (targetNode == nullptr) return;
    
    GroupNode* newNode = new GroupNode(newStudent);
    newNode->next = targetNode;
    newNode->prev = targetNode->prev;
    
    if (targetNode->prev != nullptr) {
        targetNode->prev->next = newNode;
    } else {
        head = newNode;
    }
    targetNode->prev = newNode;
}

void StudentGroup::deleteStudentAfter(const string& target) {
    GroupNode* targetNode = findNode(target);
    if (targetNode == nullptr || targetNode->next == nullptr) return;
    
    GroupNode* nodeToDelete = targetNode->next;
    targetNode->next = nodeToDelete->next;
    
    if (nodeToDelete->next != nullptr) {
        nodeToDelete->next->prev = targetNode;
    }
    
    if (nodeToDelete == tail) {
        tail = targetNode;
    }
    
    delete nodeToDelete;
}

void StudentGroup::deleteStudentBefore(const string& target) {
    GroupNode* targetNode = findNode(target);
    if (targetNode == nullptr || targetNode->prev == nullptr) return;
    
    GroupNode* nodeToDelete = targetNode->prev;
    
    if (nodeToDelete->prev != nullptr) {
        nodeToDelete->prev->next = targetNode;
    } else {
        head = targetNode;
    }
    targetNode->prev = nodeToDelete->prev;
    
    delete nodeToDelete;
}

void StudentGroup::addStudentToStart(const string& student) {
    GroupNode* newNode = new GroupNode(student);
    newNode->next = head;
    
    if (head != nullptr) {
        head->prev = newNode;
    }
    
    head = newNode;
    
    if (tail == nullptr) {
        tail = newNode;
    }
}

void StudentGroup::addStudentToEnd(const string& student) {
    GroupNode* newNode = new GroupNode(student);
    newNode->next = nullptr;
    
    if (head == nullptr) {
        newNode->prev = nullptr;
        head = newNode;
        tail = newNode;
        return;
    }
    
    newNode->prev = tail;
    tail->next = newNode;
    tail = newNode;
}

void StudentGroup::deleteStudentFromStart() {
    if (head == nullptr) return;
    
    GroupNode* nodeToDelete = head;
    head = head->next;
    
    if (head != nullptr) {
        head->prev = nullptr;
    }
    
    if (nodeToDelete == tail) {
        tail = nullptr;
    }
    
    delete nodeToDelete;
}

void StudentGroup::deleteStudentFromEnd() {
    if (head == nullptr) return;
    
    if (head->next == nullptr) {
        delete head;
        head = nullptr;
        tail = nullptr;
        return;
    }
    
    GroupNode* nodeToDelete = tail;
    tail = tail->prev;
    tail->next = nullptr;
    delete nodeToDelete;
}

void StudentGroup::deleteStudentByValue(const string& student) {
    if (!contains(student)) {
        cout << "Нельзя удалить несуществующий элемент" << endl;
        return;
    }
    
    GroupNode* nodeToDelete = findNode(student);
    if (nodeToDelete == nullptr) {
        cout << "Элемент не найден" << endl;
        return;
    }
    
    if (nodeToDelete == head) {
        head = nodeToDelete->next;
        if (head != nullptr) {
            head->prev = nullptr;
        }
    } else {
        nodeToDelete->prev->next = nodeToDelete->next;
    }
    
    if (nodeToDelete == tail) {
        tail = nodeToDelete->prev;
    } else {
        nodeToDelete->next->prev = nodeToDelete->prev;
    }
    
    delete nodeToDelete;
}

void StudentGroup::printForward() const {
    GroupNode* current = head;
    while (current != nullptr) {
        cout << current->student << " ";
        current = current->next;
    }
    cout << endl;
}

void StudentGroup::printReverse() const {
    GroupNode* current = tail;
    while (current != nullptr) {
        cout << current->student << " ";
        current = current->prev;
    }
    cout << endl;
}

bool StudentGroup::contains(const string& student) const {
    return findNode(student) != nullptr;
}

void StudentGroup::clear() {
    while (head != nullptr) {
        deleteStudentFromStart();
    }
}

bool StudentGroup::isEmpty() const {
    return head == nullptr;
}

string StudentGroup::getFirst() const {
    return head != nullptr ? head->student : "";
}

string StudentGroup::getLast() const {
    return tail != nullptr ? tail->student : "";
}
