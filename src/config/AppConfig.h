#pragma once

#include <string>
#include <yaml-cpp/yaml.h>

class AppConfig {
public:
    static AppConfig* getInstance() {
        static AppConfig instance;
        return &instance;
    }
    
    std::string getDbHost() const { return dbHost; }
    int getDbPort() const { return dbPort; }
    std::string getDbName() const { return dbName; }
    std::string getDbUser() const { return dbUser; }
    std::string getDbPassword() const { return dbPassword; }
    int getServerPort() const { return serverPort; }
    std::string getUserFilePath() const { return userFilePath; }
    std::string getBaseUrl() const { return baseUrl; }
    
private:
    AppConfig() { loadConfig(); }
    
    void loadConfig() {
        try {
            YAML::Node config = YAML::LoadFile("config/application.yml");
            
            dbHost = config["database"]["host"].as<std::string>("localhost");
            dbPort = config["database"]["port"].as<int>(3306);
            dbName = config["database"]["name"].as<std::string>("campus_trade");
            dbUser = config["database"]["user"].as<std::string>("root");
            dbPassword = config["database"]["password"].as<std::string>("");
            serverPort = config["server"]["port"].as<int>(8080);
            userFilePath = config["file"]["userFilePath"].as<std::string>("./uploads");
            baseUrl = config["server"]["baseUrl"].as<std::string>("http://localhost:8080");
            
        } catch (const std::exception &e) {
            dbHost = "localhost";
            dbPort = 3306;
            dbName = "campus_trade";
            dbUser = "root";
            dbPassword = "";
            serverPort = 8080;
            userFilePath = "./uploads";
            baseUrl = "http://localhost:8080";
        }
    }
    
    std::string dbHost;
    int dbPort;
    std::string dbName;
    std::string dbUser;
    std::string dbPassword;
    int serverPort;
    std::string userFilePath;
    std::string baseUrl;
};
