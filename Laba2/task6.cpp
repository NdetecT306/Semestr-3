#include <iostream>
#include <string>
using namespace std;
const int N = 256; // Размер 
const int MAX_ITERATIONS = 10;
struct CuckooHash {
    int key;        
    int value;      
    bool isDeleted;
    bool isEmpty;
};
CuckooHash cuckooTable[N];
// Инициализация хеш-таблицы
void CREATECuckooTable() {
    for (int i = 0; i < N; i++) {
        cuckooTable[i].key = -1;
        cuckooTable[i].value = -1;
        cuckooTable[i].isDeleted = false;
        cuckooTable[i].isEmpty = true;
    }
}
// Первая хеш-функция
int cuckooH1(int key) {
    return key % N;
}
// Вторая хеш-функция
int cuckooH2(int key) {
    return (key / N) % N;
}
// Поиск ключа в хеш-таблице
int cuckooFindKey(int key) {
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
// Добавление 
bool cuckooADD(int key, int value) {
    // Проверяем, существует ли уже ключ
    int existingIndex = cuckooFindKey(key);
    if (existingIndex != -1) {
        // Обновляем 
        cuckooTable[existingIndex].value = value;
        return true;
    }
    int currentKey = key;
    int currentValue = value;
    for (int i = 0; i < MAX_ITERATIONS; i++) {
        // Пробуем добавить
        int h1 = cuckooH1(currentKey);
        if (cuckooTable[h1].isEmpty || cuckooTable[h1].isDeleted) {
            cuckooTable[h1].key = currentKey;
            cuckooTable[h1].value = currentValue;
            cuckooTable[h1].isDeleted = false;
            cuckooTable[h1].isEmpty = false;
            return true;
        }
        // Вытесняем существующий элемент
        swap(currentKey, cuckooTable[h1].key);
        swap(currentValue, cuckooTable[h1].value);
        // Пробуем добавить во вторую позицию
        int h2 = cuckooH2(currentKey);
        if (cuckooTable[h2].isEmpty || cuckooTable[h2].isDeleted) {
            cuckooTable[h2].key = currentKey;
            cuckooTable[h2].value = currentValue;
            cuckooTable[h2].isDeleted = false;
            cuckooTable[h2].isEmpty = false;
            return true;
        }
        swap(currentKey, cuckooTable[h2].key);
        swap(currentValue, cuckooTable[h2].value);
    }
    return false; 
}
bool cuckooPOISK(int key, int& result) {
    int index = cuckooFindKey(key);
    if (index != -1) {
        result = cuckooTable[index].value;
        return true;
    }
    return false;
}
bool cuckooDELETE(int key) {
    int index = cuckooFindKey(key);
    if (index != -1) {
        cuckooTable[index].isDeleted = true;
        cuckooTable[index].isEmpty = true;
        return true;
    }
    return false;
}
void findLongestSubstring(string s, int& maxLength, string& longestSubstr) { //Поиск
    if (s.empty()) {
        maxLength = 0;
        longestSubstr = "";
        return;
    }
    CREATECuckooTable();
    maxLength = 0;
    int start = 0;      // Начало текущей 
    int maxStart = 0;   // Начало необходимой
    for (int end = 0; end < s.length(); end++) {
        int charCode = static_cast<int>(s[end]);
        int lastPosition;
        if (cuckooPOISK(charCode, lastPosition)) {// Проверяем, встречался ли символ ранее в текущей подстроке
            if (lastPosition >= start) {
                start = lastPosition + 1; 
            }
        }
        cuckooADD(charCode, end);
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
