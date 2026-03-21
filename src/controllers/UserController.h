#pragma once

#include <drogon/drogon.h>
#include <chrono>
#include "service/UserService.h"
#include "vo/R.h"
#include "enums/ErrorMsg.h"

using namespace drogon;

namespace controllers {

class UserController : public HttpController<UserController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(UserController::signIn, "/user/sign-in", Post);
    ADD_METHOD_TO(UserController::login, "/user/login", Get);
    ADD_METHOD_TO(UserController::logout, "/user/logout", Get);
    ADD_METHOD_TO(UserController::getInfo, "/user/info", Get);
    ADD_METHOD_TO(UserController::updateInfo, "/user/info", Post);
    ADD_METHOD_TO(UserController::updatePassword, "/user/password", Get);
    METHOD_LIST_END

    void signIn(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback) {

        models::User user = models::User::VtoModel(*req->getJsonObject());
        user.userStatus = 0;
        
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        char buffer[64];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&time_t));
        user.signInTime = buffer;
        
        // avatar 字段存后缀名，默认为空（读取时会fallback 到 A_default.jpg）
        if (user.avatar.empty()) {
            user.avatar = "";// 空表示使用默认头像
        }
        
        services::UserService userService;
        if (userService.userSignIn(user)) {
            auto resp = vo::R<models::User>::success(user);
            callback(HttpResponse::newHttpJsonResponse(resp.toValue()));
        } else {
            auto resp = vo::R<>::fail(enums::ErrorMsg::REGISTER_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toValue()));
        }
    }
    
    void login(const HttpRequestPtr &req,
            std::function<void(const HttpResponsePtr &)> &&callback) {
        auto accountNumber = req->getOptionalParameter<std::string>("accountNumber");
        auto userPassword = req->getOptionalParameter<std::string>("userPassword");

        if (!accountNumber || !userPassword) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::PARAM_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toValue()));
            return;
        }

        services::UserService userService;
        auto user = userService.userLogin(accountNumber.value(), userPassword.value());

        if (!user.has_value()) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::EMAIL_LOGIN_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toValue()));
            return;
        }

        if (user->userStatus == 1) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::ACCOUNT_BAN);
            callback(HttpResponse::newHttpJsonResponse(resp.toValue()));
            return;
        }

        auto resp = vo::R<models::User>::success(*user);
        auto httpResp = HttpResponse::newHttpJsonResponse(resp.toValue());
        Cookie cookie("shUserId", std::to_string(user->id));
        cookie.setPath("/");
        cookie.setHttpOnly(false);
        httpResp->addCookie(cookie);
        callback(httpResp);
    }
    
    void logout(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback) {
        auto shUserId = req->getCookie("shUserId");
        if (shUserId.empty()) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::COOKIE_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toValue()));
            return;
        }
        
        // 清除Cookie
        auto resp = vo::R<>::success();
        auto httpResp = HttpResponse::newHttpJsonResponse(resp.toValue());
        
        Cookie cookie("shUserId", "");
        cookie.setPath("/");
        cookie.setHttpOnly(true);
        cookie.setMaxAge(0);  // 立即过期
        httpResp->addCookie(cookie);
        
        callback(httpResp);
    }
    
    void getInfo(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback) {
        auto shUserId = req->getCookie("shUserId");
        if (shUserId.empty()) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::COOKIE_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toValue()));
            return;
        }
        
        long userId = std::stol(shUserId);
        services::UserService userService;
        auto user = userService.getUser(userId);
        
        if (user.has_value()) {
            // 将 avatar 字段转换为完整的 URL 返回给前端
            auto config = AppConfig::getInstance();
            user->avatar = config->getBaseUrl() + "/avatar?userId=" + std::to_string(userId);
            auto resp = vo::R<models::User>::success(*user);
            callback(HttpResponse::newHttpJsonResponse(resp.toValue()));
        } else {
            auto resp = vo::R<>::fail(enums::ErrorMsg::SYSTEM_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toValue()));
        }
    }
    
    void updateInfo(const HttpRequestPtr &req,
                    std::function<void(const HttpResponsePtr &)> &&callback) {
        auto shUserId = req->getCookie("shUserId");
        if (shUserId.empty()) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::COOKIE_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toValue()));
            return;
        }
        
        models::User user = models::User::VtoModel(*req->getJsonObject());
        user.id = std::stol(shUserId);
        
        services::UserService userService;
        if (userService.updateUserInfo(user)) {
            auto resp = vo::R<>::success();
            callback(HttpResponse::newHttpJsonResponse(resp.toValue()));
        } else {
            auto resp = vo::R<>::fail(enums::ErrorMsg::SYSTEM_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toValue()));
        }
    }
    
    void updatePassword(const HttpRequestPtr &req,
                        std::function<void(const HttpResponsePtr &)> &&callback) {
        auto oldPassword = req->getOptionalParameter<std::string>("oldPassword").value();
        auto newPassword = req->getOptionalParameter<std::string>("newPassword").value();
        auto shUserId = req->getCookie("shUserId");
        if (shUserId.empty()) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::COOKIE_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toValue()));
            return;
        }
        
        long userId = std::stol(shUserId);
        services::UserService userService;
        
        if (userService.updatePassword(newPassword, oldPassword, userId)) {
            auto resp = vo::R<>::success();
            callback(HttpResponse::newHttpJsonResponse(resp.toValue()));
        } else {
            auto resp = vo::R<>::fail(enums::ErrorMsg::PASSWORD_RESET_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toValue()));
        }
    }
};

} // namespace controllers
