#pragma once

#include <drogon/drogon.h>
#include <chrono>
#include "services/UserService.h"
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
        auto json = req->getJsonObject();
        models::User user = *json;
        
        // 设置注册时间
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        char buffer[64];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&time_t));
        user.signInTime = buffer;
        
        // 设置默认头像
        if (user.avatar.empty()) {
            user.avatar = "https://cube.elemecdn.com/0/88/03b0d39583f48206768a7534e55bcpng.png";
        }
        
        services::UserService userService;
        if (userService.userSignIn(user)) {
            auto resp = vo::R<models::User>::success(user);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
        } else {
            auto resp = vo::R<>::fail(enums::ErrorMsg::REGISTER_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
        }
    }
    
    void login(const HttpRequestPtr &req,
               std::function<void(const HttpResponsePtr &)> &&callback,
               const std::string &accountNumber,
               const std::string &userPassword) {
        services::UserService userService;
        auto user = userService.userLogin(accountNumber, userPassword);
        
        if (!user.has_value()) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::EMAIL_LOGIN_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
            return;
        }
        
        // 检查用户状态（是否被封禁）
        if (user->userStatus ==1) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::ACCOUNT_BAN);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
            return;
        }
        
        // 设置Cookie
        auto resp = vo::R<models::User>::success(*user);
        auto httpResp = HttpResponse::newHttpJsonResponse(resp.toJson());
        
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
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
            return;
        }
        
        // 清除Cookie
        auto resp = vo::R<>::success();
        auto httpResp = HttpResponse::newHttpJsonResponse(resp.toJson());
        
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
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
            return;
        }
        
        long userId = std::stol(shUserId);
        services::UserService userService;
        auto user = userService.getUser(userId);
        
        if (user.has_value()) {
            auto resp = vo::R<models::User>::success(*user);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
        } else {
            auto resp = vo::R<>::fail(enums::ErrorMsg::SYSTEM_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
        }
    }
    
    void updateInfo(const HttpRequestPtr &req,
                    std::function<void(const HttpResponsePtr &)> &&callback) {
        auto shUserId = req->getCookie("shUserId");
        if (shUserId.empty()) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::COOKIE_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
            return;
        }
        
        auto json = req->getJsonObject();
        models::User user = *json;
        user.id = std::stol(shUserId);
        
        services::UserService userService;
        if (userService.updateUserInfo(user)) {
            auto resp = vo::R<>::success();
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
        } else {
            auto resp = vo::R<>::fail(enums::ErrorMsg::SYSTEM_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
        }
    }
    
    void updatePassword(const HttpRequestPtr &req,
                        std::function<void(const HttpResponsePtr &)> &&callback,
                        const std::string &oldPassword,
                        const std::string &newPassword) {
        auto shUserId = req->getCookie("shUserId");
        if (shUserId.empty()) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::COOKIE_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
            return;
        }
        
        long userId = std::stol(shUserId);
        services::UserService userService;
        
        if (userService.updatePassword(newPassword, oldPassword, userId)) {
            auto resp = vo::R<>::success();
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
        } else {
            auto resp = vo::R<>::fail(enums::ErrorMsg::PASSWORD_RESET_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
        }
    }
};

} // namespace controllers
