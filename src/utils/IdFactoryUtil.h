#pragma once

#include <atomic>
#include <chrono>
#include <string>

namespace utils {

class IdFactoryUtil {
public:
    static std::string getFileId() {
        static std::atomic<int> counter{0};
        auto now = std::chrono::system_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()).count();
        int count = counter.fetch_add(1) % 10000;
        return std::to_string(ms) + std::to_string(count);
    }
    
    static std::string getOrderId() {
        static std::atomic<int> counter{0};
        auto now = std::chrono::system_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()).count();
        int count = counter.fetch_add(1) % 10000;
        return "ORD" + std::to_string(ms) + std::to_string(count);
    }
};

} // namespace utils
