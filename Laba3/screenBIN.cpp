#include "screen.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>

map<string, Queue*> queues;
map<string, Stack*> stacks;
map<string, OdSpis*> lists;
map<string, Array> arrays;
map<string, StudentGroup*> groups; 
map<string, Queue*> queueTails;
map<string, Tree*> trees;
map<string, DoubleHashTable*> hashTables;  // Добавлен map для хэш-таблиц

// Функции для работы с хэш-таблицами
string HCREATE(const string& tableName) {
    if (hashTables.find(tableName) != hashTables.end()) {
        delete hashTables[tableName];
    }
    hashTables[tableName] = new DoubleHashTable();
    return tableName;
}

string HADD(const string& tableName, int key, const string& value) {
    if (hashTables.find(tableName) == hashTables.end()) return "Хэш-таблица не найдена";
    bool success = hashTables[tableName]->insert(key, value);
    return success ? value : "Ошибка добавления (возможно, ключ уже существует)";
}

string HDEL(const string& tableName, int key, const string& value) {
    if (hashTables.find(tableName) == hashTables.end()) return "Хэш-таблица не найдена";
    bool success = hashTables[tableName]->remove(key, value);
    return success ? "Элемент удален" : "Элемент не найден";
}

string HSEARCH(const string& tableName, int key, const string& value) {
    if (hashTables.find(tableName) == hashTables.end()) return "Хэш-таблица не найдена";
    bool found = hashTables[tableName]->contains(key, value);
    return found ? "Найден" : "Не найден";
}

string HGET(const string& tableName) {
    if (hashTables.find(tableName) == hashTables.end()) return "Хэш-таблица не найдена";
    
    int count = hashTables[tableName]->elementCount();
    if (count == 0) return "Хэш-таблица пустая";
    
    int* keys = new int[count];
    string* values = new string[count];
    int actualCount;
    
    hashTables[tableName]->getAllElements(keys, values, actualCount);
    
    stringstream output;
    for (int i = 0; i < actualCount; i++) {
        output << keys[i] << ":" << values[i];
        if (i < actualCount - 1) output << " ";
    }
    
    delete[] keys;
    delete[] values;
    
    return output.str();
}

string HSIZE(const string& tableName) {
    if (hashTables.find(tableName) == hashTables.end()) return "Хэш-таблица не найдена";
    return to_string(hashTables[tableName]->elementCount());
}
string TADD(const string& treeName, int value) {
    if (trees.find(treeName) == trees.end()) {
        trees[treeName] = Tree::CreateRoot(value);
        return to_string(value);
    } else {
        trees[treeName]->AddLeaf(trees[treeName], value);
        return to_string(value);
    }
}

string TSEARCH(const string& treeName, int value) {
    if (trees.find(treeName) == trees.end()) return "Дерево не найдено";
    bool found = trees[treeName]->ifZnachTreeExist(trees[treeName], value);
    return found ? "Найден" : "Не найден";
}

string TDELETE(const string& treeName, int value) {
    if (trees.find(treeName) == trees.end()) return "Дерево не найдено";
    bool success = trees[treeName]->deleteNode(trees[treeName], value);
    return success ? "Элемент удален" : "Элемент не найден или невозможно удалить";
}

string TGET(const string& treeName) {
    if (trees.find(treeName) == trees.end()) return "Дерево не найдено";
    if (trees[treeName] == nullptr) return "Дерево пустое";
    stringstream output;
    auto old_cout_buffer = cout.rdbuf(output.rdbuf());
    trees[treeName]->BFS(trees[treeName]);
    cout.rdbuf(old_cout_buffer);
    string result = output.str();
    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }
    return result;
}
string TGETSYM(const string& treeName) {
    if (trees.find(treeName) == trees.end()) return "Дерево не найдено";
    if (trees[treeName] == nullptr) return "Дерево пустое";
    stringstream output;
    auto old_cout_buffer = cout.rdbuf(output.rdbuf());
    trees[treeName]->symmetrical(trees[treeName]);
    cout.rdbuf(old_cout_buffer);
    string result = output.str();
    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }
    return result;
}

string TDEL(const string& treeName, int value) {
    if (trees.find(treeName) == trees.end()) return "Дерево не найдено";
    else {
        bool success = trees[treeName]->deleteNode(trees[treeName], value);
        return success ? to_string(value) : "Элемент не найден или невозможно удалить";
    }
}

void printTreeStructure(Tree* node, stringstream& output, const string& prefix, bool isLeft) {
    if (node == nullptr) return;
    output << prefix;
    output << (isLeft ? "├----- " : "└----- ");
    output << node->getValue() << (node->getColor() == Tree::RED ? "r" : "b") << endl;
    string newPrefix = prefix + (isLeft ? "│   " : "    ");
    printTreeStructure(node->getLeft(), output, newPrefix, true);
    printTreeStructure(node->getRight(), output, newPrefix, false);
}

string TREAD(const string& treeName) {
    if (trees.find(treeName) == trees.end()) return "Дерево не найдено";
    if (trees[treeName] == nullptr) return "Дерево пустое";
    stringstream output;
    printTreeStructure(trees[treeName], output, "", true);
    string result = output.str();
    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }
    return result;
}

string LPOISK(const string& groupName, const string& str) {
    if (groups.find(groupName) == groups.end()) return "Двусвязный список не найден";
    bool found = groups[groupName]->contains(str);
    return found ? "Найден" : "Не найден";
}

