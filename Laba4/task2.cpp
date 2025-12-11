#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <chrono>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <algorithm>
#include <stdexcept>
#include <random>
#include <cstdlib>
#include <ctime>
using namespace std;
using namespace chrono;
class Time {
private:
    int date;
    int month;
    int year;
    bool isLeapYear(int y) const {
        return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
    }
    int getDaysInMonth(int m, int y) const {
        switch(m) {
            case 1: case 3: case 5: case 7: case 8: case 10: case 12: return 31;
            case 4: case 6: case 9: case 11: return 30;
            case 2: return isLeapYear(y) ? 29 : 28;
            default: return 0;
        }
    }
    bool isValidDate(int d, int m, int y) const {
        if (y < 1 || y > 9999) return false;
        if (m < 1 || m > 12) return false;
        if (d < 1) return false;
        int maxDays = getDaysInMonth(m, y);
        return d <= maxDays;
    } 
public:
    Time(int d, int m, int y) {
        setDate(d, m, y);
    }
    Time(int d, int m, int y, bool noCheck) : date(d), month(m), year(y) {}
    void setDate(int d, int m, int y) {
        if (!isValidDate(d, m, y)) {
            throw invalid_argument("Некорректная дата: " + to_string(d) + "." + to_string(m) + "." + to_string(y));
        }
        date = d;
        month = m;
        year = y;
    }
    int getDate() const { return date; }
    int getMonth() const { return month; }
    int getYear() const { return year; }
    bool isValid() const {
        return isValidDate(date, month, year);
    }
    void print() const {
        cout << date << "." << month << "." << year;
    }
    string toString() const {
        return to_string(date) + "." + to_string(month) + "." + to_string(year);
    }
};
struct ThreadData {
    vector<Time>* dates;
    vector<Time>* wrongDates;
    mutex* mtx;
    int startIdx;
    int endIdx;
};
void processDatesPart(ThreadData* data) {
    vector<Time> localWrong;
    for (int i = data->startIdx; i < data->endIdx; i++) {
        if (!(*data->dates)[i].isValid()) {
            localWrong.push_back((*data->dates)[i]);
        }
    }
    lock_guard<mutex> lock(*(data->mtx));
    for (const auto& date : localWrong) {
        data->wrongDates->push_back(date);
    }
}
void processDatesWithoutThread(const vector<Time>& dates, vector<Time>& wrongDates) {
    wrongDates.clear();
    for (const auto& date : dates) {
        if (!date.isValid()) {
            wrongDates.push_back(date);
        }
    }
}
void processDatesMultiThread(const vector<Time>& dates, vector<Time>& wrongDates, int numThreads) {
    wrongDates.clear();
    vector<thread> threads;
    vector<ThreadData> threadData(numThreads);
    mutex mtx;
    int datesPerThread = dates.size() / numThreads;
    int remainder = dates.size() % numThreads;
    int currentStart = 0;
    for (int i = 0; i < numThreads; i++) {
        int chunkSize = datesPerThread + (i < remainder ? 1 : 0);
        threadData[i] = {const_cast<vector<Time>*>(&dates), &wrongDates, &mtx, currentStart, currentStart + chunkSize};
        threads.emplace_back(processDatesPart, &threadData[i]);
        currentStart += chunkSize;
    }
    for (auto& t : threads) {
        t.join();
    }
}
Time generateRandomDate() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> yearDist(1, 9999);
    uniform_int_distribution<> monthDist(1, 12);
    uniform_int_distribution<> dayDist(1, 31);
    int year = yearDist(gen);
    int month = monthDist(gen);
    int day = dayDist(gen);
    return Time(day, month, year, true);
}
int main() {
    srand(static_cast<unsigned int>(time(nullptr)));
    int n;
    cout << "Введите количество дат: ";
    cin >> n;
    if (n <= 0) {
        cout << "Количество дат должно быть положительным числом." << endl;
        return -1;
    }
    vector<Time> dates;
    dates.reserve(n);
    for (int i = 0; i < n; i++) {
        dates.push_back(generateRandomDate());
    }
    int numThreads;
    cout << "Введите количество потоков: ";
    cin >> numThreads;
    if (numThreads < 1) {
        cout << "Количество потоков должно быть положительным числом." << endl;
        return -1;
    }
    vector<Time> wrongDatesSingle;
    auto startWithout = high_resolution_clock::now();
    processDatesWithoutThread(dates, wrongDatesSingle);
    auto endWithout = high_resolution_clock::now();
    duration<double, micro> durationSingle = endWithout - startWithout;
    vector<Time> wrongDatesMulti;
    auto startMulti = high_resolution_clock::now();
    processDatesMultiThread(dates, wrongDatesMulti, numThreads);
    auto endMulti = chrono::high_resolution_clock::now();
    duration<double, micro> durationMulti = endMulti - startMulti;
    cout << "Время обработки без многопоточности: " << durationSingle.count() << " мкс" << endl;
    cout << "Время обработки с многопоточностью (" << numThreads << " потоков): " << durationMulti.count() << " мкс" << endl;
    cout << "\nНеправильные даты (" << wrongDatesMulti.size() << " шт.):" << endl;
    for (size_t i = 0; i < wrongDatesMulti.size(); i++) {
        cout << i + 1 << ". ";
        wrongDatesMulti[i].print();
        cout << endl;
    }
    return 0;
}
