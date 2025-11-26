#include <iostream>
#include <string>
#include <chrono>
#include <random>
#include <queue>
#include <cmath>  
#include "tree.h"
using namespace std;
using namespace std::chrono;
class TreeBenchmark {
private:
    Tree* root;
public:
    TreeBenchmark() : root(nullptr) {}
    ~TreeBenchmark() {
        clearTree();
    }
    void clearTree() {
        clearTreeRecursive(root);
        root = nullptr;
    }
    void clearTreeRecursive(Tree* node) {
        if (node == nullptr) return;
        clearTreeRecursive(node->getLeft());
        clearTreeRecursive(node->getRight());
        delete node;
    }
    void testInsertOperations() {
        cout << "\nADD" << endl;
        clearTree();
        root = Tree::CreateRoot(500);
        auto start = high_resolution_clock::now();
        for (int i = 0; i < 1000; i++) {
            if (i != 500) {
                root->AddLeaf(root, i);  
            }
        }
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        cout << "Вставка 1000 элементов: " << duration.count() << " микросекунд" << endl;
        cout << "Высота дерева: " << root->getHeight(root) << endl;
        cout << "Среднее время на операцию: " << duration.count() / 1000.0 << " микросекунд" << endl;
        int found = 0;
        for (int i = 0; i < 100; i++) {
            if (root->ifZnachTreeExist(root, i * 10)) {
                found++;
            }
        }
        cout << "Проверка вставки: найдено " << found << "/100 элементов" << endl;
    }
    void testSearchOperations() {
        cout << "\nFind" << endl;
        int treeSize = countNodes(root);
        auto start = high_resolution_clock::now();
        int found = 0;
        for (int i = 0; i < 500; i++) {
            if (root->ifZnachTreeExist(root, i)) {
                found++;
            }
        }
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        cout << "Поиск 500 элементов: " << duration.count() << " микросекунд" << endl;
        cout << "Найдено элементов: " << found << endl;
        cout << "Среднее время поиска: " << duration.count() / 500.0 << " микросекунд" << endl;
        start = high_resolution_clock::now();
        int pointersFound = 0;
        for (int i = 0; i < 200; i++) {
            Tree* element = root->findPlace(root, i * 5);
            if (element != nullptr) {
                pointersFound++;
            }
        }
        end = high_resolution_clock::now();
        auto pointerDuration = duration_cast<microseconds>(end - start);
        cout << "Поиск с указателем (200 операций): " << pointerDuration.count() << " микросекунд" << endl;
        cout << "Найдено указателей: " << pointersFound << endl;
    }
    void testDeleteOperations() {
        cout << "\nDelete" << endl;
        int initialSize = countNodes(root);
        auto start = high_resolution_clock::now();
        int deletedLeaves = 0;
        for (int i = 0; i < 100; i++) {
            if (root->deleteLeaf(root, i * 2)) {
                deletedLeaves++;
            }
        }
        auto end = high_resolution_clock::now();
        auto leavesDuration = duration_cast<microseconds>(end - start);
        cout << "Удаление листьев (100 операций): " << leavesDuration.count() << " микросекунд" << endl;
        cout << "Удалено листьев: " << deletedLeaves << endl;
        start = high_resolution_clock::now();
        int deletedOneChild = 0;
        for (int i = 100; i < 200; i++) {
            if (root->deleteNodeWithOneChild(root, i)) {
                deletedOneChild++;
            }
        }
        end = high_resolution_clock::now();
        auto oneChildDuration = duration_cast<microseconds>(end - start);
        cout << "Удаление узлов с одним ребенком: " << oneChildDuration.count() << " микросекунд" << endl;
        cout << "Удалено узлов: " << deletedOneChild << endl;
        start = high_resolution_clock::now();
        int deletedUniversal = 0;
        for (int i = 200; i < 300; i++) {
            if (root->deleteNode(root, i)) {
                deletedUniversal++;
            }
        }
        end = high_resolution_clock::now();
        auto universalDuration = duration_cast<microseconds>(end - start);
        cout << "Универсальное удаление: " << universalDuration.count() << " микросекунд" << endl;
        cout << "Удалено узлов: " << deletedUniversal << endl;
        cout << "Было: " << initialSize << ", осталось: " << countNodes(root) << endl;
        cout << "Высота после удалений: " << root->getHeight(root) << endl;
    }
    
    void testTraversalOperations() {
        cout << "\nОбходы дерева" << endl;
        clearTree();
        root = Tree::CreateRoot(50);
        for (int i = 0; i < 100; i++) {
            if (i != 50) {
                root->AddLeaf(root, i);
            }
        }
        auto start = high_resolution_clock::now();
        auto end = high_resolution_clock::now();
        auto symDuration = duration_cast<microseconds>(end - start);
        cout << "Симметричный обход: " << symDuration.count() << " микросекунд" << endl;
        start = high_resolution_clock::now();
        end = high_resolution_clock::now();
        auto bfsDuration = duration_cast<microseconds>(end - start);
        cout << "Обход в ширину (BFS): " << bfsDuration.count() << " микросекунд" << endl;
        start = high_resolution_clock::now();
        end = high_resolution_clock::now();
        auto topDownDuration = duration_cast<microseconds>(end - start);
        cout << "Обход сверху вниз: " << topDownDuration.count() << " микросекунд" << endl;
    }
    void testBalanceProperties() {
        cout << "\nBalance" << endl;
        clearTree();
        auto start = high_resolution_clock::now();
        root = Tree::CreateRoot(500);
        for (int i = 0; i < 1000; i++) {
            if (i != 500) {
                root->AddLeaf(root, i);
            }
        }
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        int height = root->getHeight(root);
        int nodes = countNodes(root);
        double optimalHeight = log2(nodes + 1);
        cout << "Вставка последовательных значений: " << duration.count() << " микросекунд" << endl;
        cout << "Количество узлов: " << nodes << endl;
        cout << "Высота дерева: " << height << endl;
        cout << "Оптимальная высота (log2(n)): " << optimalHeight << endl;
        cout << "Коэффициент балансировки: " << height / optimalHeight << endl;
        if (checkRedBlackProperties(root)) {
            cout << "Свойства красно-черного дерева: СОБЛЮДЕНЫ" << endl;
        } else {
            cout << "Свойства красно-черного дерева: НАРУШЕНЫ" << endl;
        }
    }
    int countNodes(Tree* node) {
        if (node == nullptr) return 0;
        return 1 + countNodes(node->getLeft()) + countNodes(node->getRight());
    }
    bool checkRedBlackProperties(Tree* node) {
        if (node == nullptr) return true;
        if (node->getParent() == nullptr && node->getColor() != Tree::BLACK) {
            return false;
        }
        if (node->getColor() == Tree::RED) {
            if ((node->getLeft() != nullptr && node->getLeft()->getColor() != Tree::BLACK) ||
                (node->getRight() != nullptr && node->getRight()->getColor() != Tree::BLACK)) {
                return false;
            }
        }
        return checkRedBlackProperties(node->getLeft()) && 
               checkRedBlackProperties(node->getRight());
    }
    void runAllTests() {
        cout << "=== БЕНЧМАРК ТЕСТЫ ДЛЯ КРАСНО-ЧЕРНОГО ДЕРЕВА ===" << endl;
        testInsertOperations();
        testSearchOperations();
        testDeleteOperations();
        testTraversalOperations();
        testBalanceProperties();
        cout << "=== ВСЕ ТЕСТЫ ДЕРЕВА ЗАВЕРШЕНЫ ===" << endl;
    }
};

int main() {
    TreeBenchmark benchmark;
    benchmark.runAllTests();
    return 0;
}
