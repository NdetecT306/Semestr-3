#include <iostream>
#include <string>
using namespace std;
const int HASH_SIZE = 256;
struct DOUBLE_HASH {
    int key;
    string value;
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
void INIT_HASH_TABLE() {
    for (int i = 0; i < HASH_SIZE; i++) {
        hashTable[i].value = "";
        hashTable[i].isDeleted = false;
        hashTable[i].isEmpty = true;
        hashTable[i].key = -1;
    }
}
bool ADD(int key, const string& value) {
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
bool SETDEL(int key, const string& value) {
    int index = hash1(key);
    int step = hash2(key);
    int startIndex = index;
    int attempts = 0;
    while (attempts < HASH_SIZE) {
        if (hashTable[index].isEmpty && !hashTable[index].isDeleted) {
            return false;
        }
        if (hashTable[index].key == key && hashTable[index].value == value && !hashTable[index].isDeleted) {
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
bool SET_AT(int key, const string& value) {
    int index = hash1(key);
    int step = hash2(key);
    int startIndex = index;
    int attempts = 0;
    while (attempts < HASH_SIZE) {
        if (hashTable[index].isEmpty && !hashTable[index].isDeleted) {
            return false;
        }
        if (hashTable[index].key == key && hashTable[index].value == value && !hashTable[index].isDeleted) {
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
void getAllElements(int* keys, int* values, int& count) {
    count = 0;
    for (int i = 0; i < HASH_SIZE; i++) {
        if (!hashTable[i].isEmpty && !hashTable[i].isDeleted) {
            keys[count] = hashTable[i].key;
            values[count] = hashTable[i].key; 
            count++;
        }
    }
}
int calculateTotalSum() {
    int sum = 0;
    for (int i = 0; i < HASH_SIZE; i++) {
        if (!hashTable[i].isEmpty && !hashTable[i].isDeleted) {
            sum += hashTable[i].key;
        }
    }
    return sum;
}
int findMinDifference(int* keys, int count, int totalSum) {
    bool** dp = new bool*[count + 1];
    for (int i = 0; i <= count; i++) {
        dp[i] = new bool[totalSum + 1];
        for (int j = 0; j <= totalSum; j++) {
            dp[i][j] = false;
        }
    }
    for (int i = 0; i <= count; i++) {
        dp[i][0] = true;
    }
    for (int i = 1; i <= count; i++) {
        for (int j = 1; j <= totalSum; j++) {
            dp[i][j] = dp[i - 1][j];
            if (keys[i - 1] <= j) {
                dp[i][j] = dp[i][j] || dp[i - 1][j - keys[i - 1]];
            }
        }
    }
    int minDiff = totalSum;
    for (int j = totalSum / 2; j >= 0; j--) {
        if (dp[count][j]) {
            minDiff = totalSum - 2 * j;
            break;
        }
    }
    return minDiff;
}
void findSubsets(int* keys, int count, int minDiff, int totalSum, int* subset1, int& count1, int* subset2, int& count2) {
    int target = (totalSum - minDiff) / 2;
    bool** dp = new bool*[count + 1];
    for (int i = 0; i <= count; i++) {
        dp[i] = new bool[target + 1];
        for (int j = 0; j <= target; j++) {
            dp[i][j] = false;
        }
    }
    for (int i = 0; i <= count; i++) {
        dp[i][0] = true;
    }
    for (int i = 1; i <= count; i++) {
        for (int j = 1; j <= target; j++) {
            dp[i][j] = dp[i - 1][j];
            if (keys[i - 1] <= j) {
                dp[i][j] = dp[i][j] || dp[i - 1][j - keys[i - 1]];
            }
        }
    }
    bool* inSubset1 = new bool[count] {false};
    int i = count, j = target;
    while (i > 0 && j > 0) {
        if (dp[i][j] && !dp[i - 1][j]) {
            inSubset1[i - 1] = true;
            j -= keys[i - 1];
        }
        i--;
    }
    count1 = 0;
    count2 = 0;
    for (int k = 0; k < count; k++) {
        if (inSubset1[k]) {
            subset1[count1++] = keys[k];
        } else {
            subset2[count2++] = keys[k];
        }
    }
}
int calculateArraySum(int* arr, int count) {
    int sum = 0;
    for (int i = 0; i < count; i++) {
        sum += arr[i];
    }
    return sum;
}
void partitionSet() {
    int keys[HASH_SIZE], values[HASH_SIZE];
    int count = 0;
    getAllElements(keys, values, count);
    if (count == 0) {
        cout << "Множество пусто!" << endl;
        return;
    }
    cout << "Ваше множество S: {";
    for (int i = 0; i < count; i++) {
        cout << keys[i];
        if (i < count - 1) cout << ", ";
    }
    cout << "}" << endl;
    int totalSum = calculateTotalSum();
    int minDiff = findMinDifference(keys, count, totalSum);
    int subset1[HASH_SIZE], subset2[HASH_SIZE];
    int count1 = 0, count2 = 0;
    findSubsets(keys, count, minDiff, totalSum, subset1, count1, subset2, count2);
    cout << "Подмножество S1: {";
    for (int i = 0; i < count1; i++) {
        cout << subset1[i];
        if (i < count1 - 1) cout << ", ";
    }
    cout << "}" << endl;
    cout << "Подмножество S2: {";
    for (int i = 0; i < count2; i++) {
        cout << subset2[i];
        if (i < count2 - 1) cout << ", ";
    }
    cout << "}" << endl;
    cout << "Разница сумм = " << minDiff << endl;
}
int main() {
    setlocale(LC_ALL, "rus");
    INIT_HASH_TABLE();
    int n;
    cout << "Введите количество чисел для добавления: ";
    cin >> n;
    cout << "Введите " << n << " чисел:" << endl;
    for (int i = 0; i < n; i++) {
        int num;
        cin >> num;
        ADD(num, to_string(num));
    }
    cout << "\nРезультат разбиения множества:" << endl;
    partitionSet();
    return 0;
}
