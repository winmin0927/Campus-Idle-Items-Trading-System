#pragma once

#include <drogon/drogon.h>
#include <fstream>
#include <filesystem>
#include <sstream>
#include "service/FileService.h"
#include "service/UserService.h"
#include "utils/IdFactoryUtil.h"
#include "vo/R.h"
#include "enums/ErrorMsg.h"
#include "config/AppConfig.h"

using namespace drogon;

namespace controllers {

class FileController : public HttpController<FileController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(FileController::uploadAvatar, "/file/avatar", Post);
    ADD_METHOD_TO(FileController::uploadIdleImage, "/file/idle", Post);
    ADD_METHOD_TO(FileController::getAvatar, "/avatar", Get);
    ADD_METHOD_TO(FileController::getIdleImage, "/idleImage", Get);
    METHOD_LIST_END

    // ===================== 上传头像 =====================
    void uploadAvatar(const HttpRequestPtr &req,std::function<void(const HttpResponsePtr &)> &&callback) {
        auto shUserId = req->getCookie("shUserId");
        if (shUserId.empty()) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::COOKIE_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toValue()));
            return;
        }

        long userId = std::stol(shUserId);

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
        std::string ext = fileService.uploadAvatar(file, userId);

        if (!ext.empty()) {
            // 将后缀名存入数据库的 avatar 字段
            services::UserService userService;
            models::User user;
            user.id = userId;
            user.avatar = ext;  // 只存后缀名，如 "jpg"
            userService.updateUserInfo(user);

            // 返回头像访问 URL
            auto config = AppConfig::getInstance();
            std::string avatarUrl = config->getBaseUrl() + "/avatar?userId=" + std::to_string(userId);
            auto resp = vo::R<std::string>::success(avatarUrl);
            callback(HttpResponse::newHttpJsonResponse(resp.toValue()));
        } else {
            auto resp = vo::R<>::fail(enums::ErrorMsg::SYSTEM_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toValue()));
        }
    }

    // ===================== 上传物品图片（支持多张） =====================
    void uploadIdleImage(const HttpRequestPtr &req,
                         std::function<void(const HttpResponsePtr &)> &&callback) {
        auto shUserId = req->getCookie("shUserId");
        if (shUserId.empty()) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::COOKIE_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toValue()));
            return;
        }

        MultiPartParser fileUpload;
        fileUpload.parse(req);

        // 从表单参数中获取 idleId
        auto params = fileUpload.getParameters();
        std::string idleIdStr;
        for (auto& p : params) {
            if (p.first == "idleId") {
                idleIdStr = p.second;
                break;
            }
        }

        if (idleIdStr.empty()) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::PARAM_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toValue()));
            return;
        }

        long idleId = std::stol(idleIdStr);

        auto files = fileUpload.getFiles();
        if (files.empty()) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::PARAM_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toValue()));
            return;
        }

        services::FileService fileService;

        // 先删除该物品的旧图片
        fileService.removeIdleItemImages(idleId);

        // 逐张上传，收集后缀名
        std::vector<std::string> extList;
        for (int i = 0; i < (int)files.size(); i++) {
            std::string ext = fileService.uploadIdleItemImage(files[i], idleId, i);
            if (!ext.empty()) {
                extList.push_back(ext);
            }
        }

        if (extList.empty()) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::SYSTEM_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toValue()));
            return;
        }

        // 拼接后缀列表，用逗号分隔：如 "jpg,png,jpg"
        std::string pictureList;
        for (size_t i = 0; i < extList.size(); i++) {
            if (i > 0) pictureList += ",";
            pictureList += extList[i];
        }

        // 返回 pictureList 字符串，由前端在发布物品时存入数据库
        auto resp = vo::R<std::string>::success(pictureList);
        callback(HttpResponse::newHttpJsonResponse(resp.toValue()));
    }

    // ===================== 读取头像 =====================
    void getAvatar(const HttpRequestPtr &req,
                   std::function<void(const HttpResponsePtr &)> &&callback) {
        namespace fs = std::filesystem;

        auto userIdParam = req->getOptionalParameter<std::string>("userId");
        if (!userIdParam || userIdParam->empty()) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k400BadRequest);
            callback(resp);
            return;
        }

        long userId = 0;
        try {
            userId = std::stol(*userIdParam);
        } catch (...) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k400BadRequest);
            callback(resp);
            return;
        }

        std::string avatarDir = AppConfig::getInstance()->getUserFilePath() + "/avatar/";
        fs::path avatarPath;

        // 从数据库获取用户的avatar 字段（存的是后缀名）
        services::UserService userService;
        auto userOpt = userService.getUser(userId);

        if (userOpt.has_value() && !userOpt->avatar.empty()) {
            std::string ext = userOpt->avatar;
            std::string fileName = "A_" + std::to_string(userId) + "." + ext;
            avatarPath = fs::path(avatarDir) / fileName;

            // 文件不存在则使用默认头像
            if (!fs::exists(avatarPath)) {
                avatarPath = fs::path(avatarDir) / "A_default.jpg";
            }
        } else {
            avatarPath = fs::path(avatarDir) / "A_default.jpg";
        }

        sendImageFile(avatarPath, callback);
    }

    // ===================== 读取物品图片 =====================
    // GET /idleImage?idleId=xxx→ 返回第一张图片（用于列表缩略图）
    // GET /idleImage?idleId=xxx&index=i   → 返回第i 张图片
    // GET /idleImage?idleId=xxx&all=true  → 返回所有图片URL列表（JSON）
    void getIdleImage(const HttpRequestPtr &req,
                      std::function<void(const HttpResponsePtr &)> &&callback) {
        namespace fs = std::filesystem;

        auto idleIdParam = req->getOptionalParameter<std::string>("idleId");
        if (!idleIdParam || idleIdParam->empty()) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k400BadRequest);
            callback(resp);
            return;
        }

        long idleId = 0;
        try {
            idleId = std::stol(*idleIdParam);
        } catch (...) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k400BadRequest);
            callback(resp);
            return;
        }

        // 检查是否请求所有图片的URL列表
        auto allParam = req->getOptionalParameter<std::string>("all");
        if (allParam && *allParam == "true") {
            // 返回所有图片的URL列表（JSON数组）
            returnAllIdleImageUrls(idleId, callback);
            return;
        }

        // 获取图片序号，默认为0（第一张）
        int index = 0;
        auto indexParam = req->getOptionalParameter<std::string>("index");
        if (indexParam && !indexParam->empty()) {
            try {
                index = std::stoi(*indexParam);
            } catch (...) {
                index = 0;
            }
        }

        // 从数据库获取 pictureList（后缀列表，逗号分隔）
        // 这里需要 IdleItemService，先用简单方式：直接在目录中查找文件
        std::string itemDir = AppConfig::getInstance()->getUserFilePath() + "/idleItem/";
        std::string prefix = "I_" + std::to_string(idleId) + std::to_string(index) + ".";

        fs::path imagePath;
        bool found = false;

        if (fs::exists(itemDir) && fs::is_directory(itemDir)) {
            for (const auto& entry : fs::directory_iterator(itemDir)) {
                std::string filename = entry.path().filename().string();
                if (filename.rfind(prefix, 0) == 0) {
                    imagePath = entry.path();
                    found = true;
                    break;
                }
            }
        }

        if (!found) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k404NotFound);
            callback(resp);
            return;
        }

        sendImageFile(imagePath, callback);
    }

