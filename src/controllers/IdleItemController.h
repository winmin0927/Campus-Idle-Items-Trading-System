#pragma once

#include <drogon/drogon.h>
#include <chrono>
#include "service/IdleItemService.h"
#include "vo/R.h"
#include "enums/ErrorMsg.h"

using namespace drogon;

namespace controllers {

class IdleItemController : public HttpController<IdleItemController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(IdleItemController::addIdleItem, "/idle/add", Post);
    ADD_METHOD_TO(IdleItemController::getIdleItem, "/idle/info", Get);
    ADD_METHOD_TO(IdleItemController::getAllIdleItem, "/idle/all", Get);
    ADD_METHOD_TO(IdleItemController::findIdleItem, "/idle/find", Get);
    ADD_METHOD_TO(IdleItemController::findIdleItemByLabel, "/idle/lable", Get);
    ADD_METHOD_TO(IdleItemController::updateIdleItem, "/idle/update", Post);
    METHOD_LIST_END
    
    void addIdleItem(const HttpRequestPtr &req,
                     std::function<void(const HttpResponsePtr &)> &&callback) {
        auto shUserId = req->getCookie("shUserId");
        if (shUserId.empty()) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::COOKIE_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toValue()));
            return;
        }
        
        models::IdleItem idleItem = models::IdleItem::VtoModel(*req->getJsonObject());
        idleItem.userId = std::stol(shUserId);
        idleItem.idleStatus = 1;
        
        // 设置发布时间
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        char buffer[64];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&time_t));
        idleItem.releaseTime = buffer;
        
        services::IdleItemService idleService;
        if (idleService.addIdleItem(idleItem)) {
            auto resp = vo::R<models::IdleItem>::success(idleItem);
            callback(HttpResponse::newHttpJsonResponse(resp.toValue()));
        } else {
            auto resp = vo::R<>::fail(enums::ErrorMsg::SYSTEM_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toValue()));
        }
    }
    
    void getIdleItem(const HttpRequestPtr &req,
                     std::function<void(const HttpResponsePtr &)> &&callback) {
        long id = std::stol(req->getParameter("id"));
        services::IdleItemService idleService;
        auto idleItem = idleService.getIdleItem(id);
        
        if (idleItem.has_value()) {
            auto resp = vo::R<vo::IdleItemVo>::success(*idleItem);
            callback(HttpResponse::newHttpJsonResponse(resp.toValue()));
        } else {
            auto resp = vo::R<>::fail(enums::ErrorMsg::PARAM_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toValue()));
        }
    }
    
    void getAllIdleItem(const HttpRequestPtr &req,
                       std::function<void(const HttpResponsePtr &)> &&callback) {
        auto shUserId = req->getCookie("shUserId");
        if (shUserId.empty()) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::COOKIE_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toValue()));
            return;
        }
        
        long userId = std::stol(shUserId);
        services::IdleItemService idleService;
        auto items = idleService.getAllIdleItem(userId);
        
        auto resp = vo::R<std::vector<models::IdleItem>>::success(items);
        callback(HttpResponse::newHttpJsonResponse(resp.toValue()));
    }
    
    void findIdleItem(const HttpRequestPtr &req,
                     std::function<void(const HttpResponsePtr &)> &&callback) {
        std::string findValue = req->getOptionalParameter<std::string>("findValue").value_or("");
        int page = req->getOptionalParameter<int>("page").value_or(1);
        int nums = req->getOptionalParameter<int>("nums").value_or(8);
        
        page = page > 0 ? page : 1;
        nums = nums > 0 ? nums : 8;
        
        services::IdleItemService idleService;
        auto pageVo = idleService.findIdleItem(findValue, page, nums);
        
        auto resp = vo::R<vo::PageVo<vo::IdleItemVo>>::success(pageVo);
        callback(HttpResponse::newHttpJsonResponse(resp.toValue()));
    }
    
    void findIdleItemByLabel(const HttpRequestPtr &req,
                            std::function<void(const HttpResponsePtr &)> &&callback) {
        auto idleLabelOpt = req->getOptionalParameter<int>("idleLabel");
        if (!idleLabelOpt.has_value()) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::PARAM_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toValue()));
            return;
        }
        
        int idleLabel = idleLabelOpt.value();
        int page = req->getOptionalParameter<int>("page").value_or(1);
        int nums = req->getOptionalParameter<int>("nums").value_or(8);
        
        page = page > 0 ? page : 1;
        nums = nums > 0 ? nums : 8;
        
        services::IdleItemService idleService;
        auto pageVo = idleService.findIdleItemByLabel(idleLabel, page, nums);
        
        auto resp = vo::R<vo::PageVo<vo::IdleItemVo>>::success(pageVo);
        callback(HttpResponse::newHttpJsonResponse(resp.toValue()));
    }
    
    void updateIdleItem(const HttpRequestPtr &req,
                std::function<void(const HttpResponsePtr &)> &&callback) {
        auto shUserId = req->getCookie("shUserId");
        if (shUserId.empty()) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::COOKIE_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toValue()));
            return;
        }

        models::IdleItem idleItem = models::IdleItem::VtoModel(*req->getJsonObject());
        idleItem.userId = std::stol(shUserId);
        
        services::IdleItemService idleService;
        if (idleService.updateIdleItem(idleItem)) {
            auto resp = vo::R<>::success();
            callback(HttpResponse::newHttpJsonResponse(resp.toValue()));
        } else {
            auto resp = vo::R<>::fail(enums::ErrorMsg::SYSTEM_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toValue()));
        }
    }
};

} // namespace controllers
