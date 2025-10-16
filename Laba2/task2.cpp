#include <iostream>
#include <string>
#include <map>
#include <cstdlib>
#include <fstream>
#include <sstream>
using namespace std;
const int HASH_SIZE = 256;
struct DOUBLE_HASH {
    int key;
    int value;
    bool isDeleted;
    bool isEmpty;
};
map<string, DOUBLE_HASH*> hashes;
int hash1(int key) {
    return key % HASH_SIZE;
}
int hash2(int key) {
    return 13 - (key % 13);
}
DOUBLE_HASH* createHashTable() {
    DOUBLE_HASH* newHash = new DOUBLE_HASH[HASH_SIZE];
    for (int i = 0; i < HASH_SIZE; i++) {
        newHash[i].key = -1;
        newHash[i].value = -1;
        newHash[i].isDeleted = false;
        newHash[i].isEmpty = true;
    }
    return newHash;
}
bool SETADD(const string& setName, int value) {
    if (hashes.find(setName) == hashes.end()) {
        hashes[setName] = createHashTable();
    }
    DOUBLE_HASH* currentHash = hashes[setName];
    int index = hash1(value);
    int step = hash2(value);
    int startIndex = index;
    int attempts = 0;
    while (attempts < HASH_SIZE) {
        if (currentHash[index].isEmpty || currentHash[index].isDeleted) {
            currentHash[index].key = value;
            currentHash[index].value = value;
            currentHash[index].isDeleted = false;
            currentHash[index].isEmpty = false;
            return true;
        }
        if (currentHash[index].key == value && !currentHash[index].isDeleted) {
            return true; // Элемент уже существует
        }
        index = (index + step) % HASH_SIZE;
        attempts++;
        if (index == startIndex) {
            break;
        }
    }
    return false;
}
bool SETDEL(const string& setName, int value) {
    if (hashes.find(setName) == hashes.end()) {
        return false;
    }
    DOUBLE_HASH* currentHash = hashes[setName];
    int index = hash1(value);
    int step = hash2(value);
    int startIndex = index;
    int attempts = 0;
    while (attempts < HASH_SIZE) {
        if (currentHash[index].isEmpty && !currentHash[index].isDeleted) {
            return false;
        }
        if (currentHash[index].key == value && !currentHash[index].isDeleted) {
            currentHash[index].isDeleted = true;
            currentHash[index].isEmpty = true;
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
bool SET_AT(const string& setName, int value) {
    if (hashes.find(setName) == hashes.end()) {
        return false;
    }
    DOUBLE_HASH* currentHash = hashes[setName];
    int index = hash1(value);
    int step = hash2(value);
    int startIndex = index;
    int attempts = 0;
    while (attempts < HASH_SIZE) {
        if (currentHash[index].isEmpty && !currentHash[index].isDeleted) {
            return false;
        }
        if (currentHash[index].key == value && !currentHash[index].isDeleted) {
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
void saveToFile(const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Ошибка открытия файла" << endl;
        return;
    }
    file << "[SET]" << endl;
    for (const auto& pair : hashes) {
        file << pair.first;
        DOUBLE_HASH* currentHash = pair.second;
        for (int i = 0; i < HASH_SIZE; i++) {
            if (!currentHash[i].isEmpty && !currentHash[i].isDeleted) {
                file << " " << currentHash[i].key;
            }
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
    for (auto& pair : hashes) {
        delete[] pair.second;
    }
    hashes.clear();
    string line;
    string section;
    while (getline(file, line)) {
        if (line.empty()) continue;
        if (line == "[SET]") {
            section = "SET";
            continue;
        } 
        else if (section == "SET") {
            istringstream iss(line);
            string setName;
            iss >> setName;
            DOUBLE_HASH* newHash = createHashTable();
            int value;
            while (iss >> value) {
                int index = hash1(value);
                int step = hash2(value);
                int startIndex = index;
                int attempts = 0;
                while (attempts < HASH_SIZE) {
                    if (newHash[index].isEmpty || newHash[index].isDeleted) {
                        newHash[index].key = value;
                        newHash[index].value = value;
                        newHash[index].isDeleted = false;
                        newHash[index].isEmpty = false;
                        break;
                    }
                    index = (index + step) % HASH_SIZE;
                    attempts++;
                    if (index == startIndex) {
                        break;
                    }
                }
            }
            hashes[setName] = newHash;
        }
    }
    file.close();
}

string processQuery(const string& query, const string& filename) {
    istringstream iss(query);
    string operation, setName;
    int value;
    iss >> operation >> setName >> value;
    if (operation == "SETADD") {
        bool result = SETADD(setName, value);
        saveToFile(filename); 
        return (result) ? "Добавлен" : "Уже есть";
    }
    else if (operation == "SETDEL") {
        bool result = SETDEL(setName, value);
        saveToFile(filename);
        return result ? "Удален" : "Не найден";
    }
    else if (operation == "SET_AT") {
        bool result = SET_AT(setName, value);
        return result ? "true" : "false";
    }
    else {
        return "Неизвестная операция";
    }
}
int main(int argc, char* argv[]) {
    string filename, query;
    for (int i = 1; i < argc; i++) {
        if (string(argv[i]) == "--file" && i + 1 < argc) { 
            filename = argv[++i];
        } 
        else if (string(argv[i]) == "--query" && i + 1 < argc) {
            query = argv[++i];
        }
    }
    if (filename.empty() || query.empty()) {
        cerr << "Usage: " << argv[0] << " --file <filename> --query <query>" << endl;
        return 1;
    }
    loadFromFile(filename);
    string result = processQuery(query, filename);
    cout << "-> " << result << endl;
    for (auto& pair : hashes) {
        delete[] pair.second;
    }
    hashes.clear();
    return 0;
}
