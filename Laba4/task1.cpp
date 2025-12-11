#include <iostream>
#include <thread>
#include <random>
#include <chrono>
#include <string>
#include <mutex>
#include <vector>
#include <barrier>
#include <atomic>
#include <condition_variable>
#include <semaphore>
using namespace std;
using namespace std::chrono;

mutex outB;
void myBarrier(int id, int summ) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(32, 126); 
    string letters;
    for(int i = 0; i < summ; i++) {
        char str = static_cast<char>(dis(gen));
        letters += str;
    }
}
void bar(int id, barrier<>& mybar, int summ){
    mybar.arrive_and_wait();
    lock_guard<mutex> lock(outB);
    myBarrier(id, summ);
}
mutex outM; 
condition_variable cv;
bool is_locked = false;
void myMonitor(int id, int summ) {
    {
        unique_lock<mutex> lock(outM);
        cv.wait(lock, []() { return !is_locked; }); 
        is_locked = true; 
    }
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(32, 126); 
    string letters;
    for(int i = 0; i < summ; i++) {
        char str = static_cast<char>(dis(gen));
        letters += str;
    }
    {
        lock_guard<mutex> lock(outM);
        is_locked = false;
        cv.notify_one();
    }
}
mutex out; 
void myMutexes(int id, int summ) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(32, 126);
    string letters;
    out.lock();
    for(int i = 0; i < summ; i++) {
        char str = static_cast<char>(dis(gen));
        letters += str;
    }
    out.unlock();
}
counting_semaphore<1> mysem(1);
void mySem(int id, int summ) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(32, 126); 
    string letters;
    mysem.acquire();
    for(int i = 0; i < summ; i++) {
        char str = static_cast<char>(dis(gen));
        letters += str;
    }
    mysem.release();
}
atomic_flag spinlock = ATOMIC_FLAG_INIT;
void mySpinLock(int id, int summ) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(32, 126); 
    string letters;
    while (spinlock.test_and_set(memory_order_acquire)) {}
    for(int i = 0; i < summ; i++) {
        char str = static_cast<char>(dis(gen));
        letters += str;
    }
    spinlock.clear(memory_order_release);
}
class SpinWait{
private:
    atomic_flag flag;
public:
    SpinWait() : flag(ATOMIC_FLAG_INIT) {}
    void lock(){
        while (flag.test_and_set(memory_order_acquire)) {
            this_thread::yield();
        }
    }
    void unlock(){
        flag.clear(memory_order_release);
    }
};
SpinWait spinwait;
void randomChars(int id, int summ) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(32, 126);  
    string letters;
    spinwait.lock();
    for (int i = 0; i < summ; ++i){
        char str = static_cast<char>(dis(gen));
        letters += str;
    }
    spinwait.unlock();
}
int main() {
    cout << "Barrier: " << endl;
    vector<thread> thBar;
    barrier<> mybar(5);
    auto startBar = high_resolution_clock::now(); 
    for(int i = 0; i < 50; i++) {
        thBar.emplace_back(bar, i, ref(mybar) ,15);
    }
    for(auto& t : thBar) {
        t.join();
    }
    auto endBar = high_resolution_clock::now(); 
    auto mytimeBar = duration_cast<microseconds>(endBar - startBar);
    cout << "Время в мкс: " << mytimeBar.count() << endl << endl;

    cout << "Monitor: " << endl;
    vector<thread> thMon;
    auto start = high_resolution_clock::now(); 
    for(int i = 0; i < 50; i++) {
        thMon.emplace_back(myMonitor, i, 15);
    }
    for(auto& t : thMon) {
        t.join();
    }
    auto end = high_resolution_clock::now(); 
    auto mytime = duration_cast<microseconds>(end - start);
    cout << "Время в мкс: " << mytime.count() << endl << endl;

    cout << "Mytexes: " << endl;
    vector<thread> threads;
    start = high_resolution_clock::now(); 
    for(int i = 0; i < 50; i++) {
        threads.emplace_back(myMutexes, i, 15);
    }
    for(auto& t : threads) {
        t.join();
    }
    end = high_resolution_clock::now(); 
    mytime = duration_cast<microseconds>(end - start);
    cout << "Время в мкс: " << mytime.count() << endl << endl;

    cout << "Semaphore: " << endl;
    vector<thread> thSem;
    start = high_resolution_clock::now(); 
    for(int i = 0; i < 50; i++) {
        thSem.emplace_back(mySem, i, 15);
    }
    for(auto& t : thSem) {
        t.join();
    }
    end = high_resolution_clock::now(); 
    mytime = duration_cast<microseconds>(end - start);
    cout << "Время в мкс: " << mytime.count() << endl << endl;

    cout << "SpinLock: " << endl;
    vector<thread> thSpL;
    start = high_resolution_clock::now(); 
    for(int i = 0; i < 50; i++) {
        thSpL.emplace_back(mySpinLock, i, 15);
    }
    for(auto& t : thSpL) {
        t.join();
    }
    end = high_resolution_clock::now(); 
    mytime = duration_cast<microseconds>(end - start);
    cout << "Время в мкс: " << mytime.count() << endl << endl;

    cout << "SpinWait: " << endl;
    vector<thread> thSpW;
    start = high_resolution_clock::now();
    for (int i = 0; i < 50; ++i) { 
        thSpW.push_back(thread(randomChars, i, 10));
    }
    for (auto& t : thSpW) {
        t.join();
    }
    end = high_resolution_clock::now();
    mytime = duration_cast<microseconds>(end - start);
    cout << "Время в мкс: " << mytime.count() << endl;
    return 0;
}
