#ifndef TREE_H
#define TREE_H

#include <iostream>
using namespace std;

class Tree {
public:
    enum Color { RED, BLACK };

private:
    int value;
    Color color;
    Tree* left;
    Tree* right;
    Tree* parent;

    // Приватные методы
    Tree* rotateLeft(Tree*& root, Tree* x);
    Tree* rotateRight(Tree*& root, Tree* y);
    void transplant(Tree*& root, Tree* u, Tree* v);
    void fixDelete(Tree*& root, Tree* x, Tree* parent);
    void fixInsert(Tree*& root, Tree* ptr);
    

public:
    // Конструкторы
    Tree();
    Tree(int val);

    // Геттеры
    int getValue() const;
    Color getColor() const;
    Tree* getLeft() const;
    Tree* getRight() const;
    Tree* getParent() const;

    // Основные публичные методы
    Tree* findPlace(Tree* root, int k);
    bool ifZnachTreeExist(Tree* root, int k);
    static Tree* CreateRoot(int k);  // Добавлено static
    static Tree* CreateNode(int k);  // Добавлено static
    Tree* AddLeaf(Tree*& root, int k);
    Tree* findMinimum(Tree* node);
    Tree* findMax(Tree* node);
    bool deleteLeaf(Tree*& root, int k);
    bool deleteNodeWithOneChild(Tree*& root, int k);
    bool deleteNodeWithTwoChildren(Tree*& root, int k);
    bool deleteNode(Tree*& root, int k);
    void symmetrical(Tree* root);
    void fromTopToBottom(Tree* root);
    void fromBottomToTop(Tree* root);
    int getHeight(Tree* root);
    void BFS(Tree* root);
    void BFSRecursiveHelper(Tree* root, int level);
};

#endif
