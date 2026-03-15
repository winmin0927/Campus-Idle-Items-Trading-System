#pragma once

#include <drogon/drogon.h>
#include <nlohmann/json.hpp>
#include "vo/R.h"
#include "enums/ErrorMsg.h"
#include "exception/ParamException.h"

using namespace drogon;

namespace handler {

/**
 * 全局异常处理器
 */
class GlobalExceptionHandler {
public:
    /**
     * 注册全局异常处理器到Drogon应用
     */
    static void registerHandler() {
        app().setExceptionHandler([](const std::exception &e, const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
            handleException(e, callback);
        });
    }
    
    /**
     * 处理异常并返回统一响应
     */
    static void handleException(const std::exception &e, std::function<void(const HttpResponsePtr &)> &&callback) {
        try {
            // 尝试转换为ParamException
            auto paramEx = dynamic_cast<const exception::ParamException*>(&e);
            if (paramEx) {
                handleParamException(*paramEx, std::move(callback));
                return;
            }
            
            // 处理其他异常
            handleGenericException(e, std::move(callback));
            
        } catch (...) {
            // 异常处理器本身出错，返回系统错误
            auto resp = vo::R<>::fail(enums::ErrorMsg::SYSTEM_ERROR);
            nlohmann::json j = resp;
            auto httpResp = HttpResponse::newHttpJsonResponse(j);
            callback(httpResp);
        }
    }
    
    /**
     * 处理参数异常
     */
    static void handleParamException(const exception::ParamException &e, std::function<void(const HttpResponsePtr &)> &&callback) {
        nlohmann::json errorData = e.getMap();
        auto resp = vo::R<nlohmann::json>::fail(enums::ErrorMsg::PARAM_ERROR, errorData);
        nlohmann::json j = resp;
        auto httpResp = HttpResponse::newHttpJsonResponse(j);
        callback(httpResp);
    }
    
    /**
     * 处理通用异常
     */
    static void handleGenericException(const std::exception &e, std::function<void(const HttpResponsePtr &)> &&callback) {
        std::string errorMsg = e.what();
        
        // 根据错误消息判断异常类型
        if (errorMsg.find("Cookie") != std::string::npos || errorMsg.find("cookie") != std::string::npos) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::COOKIE_ERROR);
            nlohmann::json j = resp;
            auto httpResp = HttpResponse::newHttpJsonResponse(j);
            callback(httpResp);} else if (errorMsg.find("parameter") != std::string::npos || errorMsg.find("参数") != std::string::npos) {
            auto resp = vo::R<std::string>::fail(enums::ErrorMsg::MISSING_PARAMETER, errorMsg);
            nlohmann::json j = resp;
            auto httpResp = HttpResponse::newHttpJsonResponse(j);
        } else if (errorMsg.find("JSON") != std::string::npos || errorMsg.find("json") != std::string::npos) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::JSON_READ_ERROR);
            nlohmann::json j = resp;
            auto httpResp = HttpResponse::newHttpJsonResponse(j);} else {
            // 默认系统错误
            auto resp = vo::R<std::string>::fail(enums::ErrorMsg::SYSTEM_ERROR, errorMsg);
            nlohmann::json j = resp;
            auto httpResp = HttpResponse::newHttpJsonResponse(j);
        }
        callback(httpResp);
    }/**
     * 辅助方法：在控制器中捕获异常并处理
     */
    template<typename Func>
    static void tryCatch(Func &&func, std::function<void(const HttpResponsePtr &)> &&callback) {
        try {
            func();
        } catch (const std::exception &e) {
            handleException(e, std::move(callback));
        }
    }
};

} // namespace handler
