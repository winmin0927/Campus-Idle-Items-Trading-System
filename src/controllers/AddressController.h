#pragma once

#include <drogon/drogon.h>
#include "services/AddressService.h"
#include "vo/R.h"
#include "enums/ErrorMsg.h"

using namespace drogon;

namespace controllers {

class AddressController : public HttpController<AddressController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(AddressController::getAddress, "/address/info", Get);
    ADD_METHOD_TO(AddressController::addAddress, "/address/add", Post);
    ADD_METHOD_TO(AddressController::updateAddress, "/address/update", Post);
    ADD_METHOD_TO(AddressController::deleteAddress, "/address/delete", Post);
    METHOD_LIST_END
    
    void getAddress(const HttpRequestPtr &req,
                    std::function<void(const HttpResponsePtr &)> &&callback) {
        auto shUserId = req->getCookie("shUserId");
        if (shUserId.empty()) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::COOKIE_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
            return;
        }
        
        long userId = std::stol(shUserId);
        auto idParam = req->getOptionalParameter<long>("id");
        services::AddressService addressService;
        
        if (!idParam.has_value()) {
            // 获取用户所有地址
            auto addresses = addressService.getAddressByUser(userId);
            auto resp = vo::R<std::vector<models::Address>>::success(addresses);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
        } else {
            // 获取单个地址
            auto address = addressService.getAddressById(idParam.value(), userId);
            if (address.has_value()) {
                auto resp = vo::R<models::Address>::success(*address);
                callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
            } else {
                auto resp = vo::R<>::fail(enums::ErrorMsg::PARAM_ERROR);
                callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
            }
        }
    }
    
    void addAddress(const HttpRequestPtr &req,
                    std::function<void(const HttpResponsePtr &)> &&callback) {
        auto shUserId = req->getCookie("shUserId");
        if (shUserId.empty()) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::COOKIE_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
            return;
        }
        
        auto json = req->getJsonObject();
        models::Address address = *json;
        address.userId = std::stol(shUserId);
        
        services::AddressService addressService;
        if (addressService.addAddress(address)) {
            auto resp = vo::R<models::Address>::success(address);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
        } else {
            auto resp = vo::R<>::fail(enums::ErrorMsg::SYSTEM_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
        }
    }
    
    void updateAddress(const HttpRequestPtr &req,
                       std::function<void(const HttpResponsePtr &)> &&callback) {
        auto shUserId = req->getCookie("shUserId");
        if (shUserId.empty()) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::COOKIE_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
            return;
        }
        
        auto json = req->getJsonObject();
        models::Address address = *json;
        address.userId = std::stol(shUserId);
        
        services::AddressService addressService;
        if (addressService.updateAddress(address)) {
            auto resp = vo::R<>::success();
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
        } else {
            auto resp = vo::R<>::fail(enums::ErrorMsg::SYSTEM_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
        }
    }
    
    void deleteAddress(const HttpRequestPtr &req,
                       std::function<void(const HttpResponsePtr &)> &&callback) {
        auto shUserId = req->getCookie("shUserId");
        if (shUserId.empty()) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::COOKIE_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
            return;
        }
        
        auto json = req->getJsonObject();
        long id = (*json)["id"].as<long>();
        long userId = std::stol(shUserId);
        
        services::AddressService addressService;
        if (addressService.deleteAddress(id, userId)) {
            auto resp = vo::R<>::success();
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
        } else {
            auto resp = vo::R<>::fail(enums::ErrorMsg::SYSTEM_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
        }
    }
};

} // namespace controllers
