#include <drogon/drogon.h>
#include <spdlog/spdlog.h>
#include "utils/OrderTaskHandler.h"
#include "config/AppConfig.h"
#include "controllers/AddressController.h"
#include "controllers/AdminController.h"
#include "controllers/FavoriteController.h"
#include "controllers/FileController.h"
#include "controllers/IdleItemController.h"
#include "controllers/MessageController.h"
#include "controllers/OrderAddressController.h"
#include "controllers/OrderController.h"
#include "controllers/UserController.h"

using namespace drogon;

int main() {
    try {
        auto config = AppConfig::getInstance();
        spdlog::set_level(spdlog::level::info);
        spdlog::info("Starting Campus Idle Item Trading System Backend...");
        
        // 构造 MySQL 配置对象
        orm::MysqlConfig mysqlConfig;
        mysqlConfig.host = config->getDbHost();
        mysqlConfig.port = config->getDbPort();
        mysqlConfig.databaseName = config->getDbName();
        mysqlConfig.username = config->getDbUser();
        mysqlConfig.password = config->getDbPassword();
        mysqlConfig.connectionNumber = 10;          // 连接池大小
        mysqlConfig.name = "default";               // 可选的连接名称
        mysqlConfig.isFast = false;                  // 是否快速连接（根据需求）
        mysqlConfig.characterSet = "utf8mb4";          // 字符集
        mysqlConfig.timeout = 10.0;                   // 超时时间（秒）
        
        // 将 MysqlConfig 隐式转换为 DbConfig (std::variant) 并添加
        app().addDbClient(mysqlConfig);
        
        app().addListener("0.0.0.0", config->getServerPort());
        
        // 配置CORS（保持不变）
        app().registerPostHandlingAdvice([](const HttpRequestPtr &req, const HttpResponsePtr &resp) {
            resp->addHeader("Access-Control-Allow-Origin", req->getHeader("Origin"));
            resp->addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
            resp->addHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");
            resp->addHeader("Access-Control-Allow-Credentials", "true");
        });
        
        // 处理OPTIONS预检请求
        app().registerPreRoutingAdvice([](const HttpRequestPtr &req, AdviceCallback &&callback, AdviceChainCallback &&chainCallback) {
            if (req->method() == Options) {
                auto resp = HttpResponse::newHttpResponse();
                resp->setStatusCode(k200OK);
                callback(resp);
                return;
            }
            chainCallback();
        });
        
        app().setDocumentRoot(config->getUserFilePath());
        app().setClientMaxBodySize(10 * 1024 * 1024);
        
        // 启动延迟订单取消任务处理器
        myutils::OrderTaskHandler::getInstance().start();
        
        spdlog::info("=====================项目后端启动成功============================");
        spdlog::info("Server listening on port: {}", config->getServerPort());
        spdlog::info("Database: {}@{}", config->getDbName(), config->getDbHost());
        
        app().run();
    } catch (const std::exception &e) {
        spdlog::error("Failed to start application: {}", e.what());
        return -1;
    }
    
    return 0;
}