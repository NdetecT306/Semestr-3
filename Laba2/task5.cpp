#include <iostream>
#include <string>
using namespace std;
//ЗАДАНИЕ 5
struct Massiv
{
    int* num;
    int size;
    int C;
};
// Функции для работы с массивом
void createMas(Massiv& A, int C) {
    A.num = new int[C];
    A.size = 0; // изначально элементов нет
    A.C = C;
}
// Добавить элемент в конец
void addMasAtEnd(Massiv& A, int znach) {
    if (A.size >= A.C) {
        cout << "Массив заполнен." << endl;
        return;
    }
    A.num[A.size] = znach;
    A.size++;
}
// Получить элемент по индексу
int getMasPoInd(Massiv& A, int ind) {
    if (ind < 0 || ind >= A.size) {
        cout << "Выход за границы массива." << endl;
        return -1;
    }
    return A.num[ind];
}
// Удалить элемент по индексу
void deleteMasPoInd(Massiv& A, int ind) {
    if (ind < 0 || ind >= A.size) {
        return; // Некорректный индекс
    }
    for (int i = ind; i < A.size - 1; i++) {
        A.num[i] = A.num[i + 1];
    }
    A.size--; // Уменьшаем размер
}
// Заменить элемент по индексу
void ZamenaMas(Massiv& A, int ind, int znach) {
    if (ind < 0 || ind >= A.size) {
        cout << "Нельзя заменить то что не существует." << endl;
        return;
    }
    A.num[ind] = znach;
}
// Получить размер массива
int RazmerMas(Massiv& A) {
    return A.size;
}
// Вывести все элементы массива
void readMas(Massiv& A) {
    if (A.size == 0) {
        cout << "Массив пустой, Ваша честь." << endl;
        return;
    }
    for (int i = 0; i < A.size; i++) {
        cout << A.num[i] << " ";
    }
    cout << endl;
}
// Освободить память массива
void freeMas(Massiv& A) {
    delete[] A.num;
    A.size = 0;
    A.C = 0;
}
enum Color { RED, BLACK }; // 2 цвета
struct Tree //составляющие дерева
{
    int value;
    Color color;
    Tree* left;
    Tree* right;
    Tree* parent;
};
bool ifZnachTreeExist(struct Tree* root, int k) //Существует ли значение
{
    if (root == nullptr) return false;
    if (root->value == k) return true;
    if (ifZnachTreeExist(root->left, k)) return true;
    if (ifZnachTreeExist(root->right, k)) return true;
    return false;
}
Tree* CreateRoot(int k) //Создание корня
{
    Tree* root = new Tree;
    root->color = BLACK;
    root->value = k;
    root->left = nullptr;
    root->right = nullptr;
    root->parent = nullptr;
    return root;
}
Tree* CreateNode(int k) {// Создание нового узла (лист)
    Tree* node = new Tree;
    node->value = k;
    node->color = RED;
    node->left = nullptr;
    node->right = nullptr;
    node->parent = nullptr;
    return node;
}
Tree* rotateLeft(Tree*& root, Tree* x) { //Левый поворот
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
Tree* rotateRight(Tree*& root, Tree* y) {//Правый поворот
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
// Балансировка после вставки
void fixInsert(Tree*& root, Tree* ptr) {
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
Tree* AddLeaf(Tree*& root, int k) { //Добавление узла
    Tree* element = CreateNode(k);
    if (root == nullptr) {
        element->color = BLACK;
        root = element;
        return root;
    }
    // Поиск места вставки
    Tree* peremElem = root;
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
// Поиск узла по значению
Tree* findPlace(Tree* root, int k) {
    if (root == nullptr || root->value == k) return root;
    if (k < root->value)return findPlace(root->left, k);
    else return findPlace(root->right, k);
}
// Поиск минимального узла в поддереве
Tree* findMinimum(Tree* node) {
    while (node->left != nullptr) {
        node = node->left;
    }
    return node;
}
// Функция для трансплантации одного узла на место другого
void transplant(Tree*& root, Tree* u, Tree* v) {
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
// Балансировка после удаления
void fixDelete(Tree*& root, Tree* x, Tree* parent) {
    while (x != root && (x == nullptr || x->color == BLACK)) {
        if (parent == nullptr) break;
        if (x == parent->left) { //Левый ребенок
            Tree* sibling = parent->right;
            if (sibling == nullptr) break;
            // Случай 1: брат красный
            if (sibling->color == RED) {
                sibling->color = BLACK;
                parent->color = RED;
                rotateLeft(root, parent);
                sibling = parent->right;
                if (sibling == nullptr) break;
            }
            // Случай 2: оба ребенка брата черные
            if ((sibling->left == nullptr || sibling->left->color == BLACK) &&
                (sibling->right == nullptr || sibling->right->color == BLACK)) {
                sibling->color = RED;
                x = parent;
                parent = x->parent;
            }
            else {
                // Случай 3: правый ребенок брата черный
                if (sibling->right == nullptr || sibling->right->color == BLACK) {
                    if (sibling->left != nullptr) {
                        sibling->left->color = BLACK;
                    }
                    sibling->color = RED;
                    rotateRight(root, sibling);
                    sibling = parent->right;
                    if (sibling == nullptr) break;
                }
                // Случай 4: правый ребенок брата красный
                sibling->color = parent->color;
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
            // Правый ребенок
            Tree* sibling = parent->left;
            if (sibling == nullptr) break;
            if (sibling->color == RED) {
                sibling->color = BLACK;
                parent->color = RED;
                rotateRight(root, parent);
                sibling = parent->left;
                if (sibling == nullptr) break;
            }
            if ((sibling->right == nullptr || sibling->right->color == BLACK) &&
                (sibling->left == nullptr || sibling->left->color == BLACK)) {
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

Tree* findMax(Tree* node) {
    while (node->right != nullptr) {
        node = node->right;
    }
    return node;
}
// Удаление узла без детей
bool deleteLeaf(Tree*& root, int k) {
    Tree* z = findPlace(root, k);
    if (z == nullptr) {
        cout << "Узел со значением " << k << " не найден!" << endl;
        return false;
    }
    // Проверка, что узел без детей
    if (z->left != nullptr || z->right != nullptr) {
        cout << "Узел " << k << " имеет детей! Удаление невозможно." << endl;
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
// Удаление узла с одним ребенком
bool deleteNodeWithOneChild(Tree*& root, int k) {
    Tree* z = findPlace(root, k);
    if (z == nullptr) {
        cout << "Узел со значением " << k << " не найден!" << endl;
        return false;
    }
    if ((z->left == nullptr && z->right == nullptr) ||
        (z->left != nullptr && z->right != nullptr)) {
        cout << "Узел " << k << " имеет не одного ребенка! Удаление невозможно." << endl;
        return false;
    }
    Tree* y = z;
    Tree* x = nullptr;
    Color y_original_color = y->color;
    if (z->left != nullptr) {
        x = z->left;
    }
    else {
        x = z->right;
    }
    transplant(root, z, x);
    if (y_original_color == BLACK) {
        fixDelete(root, x, z->parent);
    }
    delete z;
    return true;
}
// Удаление узла с двумя детьми
bool deleteNodeWithTwoChildren(Tree*& root, int k) {
    Tree* z = findPlace(root, k);
    if (z == nullptr) {
        cout << "Узел со значением " << k << " не найден!" << endl;
        return false;
    }
    // Проверка, что узел имеет двух детей
    if (z->left == nullptr || z->right == nullptr) {
        cout << "Узел " << k << " имеет не двух детей! Удаление невозможно." << endl;
        return false;
    }
    // Находим максимальный элемент в левом поддереве (самый правый в левой ветке)
    Tree* successor = findMax(z->left);
    // Сохраняем оригинальные цвета
    Color z_original_color = z->color;
    Color successor_original_color = successor->color;
    // Меняем только значения, сохраняя цвета
    int temp_value = z->value;
    z->value = successor->value;
    successor->value = temp_value;
    // Теперь удаляем successor из его нового места
    // successor всегда будет иметь не более одного правого ребенка
    // (так как это максимальный элемент в левом поддереве)
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
// Универсальная функция удаления любого узла
bool deleteNode(Tree*& root, int k) {
    Tree* z = findPlace(root, k);
    if (z == nullptr) {
        cout << "Узел со значением " << k << " не найден!" << endl;
        return false;
    }
    // Определяем тип узла и вызываем соответствующую функцию
    if (z->left == nullptr && z->right == nullptr) {
        return deleteLeaf(root, k);
    }
    else if ((z->left != nullptr && z->right == nullptr) ||
        (z->left == nullptr && z->right != nullptr)) {
        return deleteNodeWithOneChild(root, k);
    }
    else {
        return deleteNodeWithTwoChildren(root, k); // Добавьте эту строку
    }

}
// Обход симметричный
void symmetrical(Tree* root) {
    if (root == nullptr) return;
    symmetrical(root->left);
    cout << root->value << (root->color == RED ? "r" : "b") << " ";
    symmetrical(root->right);
}
// Обход сверху вниз
void fromTopToBottom(Tree* root) {
    if (root == nullptr) return;
    cout << root->value << (root->color == RED ? "r" : "b") << " ";
    fromTopToBottom(root->left);
    fromTopToBottom(root->right);
}
// Обход снизу вверх
void fromBottomToTop(Tree* root) {
    if (root == nullptr) return;
    fromBottomToTop(root->left);
    fromBottomToTop(root->right);
    cout << root->value << (root->color == RED ? "r" : "b") << " ";
}
// Вспомогательная функция для рекурсивного BFS
void BFSRecursiveHelper(Tree* root, int level) {
    if (root == nullptr) return;
    if (level == 1) {
        cout << root->value << (root->color == RED ? "r" : "b") << " ";
    }
    else if (level > 1) {
        BFSRecursiveHelper(root->left, level - 1);
        BFSRecursiveHelper(root->right, level - 1);
    }
}
int getHeight(Tree* root) {
    if (root == nullptr) return 0;
    int leftHeight = getHeight(root->left);
    int rightHeight = getHeight(root->right);
    return max(leftHeight, rightHeight) + 1;
}
void BFS(Tree* root) {
    if (root == nullptr) {
        cout << "Дерево пустое!" << endl;
        return;
    }
    int height = getHeight(root);
    for (int i = 1; i <= height; i++) {
        BFSRecursiveHelper(root, i);
    }
    cout << endl;
}
// Функция для вычисления глубины узла в готовом дереве
int getNodeDepth(Tree* root, Tree* node) {
    if (root == nullptr || node == nullptr) return 0;

    int depth = 0;
    Tree* current = root;

    while (current != nullptr && current != node) {
        if (node->value < current->value) {
            current = current->left;
        }
        else {
            current = current->right;
        }
        depth++;
    }

    return depth + 1; // +1 потому что глубина включает и сам элемент
}

// Функция для сбора всех уникальных значений из дерева
void collectUniqueValues(Tree* root, Massiv& values) {
    if (root == nullptr) return;

    // Проверяем, нет ли уже этого значения в массиве
    bool exists = false;
    for (int i = 0; i < values.size; i++) {
        if (getMasPoInd(values, i) == root->value) {
            exists = true;
            break;
        }
    }

    // Если значения нет в массиве, добавляем его
    if (!exists) {
        addMasAtEnd(values, root->value);
    }

    collectUniqueValues(root->left, values);
    collectUniqueValues(root->right, values);
}

// Основная функция для обработки ввода и вывода глубин
void processInputWithDepths() {
    Tree* root = nullptr;
    int n;
    // Запрашиваем количество элементов
    cout << "Сколько элементов будет введено? ";
    cin >> n;
    if (n <= 0) {
        cout << "Неверное количество элементов!" << endl;
        return;
    }

    // Используем структуру Massiv вместо обычного массива
    Massiv inputValues;
    createMas(inputValues, n);

    int uniqueCount = 0; // Счетчик уникальных элементов
    cout << "Введите " << n << " элементов: ";
    for (int i = 0; i < n; i++) {
        int value;
        cin >> value;
        addMasAtEnd(inputValues, value);

        // Проверяем, существует ли элемент в дереве
        if (!ifZnachTreeExist(root, value)) {
            // Добавляем элемент в дерево
            if (root == nullptr) {
                root = CreateRoot(value);
            }
            else {
                AddLeaf(root, value);
            }
            uniqueCount++;
        }
    }

    // Теперь, когда дерево полностью построено, вычисляем глубины
    Massiv depths;
    createMas(depths, uniqueCount);

    // Для каждого введенного значения находим его глубину в готовом дереве
    for (int i = 0; i < RazmerMas(inputValues); i++) {
        int value = getMasPoInd(inputValues, i);
        Tree* node = findPlace(root, value);
        if (node != nullptr) {
            // Проверяем, не вычисляли ли мы уже глубину для этого значения
            bool alreadyCalculated = false;
            for (int j = 0; j < RazmerMas(depths); j++) {
                if (getMasPoInd(depths, j) == getNodeDepth(root, node)) {
                    // Ищем соответствующее значение в inputValues чтобы убедиться
                    Tree* existingNode = findPlace(root, getMasPoInd(inputValues, j));
                    if (existingNode != nullptr && existingNode->value == node->value) {
                        alreadyCalculated = true;
                        break;
                    }
                }
            }
            if (!alreadyCalculated) {
                addMasAtEnd(depths, getNodeDepth(root, node));
            }
        }
    }
    // Выводим глубины добавленных элементов
    cout << "Глубины добавленных элементов: ";
    for (int i = 0; i < RazmerMas(depths); i++) {
        cout << getMasPoInd(depths, i);
        if (i < RazmerMas(depths) - 1) {
            cout << " ";
        }
    }
    cout << endl;
    // Освобождаем память
    freeMas(inputValues);
    freeMas(depths);
}
int main() {
    setlocale(LC_ALL, "Russian");
    processInputWithDepths();
    return 0;
}
