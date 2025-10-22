#include <iostream>
#include <string>
#include <map>
#include <cstdlib>
#include <fstream>
#include <sstream>
using namespace std;
const int HASH_SIZE = 256;
struct DOUBLE_HASH {
    string value;
    bool isDeleted;
    bool isEmpty;
};
map<string, DOUBLE_HASH*> sets;
int hash1(int key) {
    return key % HASH_SIZE;
}
int hash2(int key) {
    return 13 - (key % 13);
}
DOUBLE_HASH* createHashTable() {
    DOUBLE_HASH* newHash = new DOUBLE_HASH[HASH_SIZE];
    for (int i = 0; i < HASH_SIZE; i++) {
        newHash[i].value = "";
        newHash[i].isDeleted = false;
        newHash[i].isEmpty = true;
    }
    return newHash;
}
bool SETADD(const string& setName, const string& value) {
    if (sets.find(setName) == sets.end()) {
        sets[setName] = createHashTable();
    }
    DOUBLE_HASH* currentSet = sets[setName];
    int key = 1; 
    int index = hash1(key);
    int step = hash2(key);
    int startIndex = index;
    int attempts = 0;
    while (attempts < HASH_SIZE) {
        if (currentSet[index].isEmpty || currentSet[index].isDeleted) {
            currentSet[index].value = value;
            currentSet[index].isDeleted = false;
            currentSet[index].isEmpty = false;
            return true;
        }
        if (currentSet[index].value == value && !currentSet[index].isDeleted) {
            return false; 
        }
        index = (index + step) % HASH_SIZE;
        attempts++;
        if (index == startIndex) {
            break;
        }
    }
    return false;
}
bool SETDEL(const string& setName, const string& value) {
    if (sets.find(setName) == sets.end()) {
        return false;
    }
    DOUBLE_HASH* currentSet = sets[setName];
    int key = 1; 
    int index = hash1(key);
    int step = hash2(key);
    int startIndex = index;
    int attempts = 0;
    while (attempts < HASH_SIZE) {
        if (currentSet[index].isEmpty && !currentSet[index].isDeleted) {
            return false;
        }
        if (currentSet[index].value == value && !currentSet[index].isDeleted) {
            currentSet[index].isDeleted = true;
            currentSet[index].isEmpty = true;
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
bool SET_AT(const string& setName, const string& value) {
    if (sets.find(setName) == sets.end()) {
        return false;
    }
    DOUBLE_HASH* currentSet = sets[setName];
    int key = 1; 
    int index = hash1(key);
    int step = hash2(key);
    int startIndex = index;
    int attempts = 0;
    while (attempts < HASH_SIZE) {
        if (currentSet[index].isEmpty && !currentSet[index].isDeleted) {
            return false;
        }
        if (currentSet[index].value == value && !currentSet[index].isDeleted) {
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
        return;
    }
    file << "[SET]" << endl;
    for (const auto& pair : sets) {
        file << pair.first;
        DOUBLE_HASH* currentSet = pair.second;
        for (int i = 0; i < HASH_SIZE; i++) {
            if (!currentSet[i].isEmpty && !currentSet[i].isDeleted) {
                file << " " << currentSet[i].value; 
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
    for (auto& pair : sets) {
        delete[] pair.second;
    }
    sets.clear();
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
            DOUBLE_HASH* newSet = createHashTable();
            string token;
            int key = 1; 
            while (iss >> token) {
                string value = token;
                int index = hash1(key);
                int step = hash2(key);
                int startIndex = index;
                int attempts = 0;
                while (attempts < HASH_SIZE) {
                    if (newSet[index].isEmpty || newSet[index].isDeleted) {
                        newSet[index].value = value;
                        newSet[index].isDeleted = false;
                        newSet[index].isEmpty = false;
                        break;
                    }
                    index = (index + step) % HASH_SIZE;
                    attempts++;
                    if (index == startIndex) {
                        break;
                    }
                }
            }
            sets[setName] = newSet;
        }
    }
    file.close();
}
string processQuery(const string& query, const string& filename) {
    istringstream iss(query);
    string operation, setName, value;
    iss >> operation >> setName;
    if (operation == "SETADD") {
        string remaining;
        getline(iss, remaining);
        size_t firstNonSpace = remaining.find_first_not_of(' ');
        if (firstNonSpace != string::npos) {
            value = remaining.substr(firstNonSpace);
        } else {
            value = "";
        }
        cout << " \"" << value << "\"" << endl;
        bool result = SETADD(setName, value);
        saveToFile(filename);
        return result ? "Добавлен" : "Уже есть";
    }
    else if (operation == "SETDEL") {
        string remaining;
        getline(iss, remaining);
        size_t firstNonSpace = remaining.find_first_not_of(' ');
        if (firstNonSpace != string::npos) {
            value = remaining.substr(firstNonSpace);
        } else {
            value = "";
        }
        cout << " \"" << value << "\"" << endl;
        bool result = SETDEL(setName, value);
        saveToFile(filename);
        return result ? "Удален" : "Не найден";
    }
    else if (operation == "SET_AT") {
        string remaining;
        getline(iss, remaining);
        size_t firstNonSpace = remaining.find_first_not_of(' ');
        if (firstNonSpace != string::npos) {
            value = remaining.substr(firstNonSpace);
        } else {
            value = "";
        }
        cout << " \"" << value << "\"" << endl;
        bool result = SET_AT(setName, value);
        return result ? "есть" : "нет";
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
    for (auto& pair : sets) {
        delete[] pair.second;
    }
    sets.clear();
    return 0;
}
