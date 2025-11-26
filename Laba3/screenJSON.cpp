#include "screen.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>
#include "json.hpp"

using json = nlohmann::json;

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
void treeToJson(Tree* node, json& j) {
    if (node == nullptr) {
        j = nullptr;
        return;
    }
    
    j["value"] = node->getValue();
    j["color"] = (node->getColor() == Tree::RED) ? "r" : "b";
    
    if (node->getLeft() != nullptr) {
        json leftJson;
        treeToJson(node->getLeft(), leftJson);
        j["left"] = leftJson;
    } else {
        j["left"] = nullptr;
    }
    
    if (node->getRight() != nullptr) {
        json rightJson;
        treeToJson(node->getRight(), rightJson);
        j["right"] = rightJson;
    } else {
        j["right"] = nullptr;
    }
}

Tree* treeFromJson(const json& j) {
    if (j.is_null()) return nullptr;
    
    int value = j["value"];
    Tree* node = new Tree(value);
    
    // Если в классе Tree есть метод установки цвета, используем его
    // Иначе предполагаем, что цвет устанавливается при создании или не нужен
    string color = j["color"];
    // Если в вашем классе Tree нет setColor, возможно цвет определяется автоматически
    // или это обычное бинарное дерево, а не красно-черное
    
    // Рекурсивно создаем левое и правое поддеревья
    if (j.contains("left") && !j["left"].is_null()) {
        Tree* leftChild = treeFromJson(j["left"]);
        // В зависимости от реализации вашего класса Tree:
        // Если есть метод setLeft, используем его, иначе добавляем через существующие методы
        node->AddLeaf(node, leftChild->getValue()); // Или другой подходящий метод
        delete leftChild; // Удаляем временный узел, т.к. AddLeaf создаст новый
    }
    
    if (j.contains("right") && !j["right"].is_null()) {
        Tree* rightChild = treeFromJson(j["right"]);
        node->AddLeaf(node, rightChild->getValue()); // Или другой подходящий метод
        delete rightChild;
    }
    
    return node;
}
void hashTableToJson(DoubleHashTable* table, json& j) {
    if (table == nullptr) {
        j = json::array();
        return;
    }
    
    int count = table->elementCount();
    if (count == 0) {
        j = json::array();
        return;
    }
    
    int* keys = new int[count];
    string* values = new string[count];
    int actualCount;
    table->getAllElements(keys, values, actualCount);
    
    j = json::array();
    for (int i = 0; i < actualCount; i++) {
        json entry;
        entry["key"] = keys[i];
        entry["value"] = values[i];
        j.push_back(entry);
    }
    
    delete[] keys;
    delete[] values;
}

DoubleHashTable* hashTableFromJson(const json& j) {
    DoubleHashTable* table = new DoubleHashTable();
    
    if (j.is_array()) {
        for (const auto& entry : j) {
            int key = entry["key"];
            string value = entry["value"];
            table->insert(key, value);
        }
    }
    
    return table;
}

