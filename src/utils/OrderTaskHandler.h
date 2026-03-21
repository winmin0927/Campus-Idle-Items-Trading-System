#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <chrono>
#include <functional>
#include "OrderTask.h"

namespace myutils {

class OrderTaskHandler {
public:
    static OrderTaskHandler& getInstance();
    void start();
    void stop();
    void addTask(const OrderTask& task);
    ~OrderTaskHandler();

private:
    OrderTaskHandler();
    void processTask();

    std::priority_queue<OrderTask, std::vector<OrderTask>, std::greater<OrderTask>> taskQueue;
    std::mutex queueMutex;
    std::condition_variable cv;
    std::atomic<bool> running;
    std::thread workerThread;
};

} // namespace myutils