#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <chrono>
#include "OrderTask.h"
#include "services/OrderService.h"

namespace utils {

class OrderTaskHandler {
public:
    static OrderTaskHandler& getInstance() {
        static OrderTaskHandler instance;
        return instance;
    }
    
    void start() {
        if (running.exchange(true)) {
            return; // 已经在运行
        }
        
        workerThread = std::thread(&OrderTaskHandler::processTask, this);
    }
    
    void stop() {
        running = false;
        cv.notify_all();
        if (workerThread.joinable()) {
            workerThread.join();
        }
    }
    
    void addOrder(const OrderTask& task) {
        std::lock_guard<std::mutex> lock(queueMutex);
        taskQueue.push(task);
        cv.notify_one();
    }
    
    ~OrderTaskHandler() {
        stop();
    }
    
private:
    OrderTaskHandler() : running(false) {}
    
    void processTask() {
        while (running) {
            std::unique_lock<std::mutex> lock(queueMutex);
            if (taskQueue.empty()) {
                cv.wait(lock);
                continue;
            }
            
            auto now = std::chrono::system_clock::now();
            auto nowMs = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
            
            const auto& top = taskQueue.top();
            
            if (top.expireTime <= nowMs) {
                // 任务到期，执行取消订单
                OrderTask task = taskQueue.top();
                taskQueue.pop();
                lock.unlock();
                
                // 调用服务层取消订单
                services::OrderService orderService;
                orderService.updateOrder(task.order);
                } else {
                // 等待到任务到期时间
                auto waitTime = std::chrono::milliseconds(top.expireTime - nowMs);
                cv.wait_for(lock, waitTime);
            }
        }
    }
    
    std::priority_queue<OrderTask, std::vector<OrderTask>, std::greater<OrderTask>> taskQueue;
    std::mutex queueMutex;
    std::condition_variable cv;
    std::atomic<bool> running;
    std::thread workerThread;
};

} // namespace utils
