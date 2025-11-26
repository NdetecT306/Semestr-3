#include "tree.h"
// Конструкторы

// Конструкторы
Tree::Tree() : value(0), color(RED), left(nullptr), right(nullptr), parent(nullptr) {}
Tree::Tree(int val) : value(val), color(RED), left(nullptr), right(nullptr), parent(nullptr) {}

// Геттеры
int Tree::getValue() const { return value; }
Tree::Color Tree::getColor() const { return color; }
Tree* Tree::getLeft() const { return left; }
Tree* Tree::getRight() const { return right; }
Tree* Tree::getParent() const { return parent; }

bool Tree::ifZnachTreeExist(Tree* root, int k) {
    if (root == nullptr) return false;
    if (root->value == k) return true;
    if (k < root->value) 
        return ifZnachTreeExist(root->left, k);
    else 
        return ifZnachTreeExist(root->right, k);
}

Tree* Tree::CreateRoot(int k) {
    Tree* root = new Tree;
    root->color = BLACK;
    root->value = k;
    root->left = nullptr;
    root->right = nullptr;
    root->parent = nullptr;
    return root;
}

Tree* Tree::CreateNode(int k) {
    Tree* node = new Tree;
    node->value = k;
    node->color = RED;
    node->left = nullptr;
    node->right = nullptr;
    node->parent = nullptr;
    return node;
}
Tree* Tree::rotateLeft(Tree*& root, Tree* x) { //Левый поворот
    Tree* y = x->right;
    x->right = y->left;
    if (y->left != nullptr) {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == nullptr) {
        root = y;
    }
    else if (x == x->parent->left) {
        x->parent->left = y;
    }
    else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
    return y;
}
Tree* Tree::rotateRight(Tree*& root, Tree* y) {//Правый поворот
    Tree* x = y->left;
    y->left = x->right;
    if (x->right != nullptr) {
        x->right->parent = y;
    }
    x->parent = y->parent;
    if (y->parent == nullptr) {
        root = x;
    }
    else if (y == y->parent->left) {
        y->parent->left = x;
    }
    else {
        y->parent->right = x;
    }
    x->right = y;
    y->parent = x;
    return x;
}
void Tree::fixInsert(Tree*& root, Tree* ptr) {// Балансировка после вставки
    while (ptr != root && ptr->parent != nullptr && ptr->parent->color == RED) {
        Tree* parent = ptr->parent;
        Tree* grandpa = parent->parent;
        if (parent == grandpa->left) { //Левый потомок
            Tree* uncle = grandpa->right;
            if (uncle != nullptr && uncle->color == RED) { //ДЯДЯ КРАСНЫЙ (случай 2)
                grandpa->color = RED;
                parent->color = BLACK;
                uncle->color = BLACK;
                ptr = grandpa;
            }
            else { 
                if (ptr == parent->right) {//Для случая 4, когда нужны 2 ротации
                    ptr = parent;
                    rotateLeft(root, ptr);
                    parent = ptr->parent; 
                }
                parent->color = BLACK; //Случай 3
                grandpa->color = RED;
                rotateRight(root, grandpa); //Ротация справа
            }
        }
        else {//Правый потомок
            Tree* uncle = grandpa->left;
            if (uncle != nullptr && uncle->color == RED) {
                grandpa->color = RED;
                parent->color = BLACK;
                uncle->color = BLACK;
                ptr = grandpa;
            }
            else {
                if (ptr == parent->left) { ///Для случая 4, когда нужны 2 ротации
                    ptr = parent;
                    rotateRight(root, ptr);
                    parent = ptr->parent; 
                }
                parent->color = BLACK;//Случай 3
                grandpa->color = RED;
                rotateLeft(root, grandpa);//Ротация слева
            }
        }
    }
    root->color = BLACK; //Корень всегда черный (случай 1)
}
Tree* Tree::AddLeaf(Tree*& root, int k) { //Добавление узла
    Tree* element = CreateNode(k);
    if (root == nullptr) {
        element->color = BLACK;
        root = element;
        return root;
    }
    Tree* peremElem = root;// Поиск места вставки
    Tree* parent = nullptr;
    while (peremElem != nullptr) {
        parent = peremElem;
        if (k < peremElem->value)peremElem = peremElem->left;
        else peremElem = peremElem->right;
    }
    element->parent = parent;
    if (k < parent->value) parent->left = element;
    else parent->right = element;
    fixInsert(root, element);
    return root;
}
Tree* Tree::findPlace(Tree* root, int k) {// Поиск узла по значению
    if (root == nullptr || root->value == k) return root;
    if (k < root->value)return findPlace(root->left, k);
    else return findPlace(root->right, k);
}
Tree* Tree::findMinimum(Tree* node) {// Поиск минимального узла в поддереве
    while (node->left != nullptr) {
        node = node->left;
    }
    return node;
}
void Tree::transplant(Tree*& root, Tree* u, Tree* v) {// Функция для трансплантации одного узла на место другого
    if (u->parent == nullptr) {
        root = v;
    }
    else if (u == u->parent->left) {
        u->parent->left = v;
    }
    else {
        u->parent->right = v;
    }
    if (v != nullptr) {
        v->parent = u->parent;
    }
}
void Tree::fixDelete(Tree*& root, Tree* x, Tree* parent) {// Балансировка после удаления
    while (x != root && (x == nullptr || x->color == BLACK)) {
        if (parent == nullptr) break;
        if (x == parent->left) { //Левый ребенок
            Tree* sibling = parent->right;
            if (sibling == nullptr) break;
            if (sibling->color == RED) { // Случай 1: брат красный
                sibling->color = BLACK;
                parent->color = RED;
                rotateLeft(root, parent);
                sibling = parent->right;
                if (sibling == nullptr) break;
            }
            if ((sibling->left == nullptr || sibling->left->color == BLACK) &&(sibling->right == nullptr || sibling->right->color == BLACK)) {// Случай 2: оба ребенка брата черные
                sibling->color = RED;
                x = parent;
                parent = x->parent;
            }
            else {
                if (sibling->right == nullptr || sibling->right->color == BLACK) {// Случай 3: правый ребенок брата черный
                    if (sibling->left != nullptr) {
                        sibling->left->color = BLACK;
                    }
                    sibling->color = RED;
                    rotateRight(root, sibling);
                    sibling = parent->right;
                    if (sibling == nullptr) break;
                }
                sibling->color = parent->color;// Случай 4: правый ребенок брата красный
                parent->color = BLACK;
                if (sibling->right != nullptr) {
                    sibling->right->color = BLACK;
                }
                rotateLeft(root, parent);
                x = root;
                break;
            }
        }
        else {
            Tree* sibling = parent->left;// Правый ребенок
            if (sibling == nullptr) break;
            if (sibling->color == RED) {
                sibling->color = BLACK;
                parent->color = RED;
                rotateRight(root, parent);
                sibling = parent->left;
                if (sibling == nullptr) break;
            }
            if ((sibling->right == nullptr || sibling->right->color == BLACK) &&(sibling->left == nullptr || sibling->left->color == BLACK)) {
                sibling->color = RED;
                x = parent;
                parent = x->parent;
            }
            else {
                if (sibling->left == nullptr || sibling->left->color == BLACK) {
                    if (sibling->right != nullptr) {
                        sibling->right->color = BLACK;
                    }
                    sibling->color = RED;
                    rotateLeft(root, sibling);
                    sibling = parent->left;
                    if (sibling == nullptr) break;
                }
                sibling->color = parent->color;
                parent->color = BLACK;
                if (sibling->left != nullptr) {
                    sibling->left->color = BLACK;
                }
                rotateRight(root, parent);
                x = root;
                break;
            }
        }
    }
    if (x != nullptr) {
        x->color = BLACK;
    }
}
Tree* Tree::findMax(Tree* node) {
    while (node->right != nullptr) {
        node = node->right;
    }
    return node;
}
bool Tree::deleteLeaf(Tree*& root, int k) {
    Tree* z = findPlace(root, k);
    if (z == nullptr) {
        cout << "Узел со значением " << k << " не найден!" << endl;
        return false;
    }
    Tree* y = z;
    Tree* x = nullptr;
    Color y_original_color = y->color;
    if (z->left == nullptr) {
        x = z->right;
        transplant(root, z, z->right);
    }
    else if (z->right == nullptr) {
        x = z->left;
        transplant(root, z, z->left);
    }
    if (y_original_color == BLACK) {
        fixDelete(root, x, z->parent); // Передаем parent
    }
    delete z;
    return true;
}
bool Tree::deleteNodeWithOneChild(Tree*& root, int k) {
    Tree* z = findPlace(root, k);
    if (z == nullptr) {
        cout << "Узел со значением " << k << " не найден!" << endl;
        return false;
    }
    int childCount = 0;
    if (z->left != nullptr) childCount++;
    if (z->right != nullptr) childCount++;
    if (childCount != 1) {
        cout << "Узел " << k << " имеет не одного ребенка! Удаление невозможно." << endl;
        return false;
    }
    Tree* child = (z->left != nullptr) ? z->left : z->right;
    Tree* parent = z->parent;
    Color original_color = z->color;
    
    transplant(root, z, child);
    
    if (original_color == BLACK) {
        if (child != nullptr && child->color == RED) {
            child->color = BLACK;
        } else {
            fixDelete(root, child, parent);
        }
    }
    
    delete z;
    return true;
}
bool Tree::deleteNodeWithTwoChildren(Tree*& root, int k) {
    Tree* z = findPlace(root, k);
    if (z == nullptr) {
        cout << "Узел со значением " << k << " не найден!" << endl;
        return false;
    }
    Tree* successor = findMax(z->left);
    Color z_original_color = z->color;
    Color successor_original_color = successor->color;
    int temp_value = z->value;
    z->value = successor->value;
    successor->value = temp_value;
    Tree* y = successor;
    Tree* x = nullptr;
    if (successor->left == nullptr) {
        x = successor->right;
    }
    else {
        x = successor->left;
    }
    transplant(root, successor, x);
    if (successor_original_color == BLACK) {
        fixDelete(root, x, successor->parent);
    }
    delete successor;
    return true;
}
bool Tree::deleteNode(Tree*& root, int k) {// Универсальная функция удаления любого узла
    Tree* z = findPlace(root, k);
    if (z == nullptr) {
        cout << "Узел со значением " << k << " не найден!" << endl;
        return false;
    }
    if (z->left == nullptr && z->right == nullptr) {// Определяем тип узла и вызываем соответствующую функцию
        return deleteLeaf(root, k);
    }
    else if ((z->left != nullptr && z->right == nullptr) ||(z->left == nullptr && z->right != nullptr)) {
        return deleteNodeWithOneChild(root, k);
    }
    else {
        return deleteNodeWithTwoChildren(root, k); // Добавьте эту строку
    }
}
void Tree::symmetrical(Tree* root) {// Обход симметричный
    if (root == nullptr) return;
    symmetrical(root->left);
    cout << root->value << (root->color == RED ? "r" : "b") << " ";
    symmetrical(root->right);
}
void Tree::fromTopToBottom(Tree* root) {// Обход сверху вниз
    if (root == nullptr) return;
    cout << root->value << (root->color == RED ? "r" : "b") << " ";
    fromTopToBottom(root->left);
    fromTopToBottom(root->right);
}
void Tree::fromBottomToTop(Tree* root) {// Обход снизу вверх
    if (root == nullptr) return;
    fromBottomToTop(root->left);
    fromBottomToTop(root->right);
    cout << root->value << (root->color == RED ? "r" : "b") << " ";
}
void Tree::BFSRecursiveHelper(Tree* root, int level) {// для рекурсивного BFS
    if (root == nullptr) return;
    if (level == 1) {
        cout << root->value << (root->color == RED ? "r" : "b") << " ";
    }
    else if (level > 1) {
        BFSRecursiveHelper(root->left, level - 1);
        BFSRecursiveHelper(root->right, level - 1);
    }
}
int Tree::getHeight(Tree* root) {
    if (root == nullptr) return 0;
    int leftHeight = getHeight(root->left);
    int rightHeight = getHeight(root->right);
    return max(leftHeight, rightHeight) + 1;
}
void Tree::BFS(Tree* root) {
    if (root == nullptr) {
        cout << "Дерево пустое!" << endl;
        return;
    }
    int height = Tree::getHeight(root);
    for (int i = 1; i <= height; i++) {
        BFSRecursiveHelper(root, i);
    }
    cout << endl;
}

