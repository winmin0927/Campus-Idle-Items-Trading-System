#pragma once

#include <string>
#include <json/json.h>
#include <nlohmann/json.hpp>

namespace models {

/*
收货地址实体对象
*/
struct OrderAddress {
    // 自增主键id
    long id = 0;

    // 订单ID
    long orderId = 0;

    //  收货人姓名
    std::string consigneeName;

    //  收货人电话
    std::string consigneePhone;

    // 省份名称
    std::string provinceName;

    // 城市名称
    std::string cityName;

    // 区县名称
    std::string regionName;

    // 详细地址
    std::string detailAddress;

    // 将 OrderAddress 的 Json::Value 对象转换为 nlohmann::json 对象
    static OrderAddress VtoModel(Json::Value data) {
        OrderAddress address;
        address.id = data["id"].asInt64();
        address.orderId = data["orderId"].asInt64();
        address.consigneeName = data["consigneeName"].asString();
        address.consigneePhone = data["consigneePhone"].asString();
        address.provinceName = data["provinceName"].asString();
        address.cityName = data["cityName"].asString();
        address.regionName = data["regionName"].asString();
        address.detailAddress = data["detailAddress"].asString();
        return address;
    }
    
    // JSON序列化/反序列化支持
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(OrderAddress, id, orderId, consigneeName,
        consigneePhone, provinceName, cityName, regionName, detailAddress)
};

} // namespace models