string LPOPVALUE(const string& groupName, const string& str) {
    if (groups.find(groupName) == groups.end()) return "Двусвязный список не найден";
    if (!groups[groupName]->contains(str)) return "Элемент в двусвязном списке не найден";
    groups[groupName]->deleteStudentByValue(str);
    return "Элемент удален";
}

string LPUSHBEGIN(const string& groupName, const string& str) {
    if (groups.find(groupName) == groups.end()) {
        groups[groupName] = new StudentGroup(str);
    } else {
        groups[groupName]->addStudentToStart(str);
    }
    return str;
}

string LPUSHEND(const string& groupName, const string& str) {
    if (groups.find(groupName) == groups.end()) {
        groups[groupName] = new StudentGroup(str);
    } else {
        groups[groupName]->addStudentToEnd(str);
    }
    return str;
}
string LPUSHBEFORE(const string& groupName, const string& targetElement, const string& str) {
    if (groups.find(groupName) == groups.end()) return "Двусвязный список не найден";
    if (!groups[groupName]->contains(targetElement)) return "Элемент в двусвязном списке не найден";
    groups[groupName]->addStudentBefore(targetElement, str);
    return str;
}

string LPUSHAFTER(const string& groupName, const string& pointerElement, const string& str) {
    if (groups.find(groupName) == groups.end()) return "Двусвязный список не найден";
    if (!groups[groupName]->contains(pointerElement)) return "Элемент в двусвязном списке не найден";
    groups[groupName]->addStudentAfter(pointerElement, str);
    return str;
}

string LPOPBEFORE(const string& groupName, const string& str) {
    if (groups.find(groupName) == groups.end()) return "Двусвязный список не найден";
    if (!groups[groupName]->contains(str)) return "Элемент в двусвязном списке не найден";
    if (groups[groupName]->getFirst() == str) return "Выше головы не прыгнешь, рубить нечего";
    groups[groupName]->deleteStudentBefore(str);
    return "Элемент удален";
}

string LPOPAFTER(const string& groupName, const string& str) {
    if (groups.find(groupName) == groups.end()) return "Двусвязный список не найден";
    if (!groups[groupName]->contains(str)) return "Элемент в двусвязном списке не найден";
    if (groups[groupName]->getLast() == str) return "За последним элементом ничего нет, удалять нечего";
    groups[groupName]->deleteStudentAfter(str);
    return "Элемент удален";
}

string LPOPBEGIN(const string& groupName) {
    if (groups.find(groupName) == groups.end()) return "Двусвязный список не найден";
    if (groups[groupName]->isEmpty()) return "Двусвязный список пустой";
    groups[groupName]->deleteStudentFromStart();
    return "Элемент удален";
}

string LPOPEND(const string& groupName) {
    if (groups.find(groupName) == groups.end()) return "Двусвязный список не найден";
    if (groups[groupName]->isEmpty()) return "Двусвязный список пустой";
    groups[groupName]->deleteStudentFromEnd();
    return "Элемент удален";
}
string LGET(const string& groupName) {
    if (groups.find(groupName) == groups.end()) return "Двусвязный список не найден";
    if (groups[groupName]->isEmpty()) return "Двусвязный список пустой";
    stringstream output;
    auto old_cout_buffer = cout.rdbuf(output.rdbuf());
    groups[groupName]->printForward();
    cout.rdbuf(old_cout_buffer);
    string result = output.str();
    if (!result.empty() && result.back() == '\n') result.pop_back();
    return result;
}

string LGETREV(const string& groupName) {
    if (groups.find(groupName) == groups.end()) return "Двусвязный список не найден";
    if (groups[groupName]->isEmpty()) return "Двусвязный список пустой";
    stringstream output;
    auto old_cout_buffer = cout.rdbuf(output.rdbuf());
    groups[groupName]->printReverse();
    cout.rdbuf(old_cout_buffer);
    string result = output.str();
    if (!result.empty() && result.back() == '\n') result.pop_back();
    return result;
}

bool findInGroup(StudentGroup* group, const string& str) {
    if (group == nullptr) return false;
    return group->contains(str);
}

int countGroup(StudentGroup* group) {
    if (group == nullptr) return 0;
    int count = 0;
    stringstream ss;
    auto old_cout_buffer = cout.rdbuf(ss.rdbuf());
    group->printForward();
    cout.rdbuf(old_cout_buffer);
    
    string content = ss.str();
    if (!content.empty()) {
        istringstream iss(content);
        string item;
        while (iss >> item) {
            count++;
        }
    }
    
    return count;
}
void deleteGroup(StudentGroup*& group) {
    if (group != nullptr) {
        delete group;
        group = nullptr;
    }
}
string MCREATE(const string& A, int C) {
    if (arrays.find(A) != arrays.end()) {
        // Если массив уже существует, создаем новый (старый удалится автоматически)
        arrays[A] = Array(C);
    } else {
        arrays[A] = Array(C);
    }
    return to_string(C);
}

string MPUSHIND(const string& A, int index, string value) {
    if (arrays.find(A) == arrays.end()) return "Массив не найден";
    if (index < 0 || index > arrays[A].getSize()) return "Индекс вышел за границы";
    if (arrays[A].getSize() >= arrays[A].getCapacity()) return "Массив переполнен";
    
    arrays[A].addMasAtInd(index, value);
    return value;
}

string MPUSHEND(const string& A, string value) {
    if (arrays.find(A) == arrays.end()) return "Массив не найден";
    if (arrays[A].getSize() >= arrays[A].getCapacity()) return "Массив переполнен";
    
    arrays[A].addMasAtEnd(value);
    return value;
}

