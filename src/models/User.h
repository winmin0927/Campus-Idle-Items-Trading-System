#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include <json/json.h>

namespace models {

/*
用户实体对象
*/
struct User {
    // 自增主键id
    long id = 0;

    // 账户编号
    std::string accountNumber;

    // 用户密码
    std::string userPassword;

    // 昵称
    std::string nickname;

    // 头像URL
    std::string avatar;

    // 最近一次登录时间
    std::string signInTime;

    // 用户状态
    int userStatus = 0;  // 0-正常 1-封禁

    // 将 User 的 Json::Value 对象转换为 nlohmann::json 对象
    static User VtoModel(Json::Value data) {
        User user;
        user.id = data["id"].asInt64();
        user.accountNumber = data["accountNumber"].asString();
        user.userPassword = data["userPassword"].asString();
        user.nickname = data["nickname"].asString();
        user.avatar = data["avatar"].asString();
        user.signInTime = data["signInTime"].asString();
        user.userStatus = data["userStatus"].asInt();
        return user;
    }
    
    // JSON序列化/反序列化支持
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(User, id, accountNumber, userPassword,
        nickname, avatar, signInTime, userStatus)
};

} // namespace models