private:
    // 返回所有物品图片的URL列表
    void returnAllIdleImageUrls(long idleId,
                                std::function<void(const HttpResponsePtr &)> &callback) {
        namespace fs = std::filesystem;

        std::string itemDir = AppConfig::getInstance()->getUserFilePath() + "/idleItem/";
        std::string basePrefix = "I_" + std::to_string(idleId);
        auto config = AppConfig::getInstance();

        // 收集所有匹配的文件，按序号排序
        std::map<int, std::string> imageMap;

        if (fs::exists(itemDir) && fs::is_directory(itemDir)) {
            for (const auto& entry : fs::directory_iterator(itemDir)) {
                std::string filename = entry.path().filename().string();
                if (filename.rfind(basePrefix, 0) == 0) {
                    // 从文件名中提取序号
                    // 文件名格式: I_{idleId}{i}.{ext}
                    std::string rest = filename.substr(basePrefix.size());
                    // rest 形如 "0.jpg", "1.png" 等
                    auto dotPos = rest.find('.');
                    if (dotPos != std::string::npos) {
                        try {
                            int idx = std::stoi(rest.substr(0, dotPos));
                            std::string url = config->getBaseUrl() + "/idleImage?idleId=" +std::to_string(idleId) + "&index=" + std::to_string(idx);
                            imageMap[idx] = url;
                        } catch (...) {}
                    }
                }
            }
        }

        // 转为JSON数组
        Json::Value arr(Json::arrayValue);
        for (auto& [idx, url] : imageMap) {
            arr.append(url);
        }

        auto resp = vo::R<Json::Value>::success(arr);
        callback(HttpResponse::newHttpJsonResponse(resp.toValue()));
    }

    // 通用：读取图片文件并返回
    void sendImageFile(const std::filesystem::path& imagePath,
                       std::function<void(const HttpResponsePtr &)> &callback) {
        namespace fs = std::filesystem;

        if (!fs::exists(imagePath)) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k404NotFound);
            callback(resp);
            return;
        }

        std::ifstream fileStream(imagePath, std::ios::binary);
        if (!fileStream) {
            auto resp = HttpResponse::newHttpResponse();
            resp->setStatusCode(k500InternalServerError);
            callback(resp);
            return;
        }

        std::string content((std::istreambuf_iterator<char>(fileStream)),std::istreambuf_iterator<char>());
        fileStream.close();

        auto resp = HttpResponse::newHttpResponse();
        resp->setBody(content);

        std::string ext = imagePath.extension().string();
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