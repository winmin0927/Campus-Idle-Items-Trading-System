#pragma once

#include <drogon/drogon.h>
#include <chrono>
#include "services/MessageService.h"
#include "vo/R.h"
#include "enums/ErrorMsg.h"

using namespace drogon;

namespace controllers {

class MessageController : public HttpController<MessageController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(MessageController::sendMessage, "/message/send", Post);
    ADD_METHOD_TO(MessageController::getMessage, "/message/info", Get);
    ADD_METHOD_TO(MessageController::getAllIdleMessage, "/message/idle", Get);
    ADD_METHOD_TO(MessageController::getAllMyMessage, "/message/my", Get);
    ADD_METHOD_TO(MessageController::deleteMessage, "/message/delete", Get);
    METHOD_LIST_END
    void sendMessage(const HttpRequestPtr &req,
                     std::function<void(const HttpResponsePtr &)> &&callback) {
        auto shUserId = req->getCookie("shUserId");
        if (shUserId.empty()) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::COOKIE_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
            return;
        }
        
        auto json = req->getJsonObject();
        models::Message message = *json;
        message.userId = std::stol(shUserId);
        // 设置当前时间
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        char buffer[64];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&time_t));
        message.createTime = buffer;
        
        services::MessageService messageService;
        if (messageService.addMessage(message)) {
            auto resp = vo::R<models::Message>::success(message);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
        } else {
            auto resp = vo::R<>::fail(enums::ErrorMsg::SYSTEM_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
        }
    }
    
    void getMessage(const HttpRequestPtr &req,
                    std::function<void(const HttpResponsePtr &)> &&callback,
                    long id) {
        services::MessageService messageService;
        auto message = messageService.getMessage(id);
        
        if (message.has_value()) {
            auto resp = vo::R<models::Message>::success(*message);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
        } else {
            auto resp = vo::R<>::fail(enums::ErrorMsg::PARAM_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
        }
    }
    
    void getAllIdleMessage(const HttpRequestPtr &req,
                          std::function<void(const HttpResponsePtr &)> &&callback,
                          long idleId) {
        services::MessageService messageService;
        auto messages = messageService.getAllIdleMessage(idleId);
        
        auto resp = vo::R<std::vector<vo::MessageVo>>::success(messages);
        callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
    }
    
    void getAllMyMessage(const HttpRequestPtr &req,
                        std::function<void(const HttpResponsePtr &)> &&callback) {
        auto shUserId = req->getCookie("shUserId");
        if (shUserId.empty()) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::COOKIE_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
            return;
        }
        
        long userId = std::stol(shUserId);
        services::MessageService messageService;
        auto messages = messageService.getAllMyMessage(userId);
        
        auto resp = vo::R<std::vector<vo::MessageVo>>::success(messages);
        callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
    }
    
    void deleteMessage(const HttpRequestPtr &req,
                      std::function<void(const HttpResponsePtr &)> &&callback,
                      long id) {
        auto shUserId = req->getCookie("shUserId");
        if (shUserId.empty()) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::COOKIE_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
            return;
        }
        
        services::MessageService messageService;
        if (messageService.deleteMessage(id)) {
            auto resp = vo::R<>::success();
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
        } else {
            auto resp = vo::R<>::fail(enums::ErrorMsg::SYSTEM_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
        }
    }
};

} // namespace controllers
