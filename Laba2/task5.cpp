#include <iostream>
#include <string>
#include <algorithm>
using namespace std;
const int HASH_SIZE = 256;
const int MAX_SYMBOLS = 256;
struct Huffman {
    string symbols;
    int weight;
    Huffman* left;
    Huffman* right;
    Huffman(char s, int w) : symbols(string(1, s)), weight(w), left(nullptr), right(nullptr) {}
    Huffman(const string& s, int w, Huffman* l, Huffman* r) : symbols(s), weight(w), left(l), right(r) {}
};
struct DOUBLE_HASH {
    char key;
    int value;
    bool isDeleted;
    bool isEmpty;
};
struct CodeTable {
    char symbol;
    string code;
};
int hash1(int key) {
    return key % HASH_SIZE;
}
int hash2(int key) {
    return 13 - (key % 13);
}
DOUBLE_HASH* createHashTable() {
    DOUBLE_HASH* newHash = new DOUBLE_HASH[HASH_SIZE];
    for (int i = 0; i < HASH_SIZE; i++) {
        newHash[i].key = '\0';
        newHash[i].value = 0;
        newHash[i].isDeleted = false;
        newHash[i].isEmpty = true;
    }
    return newHash;
}
bool HASH_ADD(DOUBLE_HASH* hashTable, char key, int value) {
    int intKey = static_cast<unsigned char>(key);
    int index = hash1(intKey);
    int step = hash2(intKey);
    int startIndex = index;
    int attempts = 0;
    while (attempts < HASH_SIZE) {
        if (hashTable[index].isEmpty || hashTable[index].isDeleted) {
            hashTable[index].key = key;
            hashTable[index].value = value;
            hashTable[index].isDeleted = false;
            hashTable[index].isEmpty = false;
            return true;
        }
        if (hashTable[index].key == key && !hashTable[index].isDeleted) {
            hashTable[index].value = value;
            return true;
        }
        index = (index + step) % HASH_SIZE;
        attempts++;
        if (index == startIndex) {
            break;
        }
    }
    return false;
}
bool HASH_GET(DOUBLE_HASH* hashTable, char key, int& result) {
    int intKey = static_cast<unsigned char>(key);
    int index = hash1(intKey);
    int step = hash2(intKey);
    int startIndex = index;
    int attempts = 0;
    while (attempts < HASH_SIZE) {
        if (hashTable[index].isEmpty && !hashTable[index].isDeleted) {
            return false;
        }
        if (hashTable[index].key == key && !hashTable[index].isDeleted) {
            result = hashTable[index].value;
            return true;
        }
        index = (index + step) % HASH_SIZE;
        attempts++;
        if (index == startIndex) {
            break;
        }
    }
    return false;
}
DOUBLE_HASH* Chastota(const string& text) {
    DOUBLE_HASH* frequencies = createHashTable();
    for (char c : text) {
        int currentValue;
        if (HASH_GET(frequencies, c, currentValue)) {
            HASH_ADD(frequencies, c, currentValue + 1);
        } else {
            HASH_ADD(frequencies, c, 1);
        }
    }
    return frequencies;
}
// Получение количества элементов в хэш-таблице
int getHashTableSize(DOUBLE_HASH* hashTable) {
    int count = 0;
    for (int i = 0; i < HASH_SIZE; i++) {
        if (!hashTable[i].isEmpty && !hashTable[i].isDeleted) {
            count++;
        }
    }
    return count;
}
void getHashTableEntries(DOUBLE_HASH* hashTable, Huffman** nodes, int& count) {
    count = 0;
    for (int i = 0; i < HASH_SIZE; i++) {
        if (!hashTable[i].isEmpty && !hashTable[i].isDeleted) {
            nodes[count++] = new Huffman(hashTable[i].key, hashTable[i].value);
        }
    }
}
void sortNodes(Huffman** nodes, int size) { //Сортировка
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (nodes[j]->weight < nodes[j + 1]->weight || 
                (nodes[j]->weight == nodes[j + 1]->weight && nodes[j]->symbols > nodes[j + 1]->symbols)) {
                Huffman* temp = nodes[j];
                nodes[j] = nodes[j + 1];
                nodes[j + 1] = temp;
            }
        }
    }
}
void findTwoSmallest(Huffman** nodes, int size, int& idx1, int& idx2) {// Поиск двух узлов с наименьшим весом
    idx1 = -1;
    idx2 = -1;
    for (int i = 0; i < size; i++) {
        if (nodes[i] == nullptr) continue;
        
        if (idx1 == -1 || nodes[i]->weight < nodes[idx1]->weight) {
            idx2 = idx1;
            idx1 = i;
        } else if (idx2 == -1 || nodes[i]->weight < nodes[idx2]->weight) {
            idx2 = i;
        }
    }
}
Huffman* build(DOUBLE_HASH* frequencies) {
    Huffman* nodes[MAX_SYMBOLS] = {nullptr};
    int nodeCount = 0;
    getHashTableEntries(frequencies, nodes, nodeCount); //Читаем таблицу
    sortNodes(nodes, nodeCount); //От тяжелых к легким
    cout << "Начальные узлы (отсортированные по убыванию частоты):" << endl;
    for (int i = 0; i < nodeCount; i++) {
        cout << "'" << nodes[i]->symbols << "': " << nodes[i]->weight << endl;
    }
    cout << endl;
    while (nodeCount > 1) { //Построим дерево         
        Huffman* node1 = nodes[nodeCount - 1]; //Берем 2 самых легких узла
        Huffman* node2 = nodes[nodeCount - 2];
        string combinedSymbols = node1->symbols + node2->symbols; //Обьединяем их браком (в один элемент) и считаем их вес
        int combinedWeight = node1->weight + node2->weight;
        Huffman* parent = new Huffman(combinedSymbols, combinedWeight, node1, node2); //Добавляем в реестр семью
        nodes[nodeCount - 2] = parent;
        nodeCount--;
        sortNodes(nodes, nodeCount);
    }
    return nodes[0];
}
void generateCodes(Huffman* node, const string& code, CodeTable* codes, int& codeCount) {// Генерация кодов Хаффмана
    if (!node) return;
    if (node->symbols.length() == 1) {
        codes[codeCount].symbol = node->symbols[0];
        codes[codeCount].code = code;
        codeCount++;
        return;
    }
    generateCodes(node->left, code + "0", codes, codeCount);
    generateCodes(node->right, code + "1", codes, codeCount);
}
string findCode(CodeTable* codes, int codeCount, char symbol) { //Поиск
    for (int i = 0; i < codeCount; i++) {
        if (codes[i].symbol == symbol) {
            return codes[i].code;
        }
    }
    return "";
}
string encode(const string& text, CodeTable* codes, int codeCount) { //Кодировка
    string encoded = "";
    
    for (char c : text) {
        encoded += findCode(codes, codeCount, c);
    }
    
    return encoded;
}
string decode(const string& encoded, Huffman* root) { //Декодировка
    string decoded = "";
    Huffman* current = root;
    for (char bit : encoded) {
        if (bit == '0') {
            current = current->left;
        } else {
            current = current->right;
        }
        if (current->symbols.length() == 1) {
            decoded += current->symbols[0];
            current = root;
        }
    }
    return decoded;
}
// Вывод дерева
void printTree(Huffman* node, const string& prefix = "", bool isLeft = true) {
    if (!node) return;
    if (node->right) {
        printTree(node->right, prefix + (isLeft ? "│   " : "    "), false);
    }
    cout << prefix;
    cout << (isLeft ? "└──" : "┌──");
    if (node->symbols.length() == 1) {
        cout << "'" << node->symbols << "' (" << node->weight << ")" << endl;
    } else {
        cout << "[" << node->symbols << "] (" << node->weight << ")" << endl;
    }
    
    if (node->left) {
        printTree(node->left, prefix + (isLeft ? "    " : "│   "), true);
    }
}
int main() {
    string input;
    cout << "Введите строку для кодирования: ";
    getline(cin, input);
    cout << "Исходная строка: " << input << endl;
    DOUBLE_HASH* frequencies = Chastota(input);
    Huffman* root = build(frequencies);
    CodeTable codes[MAX_SYMBOLS];
    int codeCount = 0;
    generateCodes(root, "", codes, codeCount);
    cout << "Полученные коды Хаффмана:" << endl;
    for (int i = 0; i < codeCount; i++) {
        cout << "'" << codes[i].symbol << "': " << codes[i].code << endl;
    }
    cout << "\nДерево Хаффмана:" << endl;
    printTree(root);
    string encoded = encode(input, codes, codeCount);
    cout << "\nЗакодированная строка: " << encoded << endl;
    int originalBits = input.length() * 8;
    int encodedBits = encoded.length();
    double compressionRatio = (1.0 - (double)encodedBits / originalBits) * 100.0;
    string decoded = decode(encoded, root);
    cout << "Декодированная строка: " << decoded << endl;
    return 0;
}
