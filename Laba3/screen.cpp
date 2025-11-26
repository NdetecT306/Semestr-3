#include "screen.h"
map<string, Queue*> queues;
map<string, Stack*> stacks;
map<string, OdSpis*> lists;
map<string, Array> arrays;
map<string, StudentGroup*> groups; 
map<string, Queue*> queueTails;
map<string, Tree*> trees;
map<string, DoubleHashTable*> hashTables; 
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
void saveTreeLevel(Tree* root, int level, ofstream& file) {
    if (root == nullptr) return;
    if (level == 1) {
        file << " " << root->getValue() << (root->getColor() == Tree::RED ? "r" : "b");
    } 
    else if (level > 1) {
        saveTreeLevel(root->getLeft(), level - 1, file);
        saveTreeLevel(root->getRight(), level - 1, file);
    }
}

void saveTreeBFS(Tree* root, ofstream& file) {
    if (root == nullptr) return;
    int height = root->getHeight(root);
    for (int level = 1; level <= height; level++) {
        saveTreeLevel(root, level, file);
    }
}
void saveToFile(const string& filename) { 
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Ошибка открытия файла"<< endl;
        return;
    }
    file << "[M]" << endl;
    for (const auto& pair : arrays) {
        file << pair.first << " " << pair.second.getCapacity() << " " << pair.second.getSize();
        for (int i = 0; i < pair.second.getSize(); i++) {
            file << " " << pair.second.getElementAt(i);
        }
        file << endl;
    }
    file << "\n[Q]" << endl;
    for (const auto& pair : queues) {
        file << pair.first;
        Queue* current = pair.second;
        while (current != nullptr) {
            file << " " << current->getToy();
            current = current->getSite();
        }
        file << endl;
    }
    
    file << "\n[S]" << endl;
    for (const auto& pair : stacks) {
        file << pair.first;
        // Для стека сохраняем в порядке извлечения (сверху вниз)
        vector<string> stackItems;
        Stack* current = pair.second;
        while (current != nullptr) {
            stackItems.push_back(current->getItem());
            current = current->getNext();
        }
        // Сохраняем в обратном порядке для корректного восстановления
        for (auto it = stackItems.rbegin(); it != stackItems.rend(); ++it) {
            file << " " << *it;
        }
        file << endl;
    }
    file << "\n[F]" << endl;
    for (const auto& pair : lists) {
        file << pair.first;
        OdSpis* current = pair.second;
        while (current != nullptr) {
            file << " " << current->getName();
            current = current->getNext();
        }
        file << endl;
    }
    file << "\n[L]" << endl;
    for (const auto& pair : groups) {
        file << pair.first;
        // Временный вывод для сохранения состояния
        stringstream ss;
        auto old_cout_buffer = cout.rdbuf(ss.rdbuf());
        pair.second->printForward();
        cout.rdbuf(old_cout_buffer);
        string items = ss.str();
        if (!items.empty() && items.back() == '\n') items.pop_back();
        file << items << endl;
    }
    file << "\n[T]" << endl;
    for (const auto& pair : trees) {
        file << pair.first;
        saveTreeBFS(pair.second, file);
        file << endl;
    }
    // Сохранение хэш-таблиц
    file << "\n[H]" << endl;
    for (const auto& pair : hashTables) {
        file << pair.first;
        int count = pair.second->elementCount();
        if (count > 0) {
            int* keys = new int[count];
            string* values = new string[count];
            int actualCount;
            pair.second->getAllElements(keys, values, actualCount);
            
            for (int i = 0; i < actualCount; i++) {
                file << " " << keys[i] << ":" << values[i];
            }
            
            delete[] keys;
            delete[] values;
        }
        file << endl;
    }
    file.close();
}
void loadFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        return;
    }
    string line;
    string section;
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
    while (getline(file, line)) {
        if (line.empty()) continue;
        if (line == "[M]") {
            section = "M";
            continue;
        } else if (line == "[Q]") {
            section = "Q";
            continue;
        } else if (line == "[S]") {
            section = "S";
            continue;
        } else if (line == "[F]") { 
            section = "F";
            continue;
        } else if (line == "[L]") {
            section = "L";
            continue;
        }
        else if (line == "[T]") {
            section = "T";
            continue;
        }
        else if (section == "M") {
            istringstream iss(line);
            string arrayName;
            int C, size;
            if (iss >> arrayName >> C >> size) {
                Array newArray(C);  // Создаем объект напрямую
                string value;
                int index = 0;
                while (iss >> value && index < size) {
                    newArray.addMasAtEnd(value);
                    index++;
                }
                arrays[arrayName] = newArray;  // Сохраняем объект в map
            }
        }
        else if (section == "Q") {
            istringstream iss(line);
            string queueName;
            if (iss >> queueName) {
                queues[queueName] = nullptr;
                queueTails[queueName] = nullptr;
                string item;
                while (iss >> item) {
                    Queue::QPUSH(queues[queueName], queueTails[queueName], item);
                }
            }
        }
        else if (section == "S") {
            istringstream iss(line);
            string stackName;
            if (iss >> stackName) {
                stacks[stackName] = nullptr;
                string item;
                vector<string> items;
                while (iss >> item) {
                    items.push_back(item);
                }
                // Загружаем в обратном порядке для корректного восстановления стека
                for (auto it = items.rbegin(); it != items.rend(); ++it) {
                    Stack::SPUSH(stacks[stackName], *it);
                }
            }
        }

        else if (section == "L") {
            istringstream iss(line);
            string groupName;
            if (iss >> groupName) {
                StudentGroup* newGroup = new StudentGroup();
                string element;
                while (iss >> element) {
                    if (newGroup->isEmpty()) {
                        newGroup->addStudentToEnd(element);
                    } else {
                        newGroup->addStudentToEnd(element);
                    }
                }
                groups[groupName] = newGroup;
            }
        }
        else if (section == "F") {
            istringstream iss(line);
            string listName;
            if (iss >> listName) {
                lists[listName] = nullptr;
                string element;
                while (iss >> element) {
                    if (lists[listName] == nullptr) {
                        lists[listName] = new OdSpis(element);
                    } else {
                        lists[listName]->AddToEnd(lists[listName], element);
                    }
                }
            }
        }
        else if (section == "H") {
            // Загрузка хэш-таблиц
            istringstream iss(line);
            string tableName;
            if (iss >> tableName) {
                DoubleHashTable* newTable = new DoubleHashTable();
                string keyValuePair;
                while (iss >> keyValuePair) {
                    size_t colonPos = keyValuePair.find(':');
                    if (colonPos != string::npos) {
                        string keyStr = keyValuePair.substr(0, colonPos);
                        string value = keyValuePair.substr(colonPos + 1);
                        try {
                            int key = stoi(keyStr);
                            newTable->insert(key, value);
                        } catch (const exception& e) {
                            // Пропускаем некорректные записи
                        }
                    }
                }
                hashTables[tableName] = newTable;
            }
        }
        else if (section == "T") {
            istringstream iss(line);
            string treeName;
            if (iss >> treeName) {
                Tree* newTree = nullptr;
                string valueWithColor;
                while (iss >> valueWithColor) {
                    string valueStr;
                    char colorChar;
                    for (char c : valueWithColor) {
                        if (isdigit(c) || c == '-') {
                            valueStr += c;
                        } 
                        else {
                            colorChar = c;
                            break;
                        }
                    }   
                    if (!valueStr.empty()) {
                        int value = stoi(valueStr);
                        if (newTree == nullptr) {
                            newTree = Tree::CreateRoot(value);
                            // Установка цвета для корня
                            if (colorChar == 'r') {
                                // Корень должен быть черным, игнорируем красный цвет
                            }
                        } 
                        else {
                            newTree->AddLeaf(newTree, value);
                        }
                    }
                }
                trees[treeName] = newTree;
            }
        }
    }
    file.close();
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