string MGETIND(const string& A, int index) {
    if (arrays.find(A) == arrays.end()) return "Массив не найден";
    if (index < 0 || index >= arrays[A].getSize()) return "Индекс вышел за границы";
    return arrays[A].getElementAt(index);
}

string MDELIND(const string& A, int index) {
    if (arrays.find(A) == arrays.end()) return "Массив не найден";
    if (index < 0 || index >= arrays[A].getSize()) return "Индекс вышел за границы";
    arrays[A].deleteMasPoInd(index);
    return "Элемент удален";
}

string MREPLACE(const string& A, int index, string value) {
    if (arrays.find(A) == arrays.end()) return "Массив не найден";
    if (index < 0 || index >= arrays[A].getSize()) return "Индекс за границами";
    arrays[A].ZamenaMas(index, value);
    return value;
}

string MSIZE(const string& A) {
    if (arrays.find(A) == arrays.end()) return "Массив не найден";
    return to_string(arrays[A].getSize());
}

string MGET(const string& A) {
    if (arrays.find(A) == arrays.end()) return "Массив не найден";
    if (arrays[A].getSize() == 0) return "Массив пуст";
    
    stringstream output;
    streambuf* old_cout_buffer = cout.rdbuf(output.rdbuf());
    arrays[A].readMas();
    cout.rdbuf(old_cout_buffer);
    
    string result = output.str();
    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }
    return result;
}
string FPUSHBEFORE(const string& listName, const string& targetElement, const string& newElement) {
    if (lists.find(listName) == lists.end()) return "Список не найден";
    OdSpis* target = lists[listName]->FindElement(lists[listName], targetElement);
    if (target == nullptr) return "Элемент не найден";
    lists[listName]->AddChelDo(lists[listName], target, newElement);
    return newElement;
}

string FPUSHEND(const string& listName, const string& newElement) {
    if (lists.find(listName) == lists.end()) {
        lists[listName] = new OdSpis(newElement);
        return newElement;
    }
    lists[listName]->AddToEnd(lists[listName], newElement);
    return newElement;
}

string FPUSHAFTER(const string& listName, const string& targetElement, const string& newElement) {
    if (lists.find(listName) == lists.end()) return "Список не найден";
    OdSpis* target = lists[listName]->FindElement(lists[listName], targetElement);
    if (target == nullptr) return "Элемент не найден";
    lists[listName]->AddChelPosle(target, newElement);
    return newElement;
}

string FPUSHBEGIN(const string& listName, const string& newElement) {
    if (lists.find(listName) == lists.end()) {
        lists[listName] = new OdSpis(newElement);
        return newElement;
    }
    lists[listName]->AddToBegin(lists[listName], newElement);
    return newElement;
}

string FPOISK(const string& listName, const string& searchElement) {
    if (lists.find(listName) == lists.end()) return "Список не найден";
    bool found = lists[listName]->poisk(lists[listName], searchElement);
    return found ? "Найден" : "НЕ найден";
}

string FDELVALUE(const string& listName, const string& elementToDelete) {
    if (lists.find(listName) == lists.end()) return "Список не найден";
    if (!lists[listName]->poisk(lists[listName], elementToDelete)) return "Элемент не найден";
    lists[listName]->deletePoZnach(lists[listName], elementToDelete);
    return "Удален";
}

string FDELBEGIN(const string& listName) {
    if (lists.find(listName) == lists.end()) return "Список не найден";
    if (lists[listName] == nullptr) return "Список пуст";
    lists[listName]->deleteToBegin(lists[listName]);
    return "Удален";
}

string FDELEND(const string& listName) {
    if (lists.find(listName) == lists.end()) return "Список не найден";
    if (lists[listName] == nullptr) return "Список пуст";
    lists[listName]->deleteToEnd(lists[listName]);
    return "Удален";
}

string FDELBEFORE(const string& listName, const string& targetElement) {
    if (lists.find(listName) == lists.end()) return "Список не найден";
    OdSpis* target = lists[listName]->FindElement(lists[listName], targetElement);
    if (target == nullptr) return "Элемент не найден";
    if (target == lists[listName]) return "Не может быть удален несуществующий в пустоте элемент";
    lists[listName]->deleteChelDo(lists[listName], target);
    return "Удален";
}

string FDELAFTER(const string& listName, const string& targetElement) {
    if (lists.find(listName) == lists.end()) return "Список не найден";
    OdSpis* target = lists[listName]->FindElement(lists[listName], targetElement);
    if (target == nullptr) return "Элемент не найден";
    if (target->getNext() == nullptr) return "Нельзя удалить элемент, существующий где-то после листа";
    lists[listName]->deleteChelPosle(target);
    return "Удален";
}

string FGET(const string& listName) {
    if (lists.find(listName) == lists.end()) return "Список не найден";
    if (lists[listName] == nullptr) return "Список пуст";
    stringstream output;
    auto old_cout_buffer = cout.rdbuf(output.rdbuf());
    lists[listName]->print(lists[listName]);
    cout.rdbuf(old_cout_buffer);
    string result = output.str();
    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }
    return result;
}
void saveTreeBinary(Tree* root, ofstream& file) {
    if (root == nullptr) {
        int32_t marker = -1; // маркер пустого узла
        file.write(reinterpret_cast<const char*>(&marker), sizeof(marker));
        return;
    }
    
    // Сохраняем значение и цвет
    int32_t value = root->getValue();
    file.write(reinterpret_cast<const char*>(&value), sizeof(value));
    
    char color = (root->getColor() == Tree::RED) ? 'r' : 'b';
    file.write(&color, sizeof(color));
    
    // Рекурсивно сохраняем левое и правое поддеревья
    saveTreeBinary(root->getLeft(), file);
    saveTreeBinary(root->getRight(), file);
}

