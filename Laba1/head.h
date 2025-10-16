#include <iostream>
#include <stack>
#include <queue>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
using namespace std;
//МАССИВ
struct Numbers //Сам массив (его составляющие)
{
    string* num;
    int size;
    int C;
};
void createMas(Numbers& A, int C);
void addMasAtInd(Numbers& A, int ind, string znach);
void addMasAtEnd(Numbers& A, string znach);
void poiskMasPoInd(Numbers& A, int ind);
void deleteMasPoInd(Numbers& A, int ind);
void ZamenaMas(Numbers& A, int ind, string znach);
void RazmerMas(Numbers& A);
void readMas(Numbers& A);

//СТЕК
struct Garden {
    string item;  // еда 
    Garden* point;  // указатель на следующий элемент   
};

void addStack(Garden*& ptr, const string& str);
void deleteStack(Garden*& ptr);
void readStack(Garden* ptr);
string popStack(Garden*& ptr);
int countStack(Garden* ptr);

//ОЧЕРЕДЬ
struct Toys {
    string toy;
    Toys* site;
};
/*
struct ToysPoint {
    Toys* head = nullptr;  // еда 
    Toys* tail = nullptr;  // указатель на следующий элемент   
};
*/
void addQueue(Toys*& head, Toys*& tail, string str);
void deleteQueue(Toys*& ptr);
string QPOP(Toys*& ptr);
void readQueue(Toys* ptr);
void readQueueRecursive(Toys* ptr);
int countQueue(Toys* head);

//ОДНОСВЯЗНЫЙ СПИСОК
struct Spisok //Сам односвязный список (его составляющие)
{
    string name;
    Spisok* place;
};
void CreateList(Spisok*& List, string str);
void AddChelPosle(Spisok* ptr, string str);
void AddChelDo(Spisok*& head, Spisok* ptr, string str);
void AddToEnd(Spisok*& ptr, string n);
void AddToBegin(Spisok*& ptr, string n);
void print(Spisok* ptr);
void otherPrint(Spisok* ptr);
int SizeList(Spisok* ptr);
bool poisk(Spisok* ptr, string str);
void deletePoZnach(Spisok*& ptr, string str);
void deleteToBegin(Spisok*& ptr);
void deleteToEnd(Spisok*& ptr);
Spisok* FindElement(Spisok* ptr, string str);
void deleteChelDo(Spisok*& ptr, Spisok* target);
void deleteChelPosle(Spisok* ptr);

//ДВУСВЯЗНЫЙ

struct Group {
    string student;  // студент 
    Group* next;  // указатель на следующий элемент  
    Group* prev;  // указатель на предыдущий элемент 
};
struct GroupPoint {
    Group* head;  // указатель на первый
    Group* tail;  // указатель на последний
};
void CreateGroup(Group*& List, Group*& tail, string str);
void AddStudPosle(Group* ptr, Group*& tail, string n);
void AddStudDo(Group* ptr, Group*& tail, string n);
void deleteStudPosle(Group* ptr, Group*& tail, string n);
void deleteStudDo(Group* ptr, Group*& tail, string n);
void AddStudToStart(Group*& head, Group*& tail, string n);
void AddStudToEnd(Group*& head, Group*& tail, string n);
void DeleteStudFromStart(Group*& head, Group*& tail);
void DeleteStudFromEnd(Group*& head, Group*& tail);
void printGroup(Group* ptr);
void otherPrintGroup(Group* ptr);
bool poiskGroup(Group* ptr, string str);
void deleteGroupPoZnach(Group*& ptr, Group*& tail, const string& str);

enum Color { RED, BLACK }; // 2 цвета
struct Tree //составляющие дерева
{
    int value;
    Color color;
    Tree* left;
    Tree* right;
    Tree* parent;
};
bool ifZnachTreeExist(struct Tree* root, int k);
Tree* CreateRoot(int k);
Tree* CreateNode(int k);
Tree* rotateLeft(Tree*& root, Tree* x);
Tree* rotateRight(Tree*& root, Tree* y);
void fixInsert(Tree*& root, Tree* ptr);
Tree* AddLeaf(Tree*& root, int k);
Tree* findPlace(Tree* root, int k);
Tree* findMinimum(Tree* node);
void transplant(Tree*& root, Tree* u, Tree* v);
void fixDelete(Tree*& root, Tree* x, Tree* parent);
Tree* findMax(Tree* node);
bool deleteLeaf(Tree*& root, int k);
bool deleteNodeWithOneChild(Tree*& root, int k);
bool deleteNodeWithTwoChildren(Tree*& root, int k);
bool deleteNode(Tree*& root, int k);
void symmetrical(Tree* root);
void fromTopToBottom(Tree* root);
void fromBottomToTop(Tree* root);
void BFSRecursiveHelper(Tree* root, int level);
int getHeight(Tree* root);
void BFS(Tree* root);

