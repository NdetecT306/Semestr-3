#include "head.h"
map<string, Toys*> queues;
map<string, Garden*> stacks;
map<string, Spisok*> lists;
map<string, Numbers> arrays;
map<string, GroupPoint*> groups; 
map<string, Toys*> queueTails;
map<string, Tree*> trees;
string TADD(const string& treeName, int value) {
    if (trees.find(treeName) == trees.end()) {
        trees[treeName] = CreateRoot(value);
        return to_string(value);
    } else {
        AddLeaf(trees[treeName], value);
        return to_string(value);
    }
}
string TSEARCH(const string& treeName, int value) {
    if (trees.find(treeName) == trees.end()) {
        return "Дерево не найдено";
    }
    bool found = ifZnachTreeExist(trees[treeName], value);
    return found ? "Найден" : "Не найден";
}
string TDELETE(const string& treeName, int value) {
    if (trees.find(treeName) == trees.end()) {
        return "Дерево не найдено";
    }
    bool success = deleteNode(trees[treeName], value);
    return success ? "Элемент удален" : "Элемент не найден или невозможно удалить";
}
string TGET(const string& treeName) {
    if (trees.find(treeName) == trees.end()) {
        return "Дерево не найдено";
    }
    if (trees[treeName] == nullptr) {
        return "Дерево пустое";
    }
    
    stringstream output;
    auto old_cout_buffer = cout.rdbuf(output.rdbuf());
    BFS(trees[treeName]); // Обход в ширину для сохранения в файл
    cout.rdbuf(old_cout_buffer);
    
    string result = output.str();
    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }
    return result;
}

