#pragma once

#include <drogon/drogon.h>
#include <fstream>
#include <filesystem>
#include "service/FileService.h"
#include "utils/IdFactoryUtil.h"
#include "vo/R.h"
#include "enums/ErrorMsg.h"
#include "config/AppConfig.h"
#include "service/UserService.h"  // 用于更新头像

using namespace drogon;

namespace controllers {

class FileController : public HttpController<FileController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(FileController::uploadFile, "/file", Post);  // 统一上传入口
    ADD_METHOD_TO(FileController::getAvatar, "/image/avatar/{userId}", Get);
    ADD_METHOD_TO(FileController::getIdleItemImage, "/image/idleItem/{itemId}/{index}", Get);
    ADD_METHOD_TO(FileController::getTempImage, "/image/temp/{filename}", Get);
    METHOD_LIST_END

    void uploadFile(const HttpRequestPtr &req,
                    std::function<void(const HttpResponsePtr &)> &&callback) {
        auto type = req->getParameter("type");
        if (type.empty()) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::PARAM_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toValue()));
            return;
        }

        MultiPartParser fileUpload;
        fileUpload.parse(req);
        auto files = fileUpload.getFiles();
        if (files.empty()) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::PARAM_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toValue()));
            return;
        }
        auto& file = files[0];

        services::FileService fileService;

        if (type == "avatar") {
            auto shUserId = req->getCookie("shUserId");
            if (shUserId.empty()) {
                auto resp = vo::R<>::fail(enums::ErrorMsg::COOKIE_ERROR);
                callback(HttpResponse::newHttpJsonResponse(resp.toValue()));
                return;
            }
            long userId = std::stol(shUserId);
            std::string avatarUrl = fileService.uploadAvatar(file, userId);
            if (avatarUrl.empty()) {
                auto resp = vo::R<>::fail(enums::ErrorMsg::SYSTEM_ERROR);
                callback(HttpResponse::newHttpJsonResponse(resp.toValue()));
                return;
            }
            // 更新用户头像字段
            services::UserService userService;
            models::User user;
            user.id = userId;
            user.avatar = avatarUrl;
            userService.updateUserInfo(user);  // 假设有该方法只更新头像

            auto resp = vo::R<std::string>::success(avatarUrl);
            callback(HttpResponse::newHttpJsonResponse(resp.toValue()));

        } else if (type == "idleItem") {
            // 上传临时文件
            std::string tempName = fileService.uploadTempFile(file);
            if (tempName.empty()) {
                auto resp = vo::R<>::fail(enums::ErrorMsg::SYSTEM_ERROR);
                callback(HttpResponse::newHttpJsonResponse(resp.toValue()));
                return;
            }
            auto config = AppConfig::getInstance();
            std::string tempUrl = config->getBaseUrl() + "/image/temp/" + tempName;
            auto resp = vo::R<std::string>::success(tempUrl);
            callback(HttpResponse::newHttpJsonResponse(resp.toValue()));
        } else {
            auto resp = vo::R<>::fail(enums::ErrorMsg::PARAM_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toValue()));
        }
    }

    void getAvatar(const HttpRequestPtr &req,
                   std::function<void(const HttpResponsePtr &)> &&callback,
                   long userId) {
        services::FileService fileService;
        std::string path = fileService.getAvatarPath(userId);
        if (path.empty()) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k404NotFound);
            callback(resp);
            return;
        }
        sendFile(path, callback);
    }

    void getIdleItemImage(const HttpRequestPtr &req,
                          std::function<void(const HttpResponsePtr &)> &&callback,
                          long itemId,
                          int index) {
        services::FileService fileService;
        std::string path = fileService.getIdleItemImagePath(itemId, index);
        if (path.empty()) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k404NotFound);
            callback(resp);
            return;
        }
        sendFile(path, callback);
    }

    void getTempImage(const HttpRequestPtr &req,
                      std::function<void(const HttpResponsePtr &)> &&callback,
                      const std::string &filename) {
        services::FileService fileService;
        std::string path = fileService.getTempFilePath(filename);
        if (path.empty()) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k404NotFound);
            callback(resp);
            return;
        }
        sendFile(path, callback);
    }

private:
    void sendFile(const std::string& filePath, std::function<void(const HttpResponsePtr &)> &callback) {
        namespace fs = std::filesystem;
        std::ifstream fileStream(filePath, std::ios::binary);
        if (!fileStream) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k500InternalServerError);
            callback(resp);
            return;
        }
        std::string content((std::istreambuf_iterator<char>(fileStream)),
                            std::istreambuf_iterator<char>());
        fileStream.close();

        auto resp = HttpResponse::newHttpResponse();
        resp->setBody(content);

        std::string ext = fs::path(filePath).extension().string();
        if (ext == ".jpg" || ext == ".jpeg") {
            resp->setContentTypeCode(CT_IMAGE_JPG);
        } else if (ext == ".png") {
            resp->setContentTypeCode(CT_IMAGE_PNG);
        } else if (ext == ".gif") {
            resp->setContentTypeCode(CT_IMAGE_GIF);
        } else {
            resp->setContentTypeCode(CT_APPLICATION_OCTET_STREAM);
        }
        callback(resp);
    }
};

} // namespace controllers
