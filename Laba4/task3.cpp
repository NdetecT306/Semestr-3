#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <chrono>
#include <string>
#include <condition_variable>
using namespace std;
class TaskAboutPhilosophyWithWaiter {
private:
    int numb;
    int rfork;
    int lfork;
    string name;
    static mutex out;
    static condition_variable cv;
    static mutex waiterMutex;
    static vector<bool> forks;
    static const vector<string> philosopherNames;
public:
    TaskAboutPhilosophyWithWaiter(int numb, int rfork, int lfork)
        : numb(numb), rfork(rfork), lfork(lfork) {
        name = philosopherNames[numb];
    }
    void solution() {
        while(true) {
            {
                lock_guard<mutex> lock(out);
                cout << name << " думает. Наверное, о смысле этой задачи." << endl;
            }
            this_thread::sleep_for(chrono::seconds(5));
            {
                unique_lock<mutex> lock(waiterMutex);
                // ИСПРАВЛЕНО: проверяем конкретные вилки, которые нужны философу
                cv.wait(lock, [this]() { 
                    return forks[lfork] && forks[rfork]; 
                });
                forks[lfork] = false;
                forks[rfork] = false;
                {
                    lock_guard<mutex> lockOut(out);
                    cout << "Официант дал " << name << " вилки " << lfork << " и " << rfork << endl;
                    cout << name << " кушает. Не будем его отвлекать от трапезы." << endl;
                }
            }
            this_thread::sleep_for(chrono::seconds(5));  
            {
                lock_guard<mutex> lock(waiterMutex);
                forks[lfork] = true;
                forks[rfork] = true;
                {
                    lock_guard<mutex> lockOut(out);
                    cout << name << " перестал есть и вернул вилки официанту." << endl;
                }
            }
            cv.notify_all(); 
        }
    }
};
mutex TaskAboutPhilosophyWithWaiter::out;
condition_variable TaskAboutPhilosophyWithWaiter::cv;
mutex TaskAboutPhilosophyWithWaiter::waiterMutex;
vector<bool> TaskAboutPhilosophyWithWaiter::forks(5, true);
const vector<string> TaskAboutPhilosophyWithWaiter::philosopherNames = {"Аристотель", "Платон", "Сократ", "Декарт", "Кант"};
int main() {
    vector<thread> philosophy;
    for(int i = 0; i < 5; i++) {
        philosophy.emplace_back(&TaskAboutPhilosophyWithWaiter::solution, TaskAboutPhilosophyWithWaiter(i, (i+1)%5, i));
    }
    for(auto& el : philosophy) {
        el.join();
    }
    return 0;
}
