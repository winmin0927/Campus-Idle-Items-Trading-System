#pragma once

#include <nlohmann/json.hpp>
#include <json/json.h>
#include <string>
#include "enums/ErrorMsg.h"

namespace vo {

template<typename T = void>
struct R {
    int statusCode;
    std::string msg;
    T data;

    static R<T> success(const T& d = T()) {
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

    Json::Value toValue() const {
        Json::Value root;
        root["status_code"] = statusCode;
        root["msg"] = msg;

        if constexpr (!std::is_same_v<T, void>) {
            // 处理 data 字段
            if constexpr (std::is_same_v<T, Json::Value>) {
                root["data"] = data;
            } else {
                // 将 T 转换为 nlohmann::json，再转为 Json::Value
                nlohmann::json j = data;
                Json::Reader reader;
                Json::Value jsonData;
                if (reader.parse(j.dump(), jsonData)) {
                    root["data"] = jsonData;
                } else {
                    root["data"] = Json::Value(); // 空对象
                }
            }
        }
        return root;
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
    
    Json::Value toValue() const {
        nlohmann::json j;
        j["status_code"] = statusCode;
        j["msg"] = msg;
        j["data"] = nullptr;
        
        Json::Value root;
        Json::Reader reader;
        reader.parse(j.dump(), root);
        return root;
    }
};

} // namespace vo