string TGETSYM(const string& treeName) {
    if (trees.find(treeName) == trees.end()) {
        return "Дерево не найдено";
    }
    if (trees[treeName] == nullptr) {
        return "Дерево пустое";
    }
    
    stringstream output;
    auto old_cout_buffer = cout.rdbuf(output.rdbuf());
    symmetrical(trees[treeName]); // Симметричный обход
    cout.rdbuf(old_cout_buffer);
    
    string result = output.str();
    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }
    return result;
}
string TDEL(const string& treeName, int value) {
    if (trees.find(treeName) == trees.end()) {
        return "Дерево не найдено";
    } else {
        bool success = deleteNode(trees[treeName], value);
        return success ? to_string(value) : "Элемент не найден или невозможно удалить";
    }
}
string LPOISK(const string& groupName, const string& str) {
    if (groups.find(groupName) == groups.end()) {
        return "Двусвязный список не найден";
    }
    bool found = poiskGroup(groups[groupName]->head, str);
    return found ? "Найден" : "Не найден";
}
string LPOPVALUE(const string& groupName, const string& str) {
    if (groups.find(groupName) == groups.end()) {
        return "Двусвязный список не найден";
    }
    if (!poiskGroup(groups[groupName]->head, str)) {
        return "Элемент в двусвязном списке не найден";
    }
    deleteGroupPoZnach(groups[groupName]->head, groups[groupName]->tail, str);
    return "Элемент удален";
}
string LPUSHBEGIN(const string& groupName, const string& str) {
    if (groups.find(groupName) == groups.end()) {
        // Создаем новый список если не существует
        GroupPoint* newGroup = new GroupPoint;
        CreateGroup(newGroup->head, newGroup->tail, str);
        groups[groupName] = newGroup;
    } else {
        AddStudToStart(groups[groupName]->head, groups[groupName]->tail, str);
    }
    return str;
}
string LPUSHEND(const string& groupName, const string& str) {
    if (groups.find(groupName) == groups.end()) {
        GroupPoint* newGroup = new GroupPoint;
        CreateGroup(newGroup->head, newGroup->tail, str);
        groups[groupName] = newGroup;
    } else {
        AddStudToEnd(groups[groupName]->head, groups[groupName]->tail, str);
    }
    return str;
}
string LPUSHBEFORE(const string& groupName, const string& targetElement, const string& str) {
    if (groups.find(groupName) == groups.end()) {
        return "Двусвязный список не найден";
    }
    Group* element = groups[groupName]->head;
    while (element != nullptr && element->student != targetElement) {
        element = element->next;
    }
    if (element == nullptr) {
        return "Элемент в двусвязном списке не найден";
    }
    AddStudDo(element, groups[groupName]->tail, str);
    if (element == groups[groupName]->head) {
        groups[groupName]->head = element->prev;
    }
    return str;
}
string LPUSHAFTER(const string& groupName, const string& pointerElement, const string& str) {
    if (groups.find(groupName) == groups.end()) {
        return "Двусвязный список не найден";
    }
    Group* element = groups[groupName]->head;
    while (element != nullptr && element->student != pointerElement) {
        element = element->next;
    }
    if (element == nullptr) {
        return "Элемент в двусвязном списке не найден";
    }
    AddStudPosle(element, groups[groupName]->tail, str);
    return str;
}
string LPOPBEFORE(const string& groupName, const string& str) {
    if (groups.find(groupName) == groups.end()) {
        return "Двусвязный список не найден";
    }
    Group* element = groups[groupName]->head;
    while (element != nullptr && element->student != str) {
        element = element->next;
    }
    if (element == nullptr) {
        return "Элемент в двусвязном списке не найден";
    }
    if (element->prev == nullptr) {
        return "Выше головы не прыгнешь, рубить нечего";
    }
    Group* delem = element->prev;
    if (delem == groups[groupName]->head) {
        groups[groupName]->head = element;
        element->prev = nullptr;
    } else {
        delem->prev->next = element;
        element->prev = delem->prev;
    }
    if (delem == groups[groupName]->tail) {
        groups[groupName]->tail = element->prev;
    }
    delete delem;
    return "Элемент удален";
}
string LPOPAFTER(const string& groupName, const string& str) {
    if (groups.find(groupName) == groups.end()) {
        return "Двусвязный список не найден";
    }
    Group* element = groups[groupName]->head;
    while (element != nullptr && element->student != str) {
        element = element->next;
    }
    if (element == nullptr) {
        return "Элемент в двусвязном списке не найден";
    }
    if (element->next == nullptr) {
        return "За последним элементом ничего нет, удалять нечего";
    }
    deleteStudPosle(element, groups[groupName]->tail, str);
    return "Элемент удален";
}
string LPOPBEGIN(const string& groupName) {
    if (groups.find(groupName) == groups.end()) {
        return "Двусвязный список не найден";
    }
    if (groups[groupName]->head == nullptr) {
        return "Двусвязный список пустой";
    }
    DeleteStudFromStart(groups[groupName]->head, groups[groupName]->tail);
    return "Элемент удален";
}
string LPOPEND(const string& groupName) {
    if (groups.find(groupName) == groups.end()) {
        return "Двусвязный список не найден";
    }
    if (groups[groupName]->head == nullptr) {
        return "Двусвязный список пустой";
    }
    DeleteStudFromEnd(groups[groupName]->head, groups[groupName]->tail);
    return "Элемент удален";
}
string LGET(const string& groupName) {
    if (groups.find(groupName) == groups.end()) {
        return "Двусвязный список не найден";
    }
    if (groups[groupName]->head == nullptr) {
        return "Двусвязный список пустой";
    }
    stringstream output;
    auto old_cout_buffer = cout.rdbuf(output.rdbuf());
    printGroup(groups[groupName]->head);
    cout.rdbuf(old_cout_buffer);
    string result = output.str();
    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }
    return result;
}
string LGETREV(const string& groupName) {
    if (groups.find(groupName) == groups.end()) {
        return "Двусвязный список не найден";
    }
    if (groups[groupName]->head == nullptr) {
        return "Двусвязный список пустой";
    }
    stringstream output;
    auto old_cout_buffer = cout.rdbuf(output.rdbuf());
    otherPrintGroup(groups[groupName]->head);
    cout.rdbuf(old_cout_buffer);
    string result = output.str();
    if (!result.empty() && result.back() == ' ') {
        result.pop_back();
    }
    return result;
}
bool findInGroup(Group* ptr, const string& str) {
    Group* element = ptr;
    while (element != nullptr) {
        if (element->student == str) {
            return true;
        }
        element = element->next;
    }
    return false;
}
int countGroup(Group* ptr) {
    int count = 0;
    Group* element = ptr;
    while (element != nullptr) {
        count++;
        element = element->next;
    }
    return count;
}
void deleteGroup(Group*& head, Group*& tail) {
    Group* element = head;
    while (element != nullptr) {
        Group* temp = element;
        element = element->next;
        delete temp;
    }
    head = nullptr;
    tail = nullptr;
}
string MPUSHIND(const string& A, int index, string value) {
    if (arrays.find(A) == arrays.end()) {
        return "Массив не найден";
    }
    if (index < 0 || index >= arrays[A].C) {
        return "Индекс вышел за границы.";
    }
    if (index >= arrays[A].size) {
        arrays[A].size = index + 1;
    }
    arrays[A].num[index] = value;
    return value;
}
string MPUSHEND(const string& A, string value) {
    if (arrays.find(A) == arrays.end()) {
        return "Массив не найден";
    }
    if (arrays[A].size >= arrays[A].C) {
        return "Массив переполнен";
    }
    arrays[A].num[arrays[A].size] = value;
    arrays[A].size++;
    return value;
}
string MCREATE(const string& A, int C) {
    if (arrays.find(A) != arrays.end()) {
        delete[] arrays[A].num;
    }
    Numbers newArray;
    newArray.num = new string[C];
    newArray.size = 0;
    newArray.C = C;
    arrays[A] = newArray;
    return to_string(C);
}
string MGETIND(const string& A, int index) {
    if (arrays.find(A) == arrays.end()) {
        return "Массив не найден";
    }
    if (index < 0 || index >= arrays[A].size) {
        return "Индекс вышел за границы";
    }
    return arrays[A].num[index];
}
string MDELIND(const string& A, int index) {
    if (arrays.find(A) == arrays.end()) {
        return "Массив не найден";
    }
    if (index < 0 || index >= arrays[A].size) {
        return "Индекс вышел за границы";
    }
    for (int i = index; i < arrays[A].size - 1; i++) {
        arrays[A].num[i] = arrays[A].num[i + 1];
    }
    arrays[A].size--;
    return "Элемент удален";
}
string MREPLACE(const string& A, int index, string value) {
    if (arrays.find(A) == arrays.end()) {
        return "Массив не найден";
    }
    if (index < 0 || index >= arrays[A].size) {
        return "Индекс за границами";
    }
    arrays[A].num[index] = value;
    return value;
}
string MSIZE(const string& A) {
    if (arrays.find(A) == arrays.end()) {
        return "Массив не найден";
    }
    return to_string(arrays[A].size);
}
string MGET(const string& A) {
    if (arrays.find(A) == arrays.end()) {
        return "Массив не найден";
    }
    if (arrays[A].size == 0) {
        return "Массив пуст";
    }
    string result = "";
    for (int i = 0; i < arrays[A].size; i++) {
        if (i > 0) result += " ";
        result += arrays[A].num[i];
    }
    return result;
}
string FPUSHBEFORE(const string& listName, const string& targetElement, const string& newElement) {
    if (lists.find(listName) == lists.end()) {
        return "Список не найден";
    }
    Spisok* target = FindElement(lists[listName], targetElement);
    if (target == nullptr) {
        return "Элемент не найден";
    }
    AddChelDo(lists[listName], target, newElement);
    return newElement;
}
string FPUSHEND(const string& listName, const string& newElement) {
    if (lists.find(listName) == lists.end()) {
        CreateList(lists[listName], newElement);
        return newElement;
    }
    AddToEnd(lists[listName], newElement);
    return newElement;
}
string FPUSHAFTER(const string& listName, const string& targetElement, const string& newElement) {
    if (lists.find(listName) == lists.end()) {
        return "Список не найден";
    }
    Spisok* target = FindElement(lists[listName], targetElement);
    if (target == nullptr) {
        return "Элемент не найден";
    }
    AddChelPosle(target, newElement);
    return newElement;
}
string FPUSHBEGIN(const string& listName, const string& newElement) {
    if (lists.find(listName) == lists.end()) {
        CreateList(lists[listName], newElement);
        return newElement;
    }
    AddToBegin(lists[listName], newElement);
    return newElement;
}
string FPOISK(const string& listName, const string& searchElement) {
    if (lists.find(listName) == lists.end()) {
        return "Список не найден";
    }
    bool found = poisk(lists[listName], searchElement);
    return found ? "Найден" : "НЕ найден";
}
string FDELVALUE(const string& listName, const string& elementToDelete) {
    if (lists.find(listName) == lists.end()) {
        return "Списорк не найден";
    }
    if (!poisk(lists[listName], elementToDelete)) {
        return "Элемент не найден";
    }
    deletePoZnach(lists[listName], elementToDelete);
    return "Удален";
}
string FDELBEGIN(const string& listName) {
    if (lists.find(listName) == lists.end()) {
        return "Список не найден";
    }
    if (lists[listName] == nullptr) {
        return "Список пуст";
    }
    deleteToBegin(lists[listName]);
    return "Удален";
}
string FDELEND(const string& listName) {
    if (lists.find(listName) == lists.end()) {
        return "Список не найден";
    }
    if (lists[listName] == nullptr) {
        return "Список пуст";
    }
    deleteToEnd(lists[listName]);
    return "Удален";
}
string FDELBEFORE(const string& listName, const string& targetElement) {
    if (lists.find(listName) == lists.end()) {
        return "Список не найден";
    }
    Spisok* target = FindElement(lists[listName], targetElement);
    if (target == nullptr) {
        return "Элемент не ннайден";
    }
    if (target == lists[listName]) {
        return "Не может быть удален несуществующий в пустоте элемент";
    }
    deleteChelDo(lists[listName], target);
    return "Удален";
}
string FDELAFTER(const string& listName, const string& targetElement) {
    if (lists.find(listName) == lists.end()) {
        return "Список не найден";
    }
    Spisok* target = FindElement(lists[listName], targetElement);
    if (target == nullptr) {
        return "Список не найден";
    }
    if (target->place == nullptr) {
        return "Нельзя удалить элемент, существующий где-то после листа";
    }
    deleteChelPosle(target);
    return "Удален";
}
string FGET(const string& listName) {
    if (lists.find(listName) == lists.end()) {
        return "Список не найден";
    }
    if (lists[listName] == nullptr) {
        return "Список пуст";
    }
    stringstream output;
    auto old_cout_buffer = cout.rdbuf(output.rdbuf());
    print(lists[listName]);
    cout.rdbuf(old_cout_buffer);
    string result = output.str();
    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }
    return result;
}
// Вспомогательная функция для сохранения дерева в порядке BFS с использованием вашего BFS
void saveTreeBFS(Tree* root, ofstream& file) {
    if (root == nullptr) return;
    stringstream output;
    auto old_cout_buffer = cout.rdbuf(output.rdbuf());
    BFS(root);
    cout.rdbuf(old_cout_buffer);
    string bfsResult = output.str();
    istringstream iss(bfsResult);
    string token;

    while (iss >> token) {
        // token имеет формат "15r" или "20b", преобразуем в "15:r" "20:b"
        string value, color;
        for (char c : token) {
            if (isdigit(c) || c == '-') {
                value += c;
            } else {
                color = c;
            }
        }
        file << " " << value << ":" << color;
    }
}
void saveToFile(const string& filename) { 
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Ошибка открытия файла с именем " << filename << endl;
        return;
    }
    
    file << "[M]" << endl;
    for (const auto& pair : arrays) {
        file << pair.first << " " << pair.second.C << " " << pair.second.size;
        for (int i = 0; i < pair.second.size; i++) {
            file << " " << pair.second.num[i];
        }
        file << endl;
    }
    
    file << "\n[Q]" << endl;
    for (const auto& pair : queues) {
        file << pair.first;
        Toys* current = pair.second;
        while (current != nullptr) {
            file << " " << current->toy;
            current = current->site;
        }
        file << endl;
    }
    
    file << "\n[S]" << endl;
    for (const auto& pair : stacks) {
        file << pair.first;
        vector<string> stackItems;
        Garden* current = pair.second;
        while (current != nullptr) {
            stackItems.push_back(current->item);
            current = current->point;
        }
        for (auto it = stackItems.rbegin(); it != stackItems.rend(); ++it) {
            file << " " << *it;
        }
        file << endl;
    }
    
    file << "\n[L]" << endl;
    for (const auto& pair : lists) {
        file << pair.first;
        Spisok* current = pair.second;
        while (current != nullptr) {
            file << " " << current->name;
            current = current->place;
        }
        file << endl;
    }
    
    file << "\n[F]" << endl;
    for (const auto& pair : groups) {
        file << pair.first;
        Group* current = pair.second->head;  
        while (current != nullptr) {
            file << " " << current->student;
            current = current->next;
        }
        file << endl;
    }
    
    // ИСПРАВЛЕННАЯ СЕКЦИЯ ДЛЯ СОХРАНЕНИЯ ДЕРЕВЬЕВ В ШИРИНУ
    file << "\n[T]" << endl;
    for (const auto& pair : trees) {
        file << pair.first;
        // Сохраняем дерево в порядке обхода в ширину (BFS) используя вашу функцию
        saveTreeBFS(pair.second, file);
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
    for (auto& pair : arrays) {
        delete[] pair.second.num;
    }
    arrays.clear();
    for (auto& pair : queues) {
        Toys* element = pair.second;
        while (element != nullptr) {
            Toys* temp = element;
            element = element->site;
            delete temp;
        }
    }
    queues.clear();
    queueTails.clear();
    for (auto& pair : stacks) {
        Garden* element = pair.second;
        while (element != nullptr) {
            Garden* temp = element;
            element = element->point;
            delete temp;
        }
    }
    stacks.clear();
    for (auto& pair : lists) {
        Spisok* element = pair.second;
        while (element != nullptr) {
            Spisok* temp = element;
            element = element->place;
            delete temp;
        }
    }
    lists.clear();
    for (auto& pair : groups) {
        if (pair.second != nullptr) {
            Group* current = pair.second->head;
            while (current != nullptr) {
                Group* temp = current;
                current = current->next;
                delete temp;
            }
            delete pair.second;
        }
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
                Numbers newArray;
                newArray.num = new string[C];
                newArray.C = C;
                newArray.size = size;
                string value;
                int index = 0;
                while (iss >> value && index < size) {
                    newArray.num[index] = value;
                    index++;
                }
                arrays[arrayName] = newArray;
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
                    addQueue(queues[queueName], queueTails[queueName], item);
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
                for (auto it = items.rbegin(); it != items.rend(); ++it) {
                    addStack(stacks[stackName], *it);
                }
            }
        }
        else if (section == "F") {
            istringstream iss(line);
            string groupName;
            if (iss >> groupName) {
                GroupPoint* newGroup = new GroupPoint{nullptr, nullptr};
                string element;
                bool firstElement = true;
                while (iss >> element) {
                    if (firstElement) {
                        newGroup->head = new Group{element, nullptr, nullptr};
                        newGroup->tail = newGroup->head;
                        firstElement = false;
                    } else {
                        Group* newElement = new Group{element, nullptr, newGroup->tail};
                        newGroup->tail->next = newElement;
                        newGroup->tail = newElement;
                    }
                }
                groups[groupName] = newGroup;
            }
        }
        else if (section == "L") {
            istringstream iss(line);
            string listName;
            if (iss >> listName) {
                lists[listName] = nullptr;
                Spisok* tail = nullptr;
                string element;
                while (iss >> element) {
                    if (lists[listName] == nullptr) {
                        lists[listName] = new Spisok{element, nullptr};
                        tail = lists[listName];
                    } else {
                        tail->place = new Spisok{element, nullptr};
                        tail = tail->place;
                    }
                }
            }
        }
        else if (section == "T") {
            istringstream iss(line);
            string treeName;
            if (iss >> treeName) {
                trees[treeName] = nullptr;
                
                string valueColor;
                while (iss >> valueColor) {
                    // Парсим строку формата "значение:цвет"
                    size_t colonPos = valueColor.find(':');
                    if (colonPos != string::npos) {
                        int value = stoi(valueColor.substr(0, colonPos));
                        char colorChar = valueColor[colonPos + 1];
                        
                        if (trees[treeName] == nullptr) {
                            // Создаем корень
                            trees[treeName] = CreateRoot(value);
                            trees[treeName]->color = (colorChar == 'r') ? RED : BLACK;
                        } else {
                            // Добавляем лист с указанным цветом
                            Tree* newNode = CreateNode(value);
                            newNode->color = (colorChar == 'r') ? RED : BLACK;
                            
                            // Вставляем узел в дерево используя ваш алгоритм
                            Tree* current = trees[treeName];
                            Tree* parent = nullptr;
                            
                            while (current != nullptr) {
                                parent = current;
                                if (value < current->value) {
                                    current = current->left;
                                } else {
                                    current = current->right;
                                }
                            }
                            
                            newNode->parent = parent;
                            if (value < parent->value) {
                                parent->left = newNode;
                            } else {
                                parent->right = newNode;
                            }
                            
                            // Балансируем если нужно
                            if (newNode->color == RED && parent->color == RED) {
                                fixInsert(trees[treeName], newNode);
                            }
                        }
                    }
                }
            }
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
    if (result != "Двусвязный список не найден" && result != "Элемент не найден" && 
        result != "Ошибка" && !result.empty()) {
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
    if (result != "Массив не найден" && 
        result != "Индекс вышел за границы" && 
        result != "Индекс за границами" &&
        result != "Массив переполнен" &&
        result != "ОШИБКА") {
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
    if (result != "Список не найден" && result != "Элемент не найден"  && result != "Список пуст") {
        saveToFile(filename);
    }
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
            if (stacks.find(structureName) == stacks.end()) {
                stacks[structureName] = nullptr;
            }
            addStack(stacks[structureName], item);
        } else {
            if (queues.find(structureName) == queues.end()) {
                queues[structureName] = nullptr;
                queueTails[structureName] = nullptr;
            }
            addQueue(queues[structureName], queueTails[structureName], item);
        }
        result = item;
    }
    else if (command == "SPOP") {
        if (stacks.find(structureName) == stacks.end() || stacks[structureName] == nullptr) {
            return "Стек пустой";
        }
        result = popStack(stacks[structureName]);
    }
    else if (command == "QPOP") {
        if (queues.find(structureName) == queues.end() || queues[structureName] == nullptr) {
            return "Очередь пуста";
        }
        result = QPOP(queues[structureName]);
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
    else if (command == "TDEL") {  // ДОБАВЛЕНА НОВАЯ КОМАНДА
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
    else if (command == "TGETSYM") {
        result = TGETSYM(treeName);
    }
    else {
        return "Неизвестная команда";
    }
    
    // Сохраняем в файл после успешных операций
    if (result != "Дерево не найдено" && 
        result != "Элемент не найден или невозможно удалить" &&
        result != "ОШИБКА" &&
        !result.empty()) {
        saveToFile(filename);
    }
    
    return result;
}
string processQuery(const string& query, const string& filename) {
    istringstream iss(query);
    string command;
    iss >> command;
    if (command.find('L') == 0) {
        return processListCommand(command, iss, filename);
    }
    else if (command.find('M') == 0) {
        return processArrayCommand(command, iss, filename);
    }
    else if (command.find('F') == 0) {
        return processFListCommand(command, iss, filename);
    }
    else if (command == "SPUSH" || command == "SPOP" || command == "QPUSH" || command == "QPOP") {
        return processStackQueueCommand(command, iss, filename);
    }
    else if (command.find('T') == 0) {
        return processTreeCommand(command, iss, filename);
    }
    return "Неизвестная команда";
}
