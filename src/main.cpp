#include <drogon/drogon.h>
#include <spdlog/spdlog.h>
#include "utils/OrderTaskHandler.h"
#include "config/AppConfig.h"

using namespace drogon;

int main() {
    try {
        auto config = AppConfig::getInstance();
        spdlog::set_level(spdlog::level::info);
        spdlog::info("Starting Campus Idle Trading System Backend...");
        
        // 配置数据库连接池
        app().addDbClient(
            "mysql",
            config->getDbHost(),
            config->getDbPort(),
            config->getDbName(),
            config->getDbUser(),
            config->getDbPassword(),
            10
        );
        
        // 配置服务器监听地址和端口
        app().addListener("0.0.0.0", config->getServerPort());
        
        // 配置CORS
        app().registerPostHandlingAdvice([](const HttpRequestPtr &req, const HttpResponsePtr &resp) {
            resp->addHeader("Access-Control-Allow-Origin", "http://localhost:8081");
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
        
        // 配置静态文件路径
        app().setDocumentRoot(config->getUserFilePath());
        
        // 配置文件上传大小限制
        app().setClientMaxBodySize(10 * 1024 * 1024);
        
        // 启动延迟订单取消任务处理器
        utils::OrderTaskHandler::getInstance().start();
        
        spdlog::info("=====================项目后端启动成功============================");
        spdlog::info("Server listening on port: {}", config->getServerPort());
        spdlog::info("Database: {}@{}", config->getDbName(), config->getDbHost());
        
        // 运行应用
        app().run();
        
    } catch (const std::exception &e) {
        spdlog::error("Failed to start application: {}", e.what());
        return -1;
    }
    
    return 0;
}