// Обновленная функция JSON сохранения с поддержкой хэш-таблиц
void saveToFile(const string& filename) {
    cout << "Сохранение в файл: " << filename << endl;
    
    json data;
    
    // Сохраняем массивы [M]
    data["arrays"] = json::object();
    for (const auto& pair : arrays) {
        json arrayJson;
        arrayJson["capacity"] = pair.second.getCapacity();
        arrayJson["size"] = pair.second.getSize();
        
        json elements = json::array();
        for (int i = 0; i < pair.second.getSize(); i++) {
            elements.push_back(pair.second.getElementAt(i));
        }
        arrayJson["elements"] = elements;
        
        data["arrays"][pair.first] = arrayJson;
    }
    
    // Сохраняем очереди [Q]
    data["queues"] = json::object();
    for (const auto& pair : queues) {
        json queueJson = json::array();
        Queue* current = pair.second;
        while (current != nullptr) {
            queueJson.push_back(current->getToy());
            current = current->getSite();
        }
        data["queues"][pair.first] = queueJson;
    }
    
    // Сохраняем стеки [S]
    data["stacks"] = json::object();
    for (const auto& pair : stacks) {
        json stackJson = json::array();
        Stack* current = pair.second;
        while (current != nullptr) {
            stackJson.push_back(current->getItem());
            current = current->getNext();
        }
        data["stacks"][pair.first] = stackJson;
    }
    
    // Сохраняем односвязные списки [F]
    data["single_lists"] = json::object();
    for (const auto& pair : lists) {
        json listJson = json::array();
        OdSpis* current = pair.second;
        while (current != nullptr) {
            listJson.push_back(current->getName());
            current = current->getNext();
        }
        data["single_lists"][pair.first] = listJson;
    }
    
    // Сохраняем двусвязные списки [L]
    data["double_lists"] = json::object();
    for (const auto& pair : groups) {
        json groupJson = json::array();
        if (!pair.second->isEmpty()) {
            stringstream ss;
            auto old_cout_buffer = cout.rdbuf(ss.rdbuf());
            pair.second->printForward();
            cout.rdbuf(old_cout_buffer);
            string items = ss.str();
            
            istringstream iss(items);
            string item;
            while (iss >> item) {
                groupJson.push_back(item);
            }
        }
        data["double_lists"][pair.first] = groupJson;
    }
    
    // Сохраняем деревья [T]
    data["trees"] = json::object();
    for (const auto& pair : trees) {
        json treeJson;
        treeToJson(pair.second, treeJson);
        data["trees"][pair.first] = treeJson;
    }
    
    // Сохраняем хэш-таблицы [H]
    data["hash_tables"] = json::object();
    for (const auto& pair : hashTables) {
        json hashJson;
        hashTableToJson(pair.second, hashJson);
        data["hash_tables"][pair.first] = hashJson;
    }
    
    // Записываем JSON в файл
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Ошибка открытия файла" << endl;
        return;
    }
    
    file << data.dump(4); // dump(4) для красивого форматирования с отступами
    file.close();
    cout << "Файл успешно сохранен" << endl;
}
// Обновленная функция JSON загрузки с поддержкой хэш-таблиц
void loadFromFile(const string& filename) {
    ifstream file(filename);
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
    
    json data;
    try {
        file >> data;
    } catch (const json::parse_error& e) {
        cerr << "Ошибка парсинга JSON: " << e.what() << endl;
        file.close();
        return;
    }
    file.close();
    
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
    
    // Загружаем массивы [M]
    if (data.contains("arrays")) {
        for (auto& [name, arrayData] : data["arrays"].items()) {
            int capacity = arrayData["capacity"];
            int size = arrayData["size"];
            
            Array newArray(capacity);
            for (const auto& element : arrayData["elements"]) {
                newArray.addMasAtEnd(element);
            }
            
            arrays[name] = newArray;
        }
    }
    
    // Загружаем очереди [Q]
    if (data.contains("queues")) {
        for (auto& [name, queueData] : data["queues"].items()) {
            queues[name] = nullptr;
            queueTails[name] = nullptr;
            
            for (const auto& item : queueData) {
                Queue::QPUSH(queues[name], queueTails[name], item);
            }
        }
    }
    
    // Загружаем стеки [S]
    if (data.contains("stacks")) {
        for (auto& [name, stackData] : data["stacks"].items()) {
            stacks[name] = nullptr;
            
            // Восстанавливаем стек в обратном порядке
            for (auto it = stackData.rbegin(); it != stackData.rend(); ++it) {
                Stack::SPUSH(stacks[name], *it);
            }
        }
    }
    
    // Загружаем односвязные списки [F]
    if (data.contains("single_lists")) {
        for (auto& [name, listData] : data["single_lists"].items()) {
            lists[name] = nullptr;
            
            for (const auto& item : listData) {
                if (lists[name] == nullptr) {
                    lists[name] = new OdSpis(item);
                } else {
                    lists[name]->AddToEnd(lists[name], item);
                }
            }
        }
    }
    
    // Загружаем двусвязные списки [L]
    if (data.contains("double_lists")) {
        for (auto& [name, groupData] : data["double_lists"].items()) {
            StudentGroup* newGroup = new StudentGroup();
            
            for (const auto& item : groupData) {
                newGroup->addStudentToEnd(item);
            }
            
            groups[name] = newGroup;
        }
    }
    
    // Загружаем деревья [T]
    if (data.contains("trees")) {
        for (auto& [name, treeData] : data["trees"].items()) {
            Tree* newTree = treeFromJson(treeData);
            trees[name] = newTree;
        }
    }
    
    // Загружаем хэш-таблицы [H]
    if (data.contains("hash_tables")) {
        for (auto& [name, hashData] : data["hash_tables"].items()) {
            DoubleHashTable* newTable = hashTableFromJson(hashData);
            hashTables[name] = newTable;
        }
    }
    
    cout << "Данные успешно загружены из файла" << endl;
}
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
