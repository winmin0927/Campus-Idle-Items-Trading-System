#pragma once

#include <string>
#include <fstream>
#include <filesystem>
#include <drogon/drogon.h>
#include "config/AppConfig.h"

namespace services {

class FileService {
public:
    FileService() : config(AppConfig::getInstance()) {}

    //========== 上传头像 ==========
    // 保存到{basePath}/avatar/A_{userId}.{ext}
    // 返回后缀名（如 "jpg"），失败返回空字符串
    std::string uploadAvatar(const drogon::HttpFile& file, long userId) {
        namespace fs = std::filesystem;

        std::string ext = getExtension(file.getFileName());
        if (ext.empty()) ext = "jpg";

        std::string avatarDir = config->getUserFilePath() + "/avatar/";
        fs::path dirPath(avatarDir);
        if (!fs::exists(dirPath)) {
            fs::create_directories(dirPath);
        }

        // 删除该用户旧头像（可能扩展名不同）
        std::string prefix = "A_" + std::to_string(userId) + ".";
        for (const auto& entry : fs::directory_iterator(dirPath)) {
            std::string filename = entry.path().filename().string();
            if (filename.rfind(prefix, 0) == 0) {
                fs::remove(entry.path());
            }
        }

        std::string fileName = "A_" + std::to_string(userId) + "." + ext;
        fs::path filePath = dirPath / fileName;

        try {
            file.save(filePath.string());
            return ext;
        } catch (...) {
            return "";
        }
    }

    // ========== 上传物品图片（单张） ==========
    // 保存到 {basePath}/idleItem/I_{idleId}{i}.{ext}
    // i 为图片序号（从0开始）
    // 返回后缀名，失败返回空字符串
    std::string uploadIdleItemImage(const drogon::HttpFile& file, long idleId, int index) {
        namespace fs = std::filesystem;

        std::string ext = getExtension(file.getFileName());
        if (ext.empty()) ext = "jpg";

        std::string itemDir = config->getUserFilePath() + "/idleItem/";
        fs::path dirPath(itemDir);
        if (!fs::exists(dirPath)) {
            fs::create_directories(dirPath);
        }

        std::string fileName = "I_" + std::to_string(idleId) + std::to_string(index) + "." + ext;
        fs::path filePath = dirPath / fileName;

        // 如果已存在则覆盖
        if (fs::exists(filePath)) {
            fs::remove(filePath);
        }

        try {
            file.save(filePath.string());
            return ext;
        } catch (...) {
            return "";
        }
    }

    // ========== 删除某个物品的所有旧图片 ==========
    void removeIdleItemImages(long idleId) {
        namespace fs = std::filesystem;
        std::string itemDir = config->getUserFilePath() + "/idleItem/";
        fs::path dirPath(itemDir);
        if (!fs::exists(dirPath)) return;

        std::string prefix = "I_" + std::to_string(idleId);
        for (const auto& entry : fs::directory_iterator(dirPath)) {
            std::string filename = entry.path().filename().string();
            if (filename.rfind(prefix, 0) == 0) {
                fs::remove(entry.path());
            }
        }
    }

    // ========== 原有通用上传（保留兼容） ==========
    bool uploadFile(const std::string& fileContent, const std::string& fileName) {
        namespace fs = std::filesystem;

        std::string userFilePath = config->getUserFilePath();
        fs::path fileDir(userFilePath);

        if (!fs::exists(fileDir)) {
            if (!fs::create_directories(fileDir)) {
                return false;
            }
        }

        fs::path filePath = fileDir / fileName;

        if (fs::exists(filePath)) {
            if (!fs::remove(filePath)) {
                return false;
            }
        }

        std::ofstream outFile(filePath, std::ios::binary);
        if (!outFile) {
            return false;
        }

        outFile.write(fileContent.data(), fileContent.size());
        outFile.close();

        return outFile.good();
    }

    bool uploadFile(const drogon::HttpFile& file, const std::string& fileName) {
        namespace fs = std::filesystem;

        std::string userFilePath = config->getUserFilePath();
        fs::path fileDir(userFilePath);

        if (!fs::exists(fileDir)) {
            if (!fs::create_directories(fileDir)) {
                return false;
            }
        }

        fs::path filePath = fileDir / fileName;

        if (fs::exists(filePath)) {
            fs::remove(filePath);
        }

        try {
            file.save(filePath.string());
            return true;
        } catch (...) {
            return false;
        }
    }

private:
    AppConfig* config;

    // 从文件名中提取扩展名（不含点号）
    std::string getExtension(const std::string& fileName) {
        auto pos = fileName.rfind('.');
        if (pos != std::string::npos && pos + 1 < fileName.size()) {
            return fileName.substr(pos + 1);
        }
        return "";
    }
};

} // namespace services