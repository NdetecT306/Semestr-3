/*
#include <iostream>
#include <string>
using namespace std;
struct DOUBLE_HASH {
    int key;
    string value;
    bool isDeleted;
};
const int N = 10;
DOUBLE_HASH doubleTable[N];
int doubleH1(int key) {
    return key % N;
}
int doubleH2(int key) {
    return 1 + (key % (N - 1));
}
int DOUBLE_findInd(int key) {
    int h1 = doubleH1(key);
    int h2 = doubleH2(key);
    int ind = h1;
    for (int i = 0; i < N; i++) {
        if (doubleTable[ind].key == 0 || doubleTable[ind].isDeleted) {
            return ind; 
        }
        if (doubleTable[ind].key == key) {
            return ind;
        }
        ind = (h1 + i * h2) % N;
    }
    return -1;
}
void DOUBLE_ADD(int key, string str) {
    int ind = DOUBLE_findInd(key);
    if (ind == -1) {
        cout << "Таблица переполнена!" << endl;
        return;
    }
    doubleTable[ind].key = key;
    doubleTable[ind].value = str;
    doubleTable[ind].isDeleted = false;
    cout << "Ключ добавлен.";
}
void DOUBLE_POISK(int key) {
    int h1 = doubleH1(key);
    int h2 = doubleH2(key);
    int ind = h1;
    for (int i = 0; i < N; i++) {
        if (doubleTable[ind].key == key && !doubleTable[ind].isDeleted) {
            cout << "Ключ был найден" << endl;
            return;
        }
        if (doubleTable[ind].key == 0 && !doubleTable[ind].isDeleted) {
            break;
        }
        ind = (h1 + i * h2) % N;
    }
    cout << "Ключ не найден" << endl;
}
void DOUBLE_DELETE(int key) {
    int h1 = doubleH1(key);
    int h2 = doubleH2(key);
    int ind = h1;
    for (int i = 0; i < N; i++) {
        if (doubleTable[ind].key == key && !doubleTable[ind].isDeleted) {
            doubleTable[ind].isDeleted = true;
            cout << "Ключ был удален." << ind << endl;
            return;
        }
        if (doubleTable[ind].key == 0 && !doubleTable[ind].isDeleted) {
            break;
        }
        ind = (h1 + i * h2) % N;
    }
    cout << "Ключ не найден." << endl;
}
void DOUBLE_PRINT() {
    cout << "\nСостояние хеш-таблицы:" << endl;
    for (int i = 0; i < N; i++) {
        cout << "[" << i << "]: ";
        if (doubleTable[i].key == 0 && !doubleTable[i].isDeleted) {
            cout << "Пусто";
        }
        else if (doubleTable[i].isDeleted) {
            cout << "Удалено";
        }
        else {
            cout << doubleTable[i].key << " , " << doubleTable[i].value;
        }
        cout << endl;
    }
    cout << endl;
}
struct cuckooHASH {
    int key;
    string value;
    bool isDeleted; 
};
const int MAX_ITERATIONS = 10; 
cuckooHASH cuckooTable[N];
int cuckooH1(int key) {
    return key % N;
}
int cuckooH2(int key) {
    return (key / N) % N;
}
int cuckoofindKey(int key) {
    int h1 = cuckooH1(key);
    int h2 = cuckooH2(key);
    if (cuckooTable[h1].key == key && !cuckooTable[h1].isDeleted) {
        return h1;
    }
    if (cuckooTable[h2].key == key && !cuckooTable[h2].isDeleted) {
        return h2;
    }
    return -1;
}
void cuckooADD(int key, string str) {
    if (cuckoofindKey(key) != -1) {
        cout << "Ключ уже существует!" << endl;
        return;
    }
    int elementKey = key;
    string elementStr = str;
    for (int i = 0; i < MAX_ITERATIONS; i++) {
        int h1 = cuckooH1(elementKey);
        if (cuckooTable[h1].key == 0 || cuckooTable[h1].isDeleted) {
            cuckooTable[h1].key = elementKey;
            cuckooTable[h1].value = elementStr;
            cuckooTable[h1].isDeleted = false;
            cout << "Ключ добавлен." << endl;
            return;
        }
        swap(elementKey, cuckooTable[h1].key);
        swap(elementStr, cuckooTable[h1].value);
        int h2 = cuckooH2(elementKey);
        if (cuckooTable[h2].key == 0 || cuckooTable[h2].isDeleted) {
            cuckooTable[h2].key = elementKey;
            cuckooTable[h2].value = elementStr;
            cuckooTable[h2].isDeleted = false;
            cout << "Ключ добавлен." << endl;
            return;
        }
        swap(elementKey, cuckooTable[h2].key);
        swap(elementStr, cuckooTable[h2].value);
    }
    cout << "Таблица переполнена! Не удалось добавить ключ." << endl;
}
void cuckooPOISK(int key) {
    int ind = cuckoofindKey(key);
    if (ind != -1) {
        cout << "Ключ найден." << endl;
    }
    else {
        cout << "Ключ не найден." << endl;
    }
}
void cuckooDELETE(int key) {
    int ind = cuckoofindKey(key);
    if (ind != -1) {
        cuckooTable[ind].isDeleted = true;
        cout << "Ключ был удален." << endl;
    }
    else {
        cout << "Ключ не найден." << endl;
    }
}
void cuckooPRINT() {
    cout << "\nСостояние хеш-таблицы:" << endl;
    for (int i = 0; i < N; i++) {
        cout << "[" << i << "]: ";
        if (cuckooTable[i].key == 0 && !cuckooTable[i].isDeleted) {
            cout << "Пусто";
        }
        else if (cuckooTable[i].isDeleted) {
            cout << "Удалено";
        }
        else {
            cout << cuckooTable[i].key << " , " << cuckooTable[i].value;
        }
        cout << endl;
    }
    cout << endl;
}
int main() {
    return 0;
} 
*/
#include <iostream>
#include <string>
using namespace std;
const int HASH_SIZE = 256; 
struct DOUBLE_HASH {
    int key;        
    int value;      
    bool isDeleted;
    bool isEmpty;
};
DOUBLE_HASH hashTable[HASH_SIZE];
int hash1(int key) {
    return key % HASH_SIZE;
}
int hash2(int key) {
    return 13 - (key % 13); 
}
void CREATE() {
    for (int i = 0; i < HASH_SIZE; i++) {
        hashTable[i].key = -1;
        hashTable[i].value = -1;
        hashTable[i].isDeleted = false;
        hashTable[i].isEmpty = true;
    }
}
bool ADD(int key, int value) {
    int index = hash1(key);
    int step = hash2(key);
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
bool POISK(int key, int& result) {
    int index = hash1(key);
    int step = hash2(key);
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
bool DELETE(int key) {
    int index = hash1(key);
    int step = hash2(key);
    int startIndex = index;
    int attempts = 0;
    while (attempts < HASH_SIZE) {
        if (hashTable[index].isEmpty && !hashTable[index].isDeleted) {
            return false;
        }
        if (hashTable[index].key == key && !hashTable[index].isDeleted) {
            hashTable[index].isDeleted = true;
            hashTable[index].isEmpty = true;
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
struct cuckooHASH {
    int key;
    string value;
    bool isDeleted;
    bool isEmpty; // Добавляем явный признак пустой ячейки
};
const int N = 10;
const int MAX_ITERATIONS = 10;
cuckooHASH cuckooTable[N];

// Инициализация таблицы
void initCuckooTable() {
    for (int i = 0; i < N; i++) {
        cuckooTable[i].key = 0;
        cuckooTable[i].value = "";
        cuckooTable[i].isDeleted = false;
        cuckooTable[i].isEmpty = true;
    }
}
int cuckooH1(int key) {
    return key % N;
}
int cuckooH2(int key) {
    return (key / N) % N;
}
int cuckoofindKey(int key) {
    int h1 = cuckooH1(key);
    int h2 = cuckooH2(key);
    if (!cuckooTable[h1].isEmpty && !cuckooTable[h1].isDeleted && cuckooTable[h1].key == key) {
        return h1;
    }
    if (!cuckooTable[h2].isEmpty && !cuckooTable[h2].isDeleted && cuckooTable[h2].key == key) {
        return h2;
    }
    return -1;
}
void cuckooADD(int key, string str) {
    if (cuckoofindKey(key) != -1) {
        cout << "Ключ уже существует!" << endl;
        return;
    }
    int elementKey = key;
    string elementStr = str;
    for (int i = 0; i < MAX_ITERATIONS; i++) {
        int h1 = cuckooH1(elementKey);
        if (cuckooTable[h1].isEmpty || cuckooTable[h1].isDeleted) {
            cuckooTable[h1].key = elementKey;
            cuckooTable[h1].value = elementStr;
            cuckooTable[h1].isDeleted = false;
            cuckooTable[h1].isEmpty = false;
            cout << "Ключ добавлен." << endl;
            return;
        }
        swap(elementKey, cuckooTable[h1].key);
        swap(elementStr, cuckooTable[h1].value);
        int h2 = cuckooH2(elementKey);
        if (cuckooTable[h2].isEmpty || cuckooTable[h2].isDeleted) {
            cuckooTable[h2].key = elementKey;
            cuckooTable[h2].value = elementStr;
            cuckooTable[h2].isDeleted = false;
            cuckooTable[h2].isEmpty = false;
            cout << "Ключ добавлен." << endl;
            return;
        }
        swap(elementKey, cuckooTable[h2].key);
        swap(elementStr, cuckooTable[h2].value);
    }
    cout << "Таблица переполнена! Не удалось добавить ключ." << endl;
}
void cuckooPOISK(int key) {
    int ind = cuckoofindKey(key);
    if (ind != -1) {
        cout << "Ключ найден: " << cuckooTable[ind].key << " -> " << cuckooTable[ind].value << endl;
    }
    else {
        cout << "Ключ не найден." << endl;
    }
}
void cuckooDELETE(int key) {
    int ind = cuckoofindKey(key);
    if (ind != -1) {
        cuckooTable[ind].isDeleted = true;
        cout << "Ключ был удален." << endl;
    }
    else {
        cout << "Ключ не найден." << endl;
    }
}
void cuckooPRINT() {
    cout << "\nСостояние хеш-таблицы:" << endl;
    for (int i = 0; i < N; i++) {
        cout << "[" << i << "]: ";
        if (cuckooTable[i].isEmpty) {
            cout << "Пусто";
        }
        else if (cuckooTable[i].isDeleted) {
            cout << "Удалено (был: " << cuckooTable[i].key << ")";
        }
        else {
            cout << cuckooTable[i].key << " , " << cuckooTable[i].value;
        }
        cout << endl;
    }
    cout << endl;
}
void findLongestSubstring(string s, int& maxLength, string& longestSubstr) {
    if (s.empty()) {
        maxLength = 0;
        longestSubstr = "";
        return;
    }
    CREATE();
    maxLength = 0;
    int start = 0; 
    int maxStart = 0; 
    for (int end = 0; end < s.length(); end++) {
        int charCode = static_cast<int>(s[end]);
        int lastPosition;
        if (POISK(charCode, lastPosition)) {
            if (lastPosition >= start) {
                start = lastPosition + 1; 
            }
        }
        ADD(charCode, end);
        if (end - start + 1 > maxLength) {
            maxLength = end - start + 1;
            maxStart = start;
        }
    }
    longestSubstr = s.substr(maxStart, maxLength);
}
int main() {
    setlocale(LC_ALL, "rus");
    string yourStr;
    cout << "Ввод: ";
    cin >> yourStr;
    int maxLength;
    string longestSubstr;
    findLongestSubstring(yourStr, maxLength, longestSubstr);
    cout << "Вывод: " << maxLength << " (" << longestSubstr << ")" << endl;
    return 0;
}
