#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace models {

/**
 * 管理员实体对象
 */
struct Admin {
    /**
     * 自增主键
     */
    long id = 0;
    
    /**
     * 管理员账号
     */
    std::string accountNumber;
    
    /**
     * 密码
     */
    std::string adminPassword;
    
    /**
     * 管理员名字
     */
    std::string adminName;
    
    // JSON序列化支持
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Admin, id, accountNumber, adminPassword, adminName)
};

} // namespace models

