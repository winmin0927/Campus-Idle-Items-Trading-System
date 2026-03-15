#pragma once

#include <drogon/drogon.h>
#include "services/AdminService.h"
#include "services/IdleItemService.h"
#include "services/OrderService.h"
#include "services/UserService.h"
#include "vo/R.h"
#include "enums/ErrorMsg.h"

using namespace drogon;

namespace controllers {

class AdminController : public HttpController<AdminController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(AdminController::login, "/admin/login", Get);
    ADD_METHOD_TO(AdminController::loginOut, "/admin/loginOut", Get);
    ADD_METHOD_TO(AdminController::getAdminList, "/admin/list", Get);
    ADD_METHOD_TO(AdminController::addAdmin, "/admin/add", Post);
    ADD_METHOD_TO(AdminController::idleList, "/admin/idleList", Get);
    ADD_METHOD_TO(AdminController::updateIdleStatus, "/admin/updateIdleStatus", Get);
    ADD_METHOD_TO(AdminController::orderList, "/admin/orderList", Get);
    ADD_METHOD_TO(AdminController::deleteOrder, "/admin/deleteOrder", Get);
    ADD_METHOD_TO(AdminController::userList, "/admin/userList", Get);
    ADD_METHOD_TO(AdminController::updateUserStatus, "/admin/updateUserStatus", Get);
    METHOD_LIST_END
    void login(const HttpRequestPtr &req,
               std::function<void(const HttpResponsePtr &)> &&callback,
               const std::string &accountNumber,
               const std::string &adminPassword) {
        services::AdminService adminService;
        auto admin = adminService.login(accountNumber, adminPassword);
        
        if (!admin.has_value()) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::EMAIL_LOGIN_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
            return;
        }
        
        // 设置session cookie
        auto sessionId = generateSessionId();
        adminSessions[sessionId] = *admin;
        
        auto resp = vo::R<models::Admin>::success(*admin);
        auto httpResp = HttpResponse::newHttpJsonResponse(resp.toJson());
        httpResp->addCookie("adminSessionId", sessionId);
        callback(httpResp);
    }
    
    void loginOut(const HttpRequestPtr &req,
                  std::function<void(const HttpResponsePtr &)> &&callback) {
        auto sessionId = req->getCookie("adminSessionId");
        if (!sessionId.empty()) {
            adminSessions.erase(sessionId);
        }
        
        auto resp = vo::R<>::success();
        callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
    }
    
    void getAdminList(const HttpRequestPtr &req,
                      std::function<void(const HttpResponsePtr &)> &&callback) {
        if (!checkAdminAuth(req)) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::COOKIE_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
            return;
        }
        
        int page = req->getOptionalParameter<int>("page").value_or(1);
        int nums = req->getOptionalParameter<int>("nums").value_or(8);
        page = page > 0 ? page : 1;
        nums = nums > 0 ? nums : 8;
        
        services::AdminService adminService;
        auto pageVo = adminService.getAdminList(page, nums);
        
        auto resp = vo::R<vo::PageVo<models::Admin>>::success(pageVo);
        callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
    }
    
    void addAdmin(const HttpRequestPtr &req,
                  std::function<void(const HttpResponsePtr &)> &&callback) {
        if (!checkAdminAuth(req)) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::COOKIE_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
            return;
        }
        
        auto json = req->getJsonObject();
        models::Admin admin = *json;
        
        services::AdminService adminService;
        if (adminService.addAdmin(admin)) {
            auto resp = vo::R<>::success();
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
        } else {
            auto resp = vo::R<>::fail(enums::ErrorMsg::PARAM_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
        }
    }
    
    void idleList(const HttpRequestPtr &req,
                  std::function<void(const HttpResponsePtr &)> &&callback,
                  int status) {
        if (!checkAdminAuth(req)) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::COOKIE_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
            return;
        }
        
        int page = req->getOptionalParameter<int>("page").value_or(1);
        int nums = req->getOptionalParameter<int>("nums").value_or(8);
        page = page > 0 ? page : 1;
        nums = nums > 0 ? nums : 8;
        
        services::IdleItemService idleService;
        auto pageVo = idleService.adminGetIdleList(status, page, nums);
        
        auto resp = vo::R<vo::PageVo<vo::IdleItemVo>>::success(pageVo);
        callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
    }
    
    void updateIdleStatus(const HttpRequestPtr &req,std::function<void(const HttpResponsePtr &)> &&callback,
                          long id,
                          int status) {
        if (!checkAdminAuth(req)) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::COOKIE_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
            return;
        }
        
        models::IdleItem idleItem;
        idleItem.id = id;
        idleItem.idleStatus = status;
        
        services::IdleItemService idleService;
        if (idleService.updateIdleItem(idleItem)) {
            auto resp = vo::R<>::success();
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
        } else {
            auto resp = vo::R<>::fail(enums::ErrorMsg::SYSTEM_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
        }
    }
    
    void orderList(const HttpRequestPtr &req,
                   std::function<void(const HttpResponsePtr &)> &&callback) {
        if (!checkAdminAuth(req)) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::COOKIE_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
            return;
        }
        
        int page = req->getOptionalParameter<int>("page").value_or(1);
        int nums = req->getOptionalParameter<int>("nums").value_or(8);
        page = page > 0 ? page : 1;
        nums = nums > 0 ? nums : 8;
        
        services::OrderService orderService;
        auto pageVo = orderService.getAllOrder(page, nums);
        
        auto resp = vo::R<vo::PageVo<models::Order>>::success(pageVo);
        callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
    }
    
    void deleteOrder(const HttpRequestPtr &req,
                     std::function<void(const HttpResponsePtr &)> &&callback,
                     long id) {
        if (!checkAdminAuth(req)) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::COOKIE_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
            return;
        }
        
        services::OrderService orderService;
        if (orderService.deleteOrder(id)) {
            auto resp = vo::R<>::success();
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
        } else {
            auto resp = vo::R<>::fail(enums::ErrorMsg::SYSTEM_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
        }
    }
    
    void userList(const HttpRequestPtr &req,
                  std::function<void(const HttpResponsePtr &)> &&callback,
                  int status) {
        if (!checkAdminAuth(req)) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::COOKIE_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
            return;
        }
        
        int page = req->getOptionalParameter<int>("page").value_or(1);
        int nums = req->getOptionalParameter<int>("nums").value_or(8);
        page = page > 0 ? page : 1;
        nums = nums > 0 ? nums : 8;
        
        services::UserService userService;
        auto pageVo = userService.getUserByStatus(status, page, nums);
        
        auto resp = vo::R<vo::PageVo<models::User>>::success(pageVo);
        callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
    }
    
    void updateUserStatus(const HttpRequestPtr &req,std::function<void(const HttpResponsePtr &)> &&callback,
                          long id,
                          int status) {
        if (!checkAdminAuth(req)) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::COOKIE_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
            return;
        }
        
        models::User user;
        user.id = id;
        user.userStatus = status;
        
        services::UserService userService;
        if (userService.updateUserInfo(user)) {
            auto resp = vo::R<>::success();
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
        } else {
            auto resp = vo::R<>::fail(enums::ErrorMsg::SYSTEM_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
        }
    }
    
private:
    static std::unordered_map<std::string, models::Admin> adminSessions;
    bool checkAdminAuth(const HttpRequestPtr &req) {
        auto sessionId = req->getCookie("adminSessionId");
        return !sessionId.empty() && adminSessions.find(sessionId) != adminSessions.end();
    }
    
    std::string generateSessionId() {
        return std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    }
};

std::unordered_map<std::string, models::Admin> AdminController::adminSessions;

} // namespace controllers
