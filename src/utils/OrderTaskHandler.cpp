#include "OrderTaskHandler.h"
#include "service/OrderService.h"

namespace myutils {

OrderTaskHandler& OrderTaskHandler::getInstance() {
    static OrderTaskHandler instance;
    return instance;
}

OrderTaskHandler::OrderTaskHandler() : running(false) {}

OrderTaskHandler::~OrderTaskHandler() {
    stop();
}

void OrderTaskHandler::start() {
    if (running.exchange(true)) return;
    workerThread = std::thread(&OrderTaskHandler::processTask, this);
}

void OrderTaskHandler::stop() {
    running = false;
    cv.notify_all();
    if (workerThread.joinable()) workerThread.join();
}

void OrderTaskHandler::addTask(const OrderTask& task) {
    std::lock_guard<std::mutex> lock(queueMutex);
    taskQueue.push(task);
    cv.notify_one();
}

void OrderTaskHandler::processTask() {
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
            OrderTask task = taskQueue.top();
            taskQueue.pop();
            lock.unlock();

            // 现在可以安全使用 OrderService
            services::OrderService orderService;
            orderService.updateOrder(task.order);
        } else {
            auto waitTime = std::chrono::milliseconds(top.expireTime - nowMs);
            cv.wait_for(lock, waitTime);
        }
    }
}

} // namespace myutils