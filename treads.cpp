#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

mutex mtx; 
condition_variable condition;
int sharedCounter = 0; 
bool done = false; 

void countUp() {
    for (int i = sharedCounter; i < 20; ++i) {
        lock_guard<mutex> lock(mtx);
        sharedCounter++;
        cout << "Counting up:  " << sharedCounter << endl;
    }
    {
        lock_guard<mutex> lock(mtx);
        done = true;
    }
    condition.notify_one(); // Notify next thread that this one is done.
}

void countDown() {
    unique_lock<mutex> lock(mtx);
    condition.wait(lock, [] { return done; }); // Wait until countUp() is done
    for (int i = sharedCounter; i >= 0; --i) {
        cout << "Counting down: " << sharedCounter << endl;
        sharedCounter--;
    }
}

int main() {
    thread thread1(countUp);
    thread thread2(countDown);

    thread1.join(); 
    thread2.join();

    cout << "\nCounting complete" << endl;

    return 0;
}