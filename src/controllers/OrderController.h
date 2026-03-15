#pragma once

#include <drogon/drogon.h>
#include <chrono>
#include "services/OrderService.h"
#include "utils/IdFactoryUtil.h"
#include "utils/OrderTaskHandler.h"
#include "vo/R.h"
#include "enums/ErrorMsg.h"

using namespace drogon;

namespace controllers {

class OrderController : public HttpController<OrderController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(OrderController::addOrder, "/order/add", Post);
    ADD_METHOD_TO(OrderController::getOrderInfo, "/order/info", Get);
    ADD_METHOD_TO(OrderController::updateOrder, "/order/update", Post);
    ADD_METHOD_TO(OrderController::getMyOrder, "/order/my", Get);
    ADD_METHOD_TO(OrderController::getMySoldIdle, "/order/my-sold", Get);
    METHOD_LIST_END
    void addOrder(const HttpRequestPtr &req,
                  std::function<void(const HttpResponsePtr &)> &&callback) {
        auto shUserId = req->getCookie("shUserId");
        if (shUserId.empty()) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::COOKIE_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
            return;
        }
        
        auto json = req->getJsonObject();
        models::Order order = *json;
        order.orderNumber = utils::IdFactoryUtil::getOrderId();
        order.userId = std::stol(shUserId);
        order.orderStatus = 0;
        order.paymentStatus = 0;
        
        // 设置创建时间
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        char buffer[64];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&time_t));
        order.createTime = buffer;
        
        services::OrderService orderService;
        if (orderService.addOrder(order)) {
            auto resp = vo::R<models::Order>::success(order);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
        } else {
            auto resp = vo::R<>::fail(enums::ErrorMsg::SYSTEM_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
        }
    }
    
    void getOrderInfo(const HttpRequestPtr &req,
                      std::function<void(const HttpResponsePtr &)> &&callback,
                      long id) {
        auto shUserId = req->getCookie("shUserId");
        if (shUserId.empty()) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::COOKIE_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
            return;
        }
        
        long userId = std::stol(shUserId);
        services::OrderService orderService;
        auto order = orderService.getOrder(id);
        
        if (order.has_value()) {
            // 验证权限：订单所有者或商品卖家
            if (order->userId == userId || order->idleItem.userId == userId) {
                auto resp = vo::R<models::Order>::success(*order);
                callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
                return;
            }
        }
        
        auto resp = vo::R<>::fail(enums::ErrorMsg::SYSTEM_ERROR);
        callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
    }
    
    void updateOrder(const HttpRequestPtr &req,
                     std::function<void(const HttpResponsePtr &)> &&callback) {
        auto shUserId = req->getCookie("shUserId");
        if (shUserId.empty()) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::COOKIE_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
            return;
        }
        
        auto json = req->getJsonObject();
        models::Order order = *json;
        
        // 如果支付状态为已支付，设置支付时间
        if (order.paymentStatus == 1) {
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            char buffer[64];
            std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&time_t));
            order.paymentTime = buffer;
        }
        
        services::OrderService orderService;
        if (orderService.updateOrder(order)) {
            auto resp = vo::R<models::Order>::success(order);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
        } else {
            auto resp = vo::R<>::fail(enums::ErrorMsg::SYSTEM_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
        }
    }
    
    void getMyOrder(const HttpRequestPtr &req,
                    std::function<void(const HttpResponsePtr &)> &&callback) {
        auto shUserId = req->getCookie("shUserId");
        if (shUserId.empty()) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::COOKIE_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
            return;
        }
        
        long userId = std::stol(shUserId);
        services::OrderService orderService;
        auto orders = orderService.getMyOrder(userId);
        
        auto resp = vo::R<std::vector<models::Order>>::success(orders);
        callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
    }
    
    void getMySoldIdle(const HttpRequestPtr &req,
                       std::function<void(const HttpResponsePtr &)> &&callback) {
        auto shUserId = req->getCookie("shUserId");
        if (shUserId.empty()) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::COOKIE_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
            return;
        }
        
        long userId = std::stol(shUserId);
        services::OrderService orderService;
        auto orders = orderService.getMySoldIdle(userId);
        
        auto resp = vo::R<std::vector<models::Order>>::success(orders);
        callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
    }
};

} // namespace controllers
