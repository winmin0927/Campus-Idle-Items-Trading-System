#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace models {

struct IdleItem {
    long id = 0;
    long userId = 0;
    std::string idleName;
    std::string idleDetails;
    std::string pictureList;  // JSON字符串，存储图片URL列表
    double idlePrice = 0.0;
    std::string idlePlace;
    int idleLabel = 0;
    int idleStatus = 0;  // 1-可售2-已下单 3-已售出等
    std::string releaseTime;
    
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(IdleItem, id, userId, idleName, idleDetails,
                                   pictureList, idlePrice, idlePlace, idleLabel,
                                   idleStatus, releaseTime)
};

} // namespace models
