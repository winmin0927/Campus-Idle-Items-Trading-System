#pragma once

#include <drogon/drogon.h>
#include <chrono>
#include "services/FavoriteService.h"
#include "vo/R.h"
#include "enums/ErrorMsg.h"

using namespace drogon;

namespace controllers {

class FavoriteController : public HttpController<FavoriteController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(FavoriteController::addFavorite, "/favorite/add", Post);
    ADD_METHOD_TO(FavoriteController::deleteFavorite, "/favorite/delete", Get);
    ADD_METHOD_TO(FavoriteController::checkFavorite, "/favorite/check", Get);
    ADD_METHOD_TO(FavoriteController::getMyFavorite, "/favorite/my", Get);
    METHOD_LIST_END
    
    void addFavorite(const HttpRequestPtr &req,
                     std::function<void(const HttpResponsePtr &)> &&callback) {
        auto shUserId = req->getCookie("shUserId");
        if (shUserId.empty()) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::COOKIE_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
            return;
        }
        
        auto json = req->getJsonObject();
        models::Favorite favorite;
        favorite.userId = std::stol(shUserId);
        favorite.idleId = (*json)["idleId"].as<long>();
        
        // 设置当前时间
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        char buffer[64];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&time_t));
        favorite.collectTime = buffer;
        
        services::FavoriteService favoriteService;
        if (favoriteService.addFavorite(favorite)) {
            auto resp = vo::R<long>::success(favorite.id);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
        } else {
            auto resp = vo::R<>::fail(enums::ErrorMsg::SYSTEM_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
        }
    }
    
    void deleteFavorite(const HttpRequestPtr &req,
                        std::function<void(const HttpResponsePtr &)> &&callback,
                        long id) {
        auto shUserId = req->getCookie("shUserId");
        if (shUserId.empty()) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::COOKIE_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
            return;
        }
        
        services::FavoriteService favoriteService;
        if (favoriteService.deleteFavorite(id)) {
            auto resp = vo::R<>::success();
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
        } else {
            auto resp = vo::R<>::fail(enums::ErrorMsg::SYSTEM_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
        }
    }
    
    void checkFavorite(const HttpRequestPtr &req,
                       std::function<void(const HttpResponsePtr &)> &&callback,
                       long idleId) {
        auto shUserId = req->getCookie("shUserId");
        if (shUserId.empty()) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::COOKIE_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
            return;
        }
        
        long userId = std::stol(shUserId);
        services::FavoriteService favoriteService;
        int result = favoriteService.isFavorite(userId, idleId);
        
        auto resp = vo::R<int>::success(result);
        callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
    }
    
    void getMyFavorite(const HttpRequestPtr &req,
                       std::function<void(const HttpResponsePtr &)> &&callback) {
        auto shUserId = req->getCookie("shUserId");
        if (shUserId.empty()) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::COOKIE_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
            return;
        }
        
        long userId = std::stol(shUserId);
        services::FavoriteService favoriteService;
        auto favorites = favoriteService.getAllFavorite(userId);
        
        auto resp = vo::R<std::vector<vo::FavoriteVo>>::success(favorites);
        callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
    }
};

} // namespace controllers
