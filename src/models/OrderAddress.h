#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace models {

struct OrderAddress {
    long id = 0;
    long orderId = 0;
    std::string consigneeName;
    std::string consigneePhone;
    std::string provinceName;
    std::string cityName;
    std::string regionName;
    std::string detailAddress;
    
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(OrderAddress, id, orderId, consigneeName,
                                   consigneePhone, provinceName, cityName,
                                   regionName, detailAddress)
};

} // namespace models