// Бинарная десериализация дерева
Tree* loadTreeBinary(ifstream& file) {
    int32_t value;
    file.read(reinterpret_cast<char*>(&value), sizeof(value));
    if (value == -1) { // маркер пустого узла
        return nullptr;
    }
    char color;
    file.read(&color, sizeof(color));
    Tree* node = new Tree(value);
    Tree* leftChild = loadTreeBinary(file);
    Tree* rightChild = loadTreeBinary(file);
    delete node; 
}
void saveHashTableBinary(DoubleHashTable* table, ofstream& file) {
    if (table == nullptr) {
        int32_t count = 0;
        file.write(reinterpret_cast<const char*>(&count), sizeof(count));
        return;
    }
    
    int count = table->elementCount();
    file.write(reinterpret_cast<const char*>(&count), sizeof(count));
    
    if (count > 0) {
        int* keys = new int[count];
        string* values = new string[count];
        int actualCount;
        table->getAllElements(keys, values, actualCount);
        
        for (int i = 0; i < actualCount; i++) {
            // Сохраняем ключ
            file.write(reinterpret_cast<const char*>(&keys[i]), sizeof(keys[i]));
            
            // Сохраняем значение
            int32_t valueLength = values[i].length();
            file.write(reinterpret_cast<const char*>(&valueLength), sizeof(valueLength));
            file.write(values[i].c_str(), valueLength);
        }
        
        delete[] keys;
        delete[] values;
    }
}

