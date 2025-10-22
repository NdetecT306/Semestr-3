#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
using namespace std;
struct Huffman {
    string symbols;  // Символы в узле 
    int weight;      // Вес (частота)
    Huffman* left; //указатель влево и право для передвижения по дереву
    Huffman* right;
    Huffman(char s, int w) : symbols(string(1, s)), weight(w), left(nullptr), right(nullptr) {} //Конструкторы
    Huffman(const string& s, int w, Huffman* l, Huffman* r) : symbols(s), weight(w), left(l), right(r) {}
};
struct MAP{
    char el;
    int key;
};
map<char, int> calculateFrequencies(const string& text) { //Частота появления символов 
    map<char, int> frequencies;
    for (char c : text) {
        frequencies[c]++;
    }
    return frequencies;
}
bool compare(const Huffman* a, const Huffman* b) { //Сортировка от тяжелых к легким
    if (a->weight == b->weight) {
        return a->symbols < b->symbols;  // При равных весах сортируем по символам
    }
    return a->weight > b->weight;  
}
Huffman* build(const map<char, int>& frequencies) {
    vector<Huffman*> nodes;
    // Шаг 2: Создаем список свободных узлов
    for (const auto& pair : frequencies) {
        nodes.push_back(new Huffman(pair.first, pair.second));
    }
    // Сортируем по убыванию веса (шаг 1)
    sort(nodes.begin(), nodes.end(), compare);
    cout << "Начальные узлы (отсортированные по убыванию частоты):" << endl;
    for (const auto& node : nodes) {
        cout << "'" << node->symbols << "': " << node->weight << endl;
    }
    cout << endl;
    int step = 1;
    // Шаги 3-7: Пока в списке не останется один узел
    while (nodes.size() > 1) {
        // Шаг 3: Выбираем два узла с наименьшим весом (последние в отсортированном списке)
        Huffman* node1 = nodes.back();
        nodes.pop_back();
        Huffman* node2 = nodes.back();
        nodes.pop_back();
        // Шаг 4: Создаем родительский узел
        string combinedSymbols = node1->symbols + node2->symbols;
        int combinedWeight = node1->weight + node2->weight;
        // Шаг 6: Левой дуге - 0, правой - 1
        Huffman* parent = new Huffman(combinedSymbols, combinedWeight, node1, node2);
        // Шаг 5: Добавляем новый узел в список
        nodes.push_back(parent);
        // Сортируем список по убыванию веса
        sort(nodes.begin(), nodes.end(), compare);
    }
    return nodes[0];
}
// Функция для генерации кодов Хаффмана
void generateCodes(Huffman* node, const string& code, map<char, string>& codes) {
    if (!node) return;
    // Если это листовой узел (один символ)
    if (node->symbols.length() == 1) {
        codes[node->symbols[0]] = code;
        return;
    }
    // Левой дуге - 0, правой - 1 (точно по описанию шага 6)
    generateCodes(node->left, code + "0", codes);
    generateCodes(node->right, code + "1", codes);
}
// Функция для кодирования строки
string encode(const string& text, const map<char, string>& codes) {
    string encoded = "";
    for (char c : text) {
        encoded += codes.at(c);
    }
    return encoded;
}
// Функция для декодирования строки
string decode(const string& encoded, Huffman* root) {
    string decoded = "";
    Huffman* current = root;
    for (char bit : encoded) {
        if (bit == '0') {
            current = current->left;
        } else {
            current = current->right;
        }
        // Если достигли листового узла (один символ)
        if (current->symbols.length() == 1) {
            decoded += current->symbols[0];
            current = root;
        }
    }
    return decoded;
}

void printTree(Huffman* node, const string& prefix = "", bool isLeft = true) {
    if (!node) return;
    // Сначала выводим правого потомка (вверху)
    if (node->right) {
        printTree(node->right, prefix + (isLeft ? "│   " : "    "), false);
    }
    // Затем выводим текущий узел
    cout << prefix;
    cout << (isLeft ? "└──" : "┌──");
    // Выводим узел
    if (node->symbols.length() == 1) {
        cout << "'" << node->symbols << "' (" << node->weight << ")" << endl;
    } else {
        cout << "[" << node->symbols << "] (" << node->weight << ")" << endl;
    }
    // Затем выводим левого потомка (внизу)
    if (node->left) {
        printTree(node->left, prefix + (isLeft ? "    " : "│   "), true);
    }
}
// Функция для очистки памяти
void deleteTree(Huffman* node) {
    if (!node) return;
    deleteTree(node->left);
    deleteTree(node->right);
    delete node;
}
int main() {
    string input;
    cout << "Введите строку для кодирования: ";
    getline(cin, input);
    if (input.empty()) {
        input = "HUFFMAN";  // Значение по умолчанию для демонстрации
        cout << "Используется строка по умолчанию: " << input << endl;
    }
    cout << "\n=== КОДИРОВАНИЕ ХАФФМАНА ===" << endl;
    cout << "Исходная строка: " << input << endl;
    // Шаг 1: Вычисляем частоты символов
    map<char, int> frequencies = calculateFrequencies(input);
    // Строим дерево Хаффмана (шаги 2-7)
    cout << "\nПостроение дерева Хаффмана:" << endl;
    Huffman* root = build(frequencies);
    // Генерируем коды
    map<char, string> codes;
    generateCodes(root, "", codes);
    cout << "Полученные коды Хаффмана:" << endl;
    for (const auto& pair : codes) {
        cout << "'" << pair.first << "': " << pair.second << endl;
    }
    // Выводим дерево для наглядности
    cout << "\nДерево Хаффмана:" << endl;
    printTree(root);
    // Кодируем строку
    string encoded = encode(input, codes);
    cout << "\nЗакодированная строка: " << encoded << endl;
    // Вычисляем статистику
    int originalBits = input.length() * 8;
    int encodedBits = encoded.length();
    double compressionRatio = (1.0 - (double)encodedBits / originalBits) * 100.0;
    string decoded = decode(encoded, root);
    cout << "Декодированная строка: " << decoded << endl;
    // Очищаем память
    deleteTree(root);
    return 0;
}
