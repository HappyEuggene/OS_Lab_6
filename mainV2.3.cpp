#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

const int MAX_VALUE = 1000;
int sharedVariable = 0;
std::mutex mutex;
std::condition_variable cv;
int turn = 1;

void synchronizedAddition(int threadNumber) {
    while (true) {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, [&]() { return sharedVariable >= MAX_VALUE || turn == threadNumber; });

        if (sharedVariable >= MAX_VALUE) {
            cv.notify_all();
            break;
        }

        ++sharedVariable;
        std::cout << "Thread " << threadNumber << " incremented value to " << sharedVariable << std::endl;

        turn = (turn % 3) + 1;
        cv.notify_all();
    }
}

int main() {
    std::thread t1(synchronizedAddition, 1);
    std::thread t2(synchronizedAddition, 2);
    std::thread t3(synchronizedAddition, 3);

    t1.join();
    t2.join();
    t3.join();

    std::cout << "Final value of sharedVariable: " << sharedVariable << std::endl;

    return 0;
}