// Бинарная загрузка хэш-таблицы
DoubleHashTable* loadHashTableBinary(ifstream& file) {
    DoubleHashTable* table = new DoubleHashTable();
    
    int32_t count;
    file.read(reinterpret_cast<char*>(&count), sizeof(count));
    
    for (int i = 0; i < count; i++) {
        int key;
        file.read(reinterpret_cast<char*>(&key), sizeof(key));
        
        int32_t valueLength;
        file.read(reinterpret_cast<char*>(&valueLength), sizeof(valueLength));
        string value(valueLength, ' ');
        file.read(&value[0], valueLength);
        
        table->insert(key, value);
    }
    
    return table;
}
// Бинарное сохранение в файл
void saveToFile(const string& filename) { 
    cout << "Сохранение в файл: " << filename << endl;
    ofstream file(filename, ios::binary);
    if (!file.is_open()) {
        cerr << "Ошибка открытия файла"<< endl;
        return;
    }
    
    // Сохраняем массивы [M]
    int32_t sectionM = 0x4D; // 'M'
    file.write(reinterpret_cast<const char*>(&sectionM), sizeof(sectionM));
    
    int32_t arrayCount = arrays.size();
    file.write(reinterpret_cast<const char*>(&arrayCount), sizeof(arrayCount));
    
    for (const auto& pair : arrays) {
        // Сохраняем имя массива
        int32_t nameLength = pair.first.length();
        file.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
        file.write(pair.first.c_str(), nameLength);
        
        // Сохраняем capacity и size
        int32_t capacity = pair.second.getCapacity();
        int32_t size = pair.second.getSize();
        file.write(reinterpret_cast<const char*>(&capacity), sizeof(capacity));
        file.write(reinterpret_cast<const char*>(&size), sizeof(size));
        
        // Сохраняем элементы
        for (int i = 0; i < size; i++) {
            string element = pair.second.getElementAt(i);
            int32_t elemLength = element.length();
            file.write(reinterpret_cast<const char*>(&elemLength), sizeof(elemLength));
            file.write(element.c_str(), elemLength);
        }
    }
    
    // Сохраняем очереди [Q]
    int32_t sectionQ = 0x51; // 'Q'
    file.write(reinterpret_cast<const char*>(&sectionQ), sizeof(sectionQ));
    
    int32_t queueCount = queues.size();
    file.write(reinterpret_cast<const char*>(&queueCount), sizeof(queueCount));
    
    for (const auto& pair : queues) {
        // Сохраняем имя очереди
        int32_t nameLength = pair.first.length();
        file.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
        file.write(pair.first.c_str(), nameLength);
        
        // Сохраняем элементы очереди
        vector<string> queueItems;
        Queue* current = pair.second;
        while (current != nullptr) {
            queueItems.push_back(current->getToy());
            current = current->getSite();
        }
        
        int32_t itemCount = queueItems.size();
        file.write(reinterpret_cast<const char*>(&itemCount), sizeof(itemCount));
        
        for (const auto& item : queueItems) {
            int32_t itemLength = item.length();
            file.write(reinterpret_cast<const char*>(&itemLength), sizeof(itemLength));
            file.write(item.c_str(), itemLength);
        }
    }
    
    // Сохраняем стеки [S]
    int32_t sectionS = 0x53; // 'S'
    file.write(reinterpret_cast<const char*>(&sectionS), sizeof(sectionS));
    
    int32_t stackCount = stacks.size();
    file.write(reinterpret_cast<const char*>(&stackCount), sizeof(stackCount));
    
    for (const auto& pair : stacks) {
        // Сохраняем имя стека
        int32_t nameLength = pair.first.length();
        file.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
        file.write(pair.first.c_str(), nameLength);
        
        // Сохраняем элементы стека (в порядке извлечения)
        vector<string> stackItems;
        Stack* current = pair.second;
        while (current != nullptr) {
            stackItems.push_back(current->getItem());
            current = current->getNext();
        }
        
        int32_t itemCount = stackItems.size();
        file.write(reinterpret_cast<const char*>(&itemCount), sizeof(itemCount));
        
        for (auto it = stackItems.rbegin(); it != stackItems.rend(); ++it) {
            int32_t itemLength = it->length();
            file.write(reinterpret_cast<const char*>(&itemLength), sizeof(itemLength));
            file.write(it->c_str(), itemLength);
        }
    }
    
    // Сохраняем односвязные списки [F]
    int32_t sectionF = 0x46; // 'F'
    file.write(reinterpret_cast<const char*>(&sectionF), sizeof(sectionF));
    
    int32_t listCount = lists.size();
    file.write(reinterpret_cast<const char*>(&listCount), sizeof(listCount));
    
    for (const auto& pair : lists) {
        // Сохраняем имя списка
        int32_t nameLength = pair.first.length();
        file.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
        file.write(pair.first.c_str(), nameLength);
        
        // Сохраняем элементы списка
        vector<string> listItems;
        OdSpis* current = pair.second;
        while (current != nullptr) {
            listItems.push_back(current->getName());
            current = current->getNext();
        }
        
        int32_t itemCount = listItems.size();
        file.write(reinterpret_cast<const char*>(&itemCount), sizeof(itemCount));
        
        for (const auto& item : listItems) {
            int32_t itemLength = item.length();
            file.write(reinterpret_cast<const char*>(&itemLength), sizeof(itemLength));
            file.write(item.c_str(), itemLength);
        }
    }
    
    // Сохраняем двусвязные списки [L]
    int32_t sectionL = 0x4C; // 'L'
    file.write(reinterpret_cast<const char*>(&sectionL), sizeof(sectionL));
    
    int32_t groupCount = groups.size();
    file.write(reinterpret_cast<const char*>(&groupCount), sizeof(groupCount));
    
    for (const auto& pair : groups) {
        // Сохраняем имя группы
        int32_t nameLength = pair.first.length();
        file.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
        file.write(pair.first.c_str(), nameLength);
        
        // Сохраняем элементы группы
        vector<string> groupItems;
        if (!pair.second->isEmpty()) {
            stringstream ss;
            auto old_cout_buffer = cout.rdbuf(ss.rdbuf());
            pair.second->printForward();
            cout.rdbuf(old_cout_buffer);
            string items = ss.str();
            
            istringstream iss(items);
            string item;
            while (iss >> item) {
                groupItems.push_back(item);
            }
        }
        
        int32_t itemCount = groupItems.size();
        file.write(reinterpret_cast<const char*>(&itemCount), sizeof(itemCount));
        
        for (const auto& item : groupItems) {
            int32_t itemLength = item.length();
            file.write(reinterpret_cast<const char*>(&itemLength), sizeof(itemLength));
            file.write(item.c_str(), itemLength);
        }
    }
    
    // Сохраняем деревья [T]
    int32_t sectionT = 0x54; // 'T'
    file.write(reinterpret_cast<const char*>(&sectionT), sizeof(sectionT));
    
    int32_t treeCount = trees.size();
    file.write(reinterpret_cast<const char*>(&treeCount), sizeof(treeCount));
    
    for (const auto& pair : trees) {
        // Сохраняем имя дерева
        int32_t nameLength = pair.first.length();
        file.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
        file.write(pair.first.c_str(), nameLength);
        
        // Сохраняем дерево в бинарном формате
        saveTreeBinary(pair.second, file);
    }
    // Сохраняем хэш-таблицы [H]
    int32_t sectionH = 0x48; // 'H'
    file.write(reinterpret_cast<const char*>(&sectionH), sizeof(sectionH));
    
    int32_t hashCount = hashTables.size();
    file.write(reinterpret_cast<const char*>(&hashCount), sizeof(hashCount));
    
    for (const auto& pair : hashTables) {
        // Сохраняем имя хэш-таблицы
        int32_t nameLength = pair.first.length();
        file.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
        file.write(pair.first.c_str(), nameLength);
        
        // Сохраняем хэш-таблицу в бинарном формате
        saveHashTableBinary(pair.second, file);
    }
    
    file.close();
    cout << "Файл успешно сохранен" << endl;
}

