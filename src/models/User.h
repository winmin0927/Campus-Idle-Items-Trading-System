#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace models {

struct User {
    long id = 0;
    std::string accountNumber;
    std::string userPassword;
    std::string nickname;
    std::string avatar;
    std::string signInTime;
    int userStatus = 0;  // 0-正常 1-封禁
    
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(User, id, accountNumber, userPassword,
                                   nickname, avatar, signInTime, userStatus)
};

} // namespace models
