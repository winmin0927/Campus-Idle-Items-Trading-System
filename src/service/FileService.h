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
    
    bool uploadFile(const std::string& fileContent, const std::string& fileName) {
        namespace fs = std::filesystem;
        
        std::string userFilePath = config->getUserFilePath();
        fs::path fileDir(userFilePath);
        
        // 创建目录（如果不存在）
        if (!fs::exists(fileDir)) {
            if (!fs::create_directories(fileDir)) {
                return false;
            }
        }
        
        // 构建完整文件路径
        fs::path filePath = fileDir / fileName;
        
        // 如果文件已存在，删除
        if (fs::exists(filePath)) {
            if (!fs::remove(filePath)) {
                return false;
            }
        }
        
        // 写入文件
        std::ofstream outFile(filePath, std::ios::binary);
        if (!outFile) {
            return false;
        }
        
        outFile.write(fileContent.data(), fileContent.size());
        outFile.close();
        
        return outFile.good();
    }// Drogon版本：直接处理HttpFile对象
    bool uploadFile(constdrogon::HttpFile& file, const std::string& fileName) {
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
        
        // Drogon的HttpFile提供save方法
        try {
            file.save(filePath.string());
            return true;
        } catch (...) {
            return false;
        }
    }
    
private:
    AppConfig* config;
};

} // namespace services