// Обновленная функция бинарной загрузки с поддержкой хэш-таблиц
void loadFromFile(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        cerr << "Ошибка: не удалось открыть файл " << filename << endl;
        return;
    }
    
    // Проверяем, не пустой ли файл
    file.seekg(0, ios::end);
    if (file.tellg() == 0) {
        cerr << "Файл пуст" << endl;
        file.close();
        return;
    }
    file.seekg(0, ios::beg);
    
    // Очищаем существующие структуры
    arrays.clear();
    
    for (auto& pair : queues) {
        if (pair.second != nullptr) {
            Queue::clearQueue(pair.second, queueTails[pair.first]);
        }
    }
    queues.clear();
    queueTails.clear();
    
    for (auto& pair : stacks) {
        if (pair.second != nullptr) {
            Stack::clearStack(pair.second);
        }
    }
    stacks.clear();
    
    for (auto& pair : lists) {
        if (pair.second != nullptr) {
            pair.second->cleanupList(pair.second);
        }
    }
    lists.clear();
    
    for (auto& pair : groups) {
        delete pair.second;
    }
    groups.clear();
    
    for (auto& pair : trees) {
        delete pair.second;
    }
    trees.clear();
    
    // Очищаем хэш-таблицы
    for (auto& pair : hashTables) {
        delete pair.second;
    }
    hashTables.clear();
    
    while (file.peek() != EOF) {
        int32_t sectionMarker;
        file.read(reinterpret_cast<char*>(&sectionMarker), sizeof(sectionMarker));
        
        if (file.eof()) break;
        
        switch (sectionMarker) {
            case 0x4D: { // 'M' - массивы
                int32_t arrayCount;
                file.read(reinterpret_cast<char*>(&arrayCount), sizeof(arrayCount));
                
                for (int i = 0; i < arrayCount; i++) {
                    // Читаем имя массива
                    int32_t nameLength;
                    file.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
                    string arrayName(nameLength, ' ');
                    file.read(&arrayName[0], nameLength);
                    
                    // Читаем capacity и size
                    int32_t capacity, size;
                    file.read(reinterpret_cast<char*>(&capacity), sizeof(capacity));
                    file.read(reinterpret_cast<char*>(&size), sizeof(size));
                    
                    Array newArray(capacity);
                    
                    // Читаем элементы
                    for (int j = 0; j < size; j++) {
                        int32_t elemLength;
                        file.read(reinterpret_cast<char*>(&elemLength), sizeof(elemLength));
                        string element(elemLength, ' ');
                        file.read(&element[0], elemLength);
                        newArray.addMasAtEnd(element);
                    }
                    
                    arrays[arrayName] = newArray;
                }
                break;
            }
            
            case 0x51: { // 'Q' - очереди
                int32_t queueCount;
                file.read(reinterpret_cast<char*>(&queueCount), sizeof(queueCount));
                
                for (int i = 0; i < queueCount; i++) {
                    // Читаем имя очереди
                    int32_t nameLength;
                    file.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
                    string queueName(nameLength, ' ');
                    file.read(&queueName[0], nameLength);
                    
                    queues[queueName] = nullptr;
                    queueTails[queueName] = nullptr;
                    
                    // Читаем элементы очереди
                    int32_t itemCount;
                    file.read(reinterpret_cast<char*>(&itemCount), sizeof(itemCount));
                    
                    for (int j = 0; j < itemCount; j++) {
                        int32_t itemLength;
                        file.read(reinterpret_cast<char*>(&itemLength), sizeof(itemLength));
                        string item(itemLength, ' ');
                        file.read(&item[0], itemLength);
                        Queue::QPUSH(queues[queueName], queueTails[queueName], item);
                    }
                }
                break;
            }
            
            case 0x53: { // 'S' - стеки
                int32_t stackCount;
                file.read(reinterpret_cast<char*>(&stackCount), sizeof(stackCount));
                
                for (int i = 0; i < stackCount; i++) {
                    // Читаем имя стека
                    int32_t nameLength;
                    file.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
                    string stackName(nameLength, ' ');
                    file.read(&stackName[0], nameLength);
                    
                    stacks[stackName] = nullptr;
                    
                    // Читаем элементы стека
                    int32_t itemCount;
                    file.read(reinterpret_cast<char*>(&itemCount), sizeof(itemCount));
                    
                    vector<string> items;
                    for (int j = 0; j < itemCount; j++) {
                        int32_t itemLength;
                        file.read(reinterpret_cast<char*>(&itemLength), sizeof(itemLength));
                        string item(itemLength, ' ');
                        file.read(&item[0], itemLength);
                        items.push_back(item);
                    }
                    
                    // Восстанавливаем стек в обратном порядке
                    for (auto it = items.rbegin(); it != items.rend(); ++it) {
                        Stack::SPUSH(stacks[stackName], *it);
                    }
                }
                break;
            }
            
            case 0x46: { // 'F' - односвязные списки
                int32_t listCount;
                file.read(reinterpret_cast<char*>(&listCount), sizeof(listCount));
                
                for (int i = 0; i < listCount; i++) {
                    // Читаем имя списка
                    int32_t nameLength;
                    file.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
                    string listName(nameLength, ' ');
                    file.read(&listName[0], nameLength);
                    
                    lists[listName] = nullptr;
                    
                    // Читаем элементы списка
                    int32_t itemCount;
                    file.read(reinterpret_cast<char*>(&itemCount), sizeof(itemCount));
                    
                    for (int j = 0; j < itemCount; j++) {
                        int32_t itemLength;
                        file.read(reinterpret_cast<char*>(&itemLength), sizeof(itemLength));
                        string item(itemLength, ' ');
                        file.read(&item[0], itemLength);
                        
                        if (lists[listName] == nullptr) {
                            lists[listName] = new OdSpis(item);
                        } else {
                            lists[listName]->AddToEnd(lists[listName], item);
                        }
                    }
                }
                break;
            }
            
            case 0x4C: { // 'L' - двусвязные списки
                int32_t groupCount;
                file.read(reinterpret_cast<char*>(&groupCount), sizeof(groupCount));
                
                for (int i = 0; i < groupCount; i++) {
                    // Читаем имя группы
                    int32_t nameLength;
                    file.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
                    string groupName(nameLength, ' ');
                    file.read(&groupName[0], nameLength);
                    
                    StudentGroup* newGroup = new StudentGroup();
                    
                    // Читаем элементы группы
                    int32_t itemCount;
                    file.read(reinterpret_cast<char*>(&itemCount), sizeof(itemCount));
                    
                    for (int j = 0; j < itemCount; j++) {
                        int32_t itemLength;
                        file.read(reinterpret_cast<char*>(&itemLength), sizeof(itemLength));
                        string item(itemLength, ' ');
                        file.read(&item[0], itemLength);
                        newGroup->addStudentToEnd(item);
                    }
                    
                    groups[groupName] = newGroup;
                }
                break;
            }
            case 0x54: { // 'T' - деревья
                int32_t treeCount;
                file.read(reinterpret_cast<char*>(&treeCount), sizeof(treeCount));
                for (int i = 0; i < treeCount; i++) {
                    int32_t nameLength;
                    file.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
                    string treeName(nameLength, ' ');
                    file.read(&treeName[0], nameLength);
                    Tree* newTree = loadTreeBinary(file);
                    trees[treeName] = newTree;
                }
                break;
            }
            case 0x48: { // 'H' - хэш-таблицы
                int32_t hashCount;
                file.read(reinterpret_cast<char*>(&hashCount), sizeof(hashCount));
                for (int i = 0; i < hashCount; i++) {
                    int32_t nameLength;
                    file.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
                    string tableName(nameLength, ' ');
                    file.read(&tableName[0], nameLength);
                    DoubleHashTable* newTable = loadHashTableBinary(file);
                    hashTables[tableName] = newTable;
                }
                break;
            }
            default:
                break;
        }
    }
    file.close();
}
string processListCommand(const string& command, istringstream& iss, const string& filename) {
    string groupName;
    iss >> groupName;
    if (groups.find(groupName) == groups.end() && 
        command != "LPUSHBEGIN" && command != "LPUSHEND") {
        return "GROUP_NOT_FOUND";
    }
    string result;
    if (command == "LPUSHBEGIN") {
        string element;
        iss >> element;
        result = LPUSHBEGIN(groupName, element);
    }
    else if (command == "LPUSHEND") {
        string element;
        iss >> element;
        result = LPUSHEND(groupName, element);
    }
    else if (command == "LPUSHBEFORE") {
        string target, element;
        iss >> target >> element;
        result = LPUSHBEFORE(groupName, target, element);
    }
    else if (command == "LPUSHAFTER") {
        string target, element;
        iss >> target >> element;
        result = LPUSHAFTER(groupName, target, element);
    }
    else if (command == "LPOPBEFORE") {
        string target;
        iss >> target;
        result = LPOPBEFORE(groupName, target);
    }
    else if (command == "LPOPAFTER") {
        string target;
        iss >> target;
        result = LPOPAFTER(groupName, target);
    }
    else if (command == "LPOPBEGIN") result = LPOPBEGIN(groupName);
    else if (command == "LPOPEND") result = LPOPEND(groupName);
    else if (command == "LPOISK") {
        string element;
        iss >> element;
        result = LPOISK(groupName, element);
    }
    else if (command == "LPOPVALUE") {
        string element;
        iss >> element;
        result = LPOPVALUE(groupName, element);
    }
    else if (command == "LGET") result = LGET(groupName);
    else if (command == "LGETREV") result = LGETREV(groupName);
    else return "Неизвестная команда";
    if (result != "Двусвязный список не найден" && result != "Элемент не найден" && result != "Ошибка" && !result.empty()) saveToFile(filename);
    return result;
}
// Функция обработки команд для хэш-таблиц
string processHashCommand(const string& command, istringstream& iss, const string& filename) {
    string tableName;
    iss >> tableName;
    string result;
    
    if (command == "HCREATE") {
        result = HCREATE(tableName);
    }
    else if (command == "HADD") {
        int key;
        string value;
        if (!(iss >> key >> value)) return "ОШИБКА";
        result = HADD(tableName, key, value);
    }
    else if (command == "HDEL") {
        int key;
        string value;
        if (!(iss >> key >> value)) return "ОШИБКА";
        result = HDEL(tableName, key, value);
    }
    else if (command == "HSEARCH") {
        int key;
        string value;
        if (!(iss >> key >> value)) return "ОШИБКА";
        result = HSEARCH(tableName, key, value);
    }
    else if (command == "HGET") {
        result = HGET(tableName);
    }
    else if (command == "HSIZE") {
        result = HSIZE(tableName);
    }
    else {
        return "Неизвестная команда";
    }
    
    // Сохранение изменений для модифицирующих команд
    if (command != "HGET" && command != "HSIZE" && command != "HSEARCH" && 
        result != "Хэш-таблица не найдена" && result != "Элемент не найден" && 
        result != "ОШИБКА" && !result.empty()) {
        saveToFile(filename);
    }
    
    return result;
}
string processArrayCommand(const string& command, istringstream& iss, const string& filename) {
    string arrayName;
    iss >> arrayName;
    string result;
    if (command == "MCREATE") {
        int capacity;
        if (!(iss >> capacity)) return "ОШИБКА";
        result = MCREATE(arrayName, capacity);
    }
    else if (command == "MPUSHIND") {
        int index;
        string value;
        if (!(iss >> index >> value)) return "ОШИБКА";
        result = MPUSHIND(arrayName, index, value);
    }
    else if (command == "MPUSHEND") {
        string value;
        if (!(iss >> value)) return "ОШИБКА";
        result = MPUSHEND(arrayName, value);
    }
    else if (command == "MGETIND") {
        int index;
        if (!(iss >> index)) return "ОШИБКА";
        result = MGETIND(arrayName, index);
    }
    else if (command == "MDELIND") {
        int index;
        if (!(iss >> index)) return "ОШИБКА";
        result = MDELIND(arrayName, index);
    }
    else if (command == "MREPLACE") {
        int index;
        string value;
        if (!(iss >> index >> value)) return "ОШИБКА";
        result = MREPLACE(arrayName, index, value);
    }
    else if (command == "MSIZE") {
        result = MSIZE(arrayName);
    }
    else if (command == "MGET") {
        result = MGET(arrayName);
    }
    else {
        return "Неизвестная команда";
    }
    if (result != "Массив не найден" && result != "Индекс вышел за границы" && result != "Индекс за границами" &&
        result != "Массив переполнен" &&result != "ОШИБКА") {
        saveToFile(filename);
    }
    return result;
}
string processFListCommand(const string& command, istringstream& iss, const string& filename) {
    string listName;
    iss >> listName;
    string result;
    if (command == "FPUSHBEFORE" || command == "FPUSHAFTER") {
        string target, element;
        iss >> target >> element;
        result = (command == "FPUSHBEFORE") ? FPUSHBEFORE(listName, target, element): FPUSHAFTER(listName, target, element);
    }
    else if (command == "FPUSHEND" || command == "FPUSHBEGIN") {
        string element;
        iss >> element;
        result = (command == "FPUSHEND") ? FPUSHEND(listName, element): FPUSHBEGIN(listName, element);
    }
    else if (command == "FPOISK") {
        string element;
        iss >> element;
        result = FPOISK(listName, element);
    }
    else if (command == "FDELVALUE") {
        string element;
        iss >> element;
        result = FDELVALUE(listName, element);
    }
    else if (command == "FDELBEGIN") result = FDELBEGIN(listName);
    else if (command == "FDELEND") result = FDELEND(listName);
    else if (command == "FDELBEFORE") {
        string target;
        iss >> target;
        result = FDELBEFORE(listName, target);
    }
    else if (command == "FDELAFTER") {
        string target;
        iss >> target;
        result = FDELAFTER(listName, target);
    }
    else if (command == "FGET") return FGET(listName);
    else return "Неизвестная команда";
    if (result != "Список не найден" && result != "Элемент не найден"  && result != "Список пуст") saveToFile(filename);
    return result;
}
string processStackQueueCommand(const string& command, istringstream& iss, const string& filename) {
    string structureName;
    iss >> structureName;
    string result;
    
    if (command == "SPUSH" || command == "QPUSH") {
        string item;
        getline(iss, item);
        if (!item.empty() && item[0] == ' ') item = item.substr(1);
        
        if (command == "SPUSH") {
            if (stacks.find(structureName) == stacks.end()) stacks[structureName] = nullptr;
            Stack::SPUSH(stacks[structureName], item);
        } else {
            if (queues.find(structureName) == queues.end()) {
                queues[structureName] = nullptr;
                queueTails[structureName] = nullptr;
            }
            Queue::QPUSH(queues[structureName], queueTails[structureName], item);
        }
        result = item;
    }
    else if (command == "SPOP") {
        if (stacks.find(structureName) == stacks.end() || stacks[structureName] == nullptr) 
            return "Стек пустой";
        result = Stack::SPOP(stacks[structureName]);
    }
    else if (command == "QPOP") {
        if (queues.find(structureName) == queues.end() || queues[structureName] == nullptr) 
            return "Очередь пуста";
        result = Queue::QPOP(queues[structureName], queueTails[structureName]);
    }
    saveToFile(filename);
    return result;
}
string processTreeCommand(const string& command, istringstream& iss, const string& filename) {
    string treeName;
    iss >> treeName;
    string result;
    if (command == "TADD") {
        int value;
        if (!(iss >> value)) return "ОШИБКА";
        result = TADD(treeName, value);
    }
    else if (command == "TDEL") {
        int value;
        if (!(iss >> value)) return "ОШИБКА";
        result = TDEL(treeName, value);
    }
    else if (command == "TSEARCH") {
        int value;
        if (!(iss >> value)) return "ОШИБКА";
        result = TSEARCH(treeName, value);
    }
    else if (command == "TDELETE") {
        int value;
        if (!(iss >> value)) return "ОШИБКА";
        result = TDELETE(treeName, value);
    }
    else if (command == "TGET") {
        result = TGET(treeName);
    }
    else if (command == "TGETSYM") result = TGETSYM(treeName);
    else if (command == "TREAD") result = TREAD(treeName);
    else return "Неизвестная команда";
    if (command != "TREAD" && command != "TGET" && command != "TGETSYM" && command != "TSEARCH" &&result != "Дерево не найдено" && 
        result != "Элемент не найден или невозможно удалить" &&result != "ОШИБКА" &&!result.empty()) {
        saveToFile(filename);
    }
    return result;
}
string processQuery(const string& query, const string& filename) {
    istringstream iss(query);
    string com;
    iss >> com;
    if (com.find('L') == 0) return processListCommand(com, iss, filename);
    else if (com.find('M') == 0) return processArrayCommand(com, iss, filename);
    else if (com.find('F') == 0) return processFListCommand(com, iss, filename);
    else if (com == "SPUSH" || com == "SPOP" || com == "QPUSH" || com == "QPOP") return processStackQueueCommand(com, iss, filename);
    else if (com.find('T') == 0) return processTreeCommand(com, iss, filename);
    return "Неизвестная команда";
}

