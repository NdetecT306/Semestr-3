#include <iostream>
#include <string>
using namespace std;
class StudentGroup {
private:
    struct GroupNode {
        string student;
        GroupNode* next;
        GroupNode* prev;
        GroupNode(const string& name) 
            : student(name), next(nullptr), prev(nullptr) {}
    };
    GroupNode* head;
    GroupNode* tail;
    GroupNode* findNode(const string& student) const;
public:
    StudentGroup();
    explicit StudentGroup(const string& firstStudent);
    ~StudentGroup();
    StudentGroup(const StudentGroup& other);
    StudentGroup& operator=(const StudentGroup& other);
    void addStudentAfter(const string& target, const string& newStudent);
    void addStudentBefore(const string& target, const string& newStudent);
    void deleteStudentAfter(const string& target);
    void deleteStudentBefore(const string& target);
    void addStudentToStart(const string& student);
    void addStudentToEnd(const string& student);
    void deleteStudentFromStart();
    void deleteStudentFromEnd();
    void deleteStudentByValue(const string& student);
    void printForward() const;
    void printReverse() const;
    bool contains(const string& student) const;
    void clear();
    bool isEmpty() const;
    string getFirst() const;
    string getLast() const;
};
