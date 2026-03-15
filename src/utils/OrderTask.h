#pragma once

#include <chrono>
#include "models/Order.h"

namespace utils {

struct OrderTask {
    models::Order order;
    long long expireTime; // 毫秒时间戳
    
    OrderTask(const models::Order& o, int delaySeconds) : order(o) {
        auto now = std::chrono::system_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
        expireTime = ms + (delaySeconds * 1000LL);
    }
    
    // 优先队列需要的比较运算符（最小堆，最早过期的在队首）
    bool operator>(const OrderTask& other) const {
        return expireTime > other.expireTime;
    }
};

} // namespace utils
