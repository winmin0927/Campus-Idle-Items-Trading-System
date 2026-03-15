#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include "enums/ErrorMsg.h"

namespace vo {

template<typename T = void>
struct R {
    int statusCode;
    std::string msg;
    T data;
    
    static R<T> success(const T&d = T()) {
        R<T> r;
        r.statusCode = 1;
        r.msg = "success";
        r.data = d;
        return r;
    }
    
    static R<T> fail(const enums::ErrorMsg& error) {
        R<T> r;
        r.statusCode = 0;
        r.msg = error.getMessage();
        return r;
    }
    
    nlohmann::json toJson() const {
        nlohmann::json j;
        j["status_code"] = statusCode;
        j["msg"] = msg;
        if constexpr (!std::is_same_v<T, void>) {
            j["data"] = data;
        }
        return j;
    }
};

// void特化
template<>
struct R<void> {
    int statusCode;
    std::string msg;
    
    static R<void> success() {
        R<void> r;
        r.statusCode = 1;
        r.msg = "success";
        return r;
    }
    
    static R<void> fail(const enums::ErrorMsg& error) {
        R<void> r;
        r.statusCode = 0;
        r.msg = error.getMessage();
        return r;
    }
    
    nlohmann::json toJson() const {
        nlohmann::json j;
        j["status_code"] = statusCode;
        j["msg"] = msg;
        j["data"] = nullptr;
        return j;
    }
};

} // namespace vo
