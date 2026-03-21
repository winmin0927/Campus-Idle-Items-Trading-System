#pragma once

#include <string>
#include <json/json.h>
#include <nlohmann/json.hpp>

namespace models {

/*
管理员实体对象
*/
struct Admin {
    // 自增主键
    long id = 0;
    
    // 管理员账号
    std::string accountNumber;
    
    // 密码
    std::string adminPassword;
    
    // 管理员名字
    std::string adminName;

    // 将 Admin的Json::Value 对象转换为 Admin的nlohmann::json 对象的函数
    static Admin VtoModel(Json::Value data) {
        Admin admin;
        admin.id = data["id"].asInt64();
        admin.accountNumber = data["accountNumber"].asString();
        admin.adminPassword = data["adminPassword"].asString();
        admin.adminName = data["adminName"].asString();
        return admin;
    }
    
    // JSON序列化支持
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Admin, id, accountNumber, adminPassword, adminName)
};

} // namespace models