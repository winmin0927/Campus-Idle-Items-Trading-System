#pragma once

#include <drogon/drogon.h>
#include "services/OrderAddressService.h"
#include "vo/R.h"
#include "enums/ErrorMsg.h"

using namespace drogon;

namespace controllers {

class OrderAddressController : public HttpController<OrderAddressController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(OrderAddressController::addOrderAddress, "/order-address/add", Post);
    ADD_METHOD_TO(OrderAddressController::updateOrderAddress, "/order-address/update", Post);
    ADD_METHOD_TO(OrderAddressController::getOrderAddress, "/order-address/info", Get);
    METHOD_LIST_END
    
    void addOrderAddress(const HttpRequestPtr &req,
                         std::function<void(const HttpResponsePtr &)> &&callback) {
        auto shUserId = req->getCookie("shUserId");
        if (shUserId.empty()) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::COOKIE_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
            return;
        }
        
        auto json = req->getJsonObject();
        models::OrderAddress orderAddress = *json;
        
        services::OrderAddressService orderAddressService;
        bool success = orderAddressService.addOrderAddress(orderAddress);
        
        auto resp = vo::R<bool>::success(success);
        callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
    }
    
    void updateOrderAddress(const HttpRequestPtr &req,
                            std::function<void(const HttpResponsePtr &)> &&callback) {
        auto shUserId = req->getCookie("shUserId");
        if (shUserId.empty()) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::COOKIE_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
            return;
        }
        
        auto json = req->getJsonObject();
        models::OrderAddress orderAddress = *json;
        
        services::OrderAddressService orderAddressService;
        if (orderAddressService.updateOrderAddress(orderAddress)) {
            auto resp = vo::R<models::OrderAddress>::success(orderAddress);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
        } else {
            auto resp = vo::R<>::fail(enums::ErrorMsg::SYSTEM_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
        }
    }
    
    void getOrderAddress(const HttpRequestPtr &req,
                         std::function<void(const HttpResponsePtr &)> &&callback,
                         long orderId) {
        auto shUserId = req->getCookie("shUserId");
        if (shUserId.empty()) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::COOKIE_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
            return;
        }
        
        services::OrderAddressService orderAddressService;
        auto orderAddress = orderAddressService.getOrderAddress(orderId);
        
        if (orderAddress.has_value()) {
            auto resp = vo::R<models::OrderAddress>::success(*orderAddress);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
        } else {
            auto resp = vo::R<>::fail(enums::ErrorMsg::PARAM_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
        }
    }
};

} // namespace controllers
