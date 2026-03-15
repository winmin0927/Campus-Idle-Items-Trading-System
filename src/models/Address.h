#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace models {

/**
 * 地址相关实体对象
 */
struct Address {
    /**
     * 自增主键
     */
    long id = 0;
    
    /**
     * 收货人姓名
     */
    std::string consigneeName;
    
    /**
     * 收货人手机号
     */
    std::string consigneePhone;
    
    /**
     * 省
     */
    std::string provinceName;
    
    /**
     * 市
     */
    std::string cityName;
    
    /**
     * 区
     */
    std::string regionName;
    
    /**
     * 详细地址
     */
    std::string detailAddress;
    
    /**
     * 是否默认地址
     */
    bool defaultFlag = false;
    
    /**
     * 用户主键id
     */
    long userId = 0;
    
    // JSON序列化支持
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Address, id, consigneeName, consigneePhone, 
                                   provinceName, cityName, regionName, 
                                   detailAddress, defaultFlag, userId)
};

} // namespace models

