#pragma once

#include <drogon/drogon.h>
#include <fstream>
#include <filesystem>
#include "services/FileService.h"
#include "utils/IdFactoryUtil.h"
#include "vo/R.h"
#include "enums/ErrorMsg.h"
#include "config/AppConfig.h"

using namespace drogon;

namespace controllers {

class FileController : public HttpController<FileController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(FileController::uploadFile, "/file", Post);
    ADD_METHOD_TO(FileController::getImage, "/image", Get);
    METHOD_LIST_END
    void uploadFile(const HttpRequestPtr &req,std::function<void(const HttpResponsePtr &)> &&callback) {
        MultiPartParser fileUpload;
        fileUpload.parse(req);
        
        auto files = fileUpload.getFiles();
        if (files.empty()) {
            auto resp = vo::R<>::fail(enums::ErrorMsg::PARAM_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
            return;
        }
        
        auto& file = files[0];
        std::string originalName = file.getFileName();
        std::string uuid = "file" + utils::IdFactoryUtil::getFileId();
        std::string fileName = uuid + originalName;
        
        services::FileService fileService;
        if (fileService.uploadFile(file, fileName)) {
            auto config = AppConfig::getInstance();
            std::string imageUrl = config->getBaseUrl() + "/image?imageName=" + fileName;
            auto resp = vo::R<std::string>::success(imageUrl);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
        } else {
            auto resp = vo::R<>::fail(enums::ErrorMsg::SYSTEM_ERROR);
            callback(HttpResponse::newHttpJsonResponse(resp.toJson()));
        }
    }
    
    void getImage(const HttpRequestPtr &req,
                  std::function<void(const HttpResponsePtr &)> &&callback,
                  const std::string &imageName) {
        namespace fs = std::filesystem;
        
        auto config = AppConfig::getInstance();
        fs::path fileDir(config->getUserFilePath());
        fs::path imagePath = fileDir / imageName;
        
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
        
        std::string content((std::istreambuf_iterator<char>(fileStream)),
                           std::istreambuf_iterator<char>());
        fileStream.close();
        
        auto resp = HttpResponse::newHttpResponse();
        resp->setBody(content);
        
        // 设置Content-Type
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
