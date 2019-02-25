#include "concurrent_queue.h"

#include <thread>
#include <atomic>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <mutex>

int main(int argc, char **argv) {
    std::vector<int> vPushCount;
    std::vector<int> vPopCount;
    std::mutex countMutex;

    tc_utils::ConcurrentQueue<int> abc(100);

    volatile bool bTerminate = false;

    std::vector<std::thread> vPushThread;
    for (int i = 0; i < 5; ++i) {
        std::thread t([&abc,&bTerminate, &countMutex, &vPushCount]{
            std::atomic<int> count(0);
            while (!bTerminate) {
                bool pushRet = abc.push(1, 300.0);
                if (pushRet) {
                    ++count;
                }
            }

            std::cout << "pushed count:" << count << std::endl;
            {
                std::lock_guard<std::mutex> guard(countMutex);
                vPushCount.push_back(count);
            }
        });

        vPushThread.emplace_back(std::move(t));
    }

    std::vector<std::thread> vPopThread;
    for (int i = 0; i < 5; ++i) {
        std::thread t([&abc,&bTerminate, &countMutex, &vPopCount]{
            std::atomic<int> count(0);
            while (!bTerminate) {
                int a;
                bool popRet = abc.pop(a, 300.0);
                if (popRet) {
                    ++count;
                }
            }

            std::cout << "pop count:" << count << std::endl;
            {
                std::lock_guard<std::mutex> guard(countMutex);
                vPopCount.push_back(count);
            }
        });

        vPopThread.emplace_back(std::move(t));
    }

    usleep(30000000);

    bTerminate = true;

    for (auto &item : vPushThread) {
        item.join();
    }
    for (auto &item : vPopThread) {
        item.join();
    }

    int sum = 0;
    for (auto &item : vPushCount) {
        sum += item;
    }
    for (auto &item : vPopCount) {
        sum -= item;
    }

    std::cout << "final queue size:" << abc.size() << ", sum:" << sum << std::endl;

    return 0;
}
