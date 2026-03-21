#pragma once

#include <string>
#include <json/json.h>
#include <nlohmann/json.hpp>

namespace models {

/*
地址相关实体对象
*/
struct Address {
    // 自增主键
    long id = 0;
    
    // 收货人姓名
    std::string consigneeName;
    
    // 收货人手机号
    std::string consigneePhone;
    
    // 省
    std::string provinceName;
    
    // 市
    std::string cityName;
    
    // 区
    std::string regionName;
    
    // 详细地址
    std::string detailAddress;
    
    // 是否默认地址
    bool defaultFlag = false;
    
    // 用户主键id
    long userId = 0;

    // 将 Address 的 Json::Value 转换为对象
    static Address VtoModel(Json::Value data) {
        Address address;
        address.id = data["id"].asInt64();
        address.consigneeName = data["consigneeName"].asString();
        address.consigneePhone = data["consigneePhone"].asString();
        address.provinceName = data["provinceName"].asString();
        address.cityName = data["cityName"].asString();
        address.regionName = data["regionName"].asString();
        address.detailAddress = data["detailAddress"].asString();
        address.defaultFlag = data["defaultFlag"].asBool();
        address.userId = data["userId"].asInt64();
        return address;
    }
    
    // JSON序列化支持
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Address, id, consigneeName, consigneePhone, 
        provinceName, cityName, regionName, detailAddress, defaultFlag, userId)
};

} // namespace models