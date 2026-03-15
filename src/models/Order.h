#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include "IdleItem.h"

namespace models {

struct Order {
    long id = 0;
    std::string orderNumber;
    long userId = 0;
    long idleId = 0;
    long addressId = 0;
    int orderStatus = 0;      // 0-待支付 1-已支付 2-已发货 3-已完成 4-已取消
    int paymentStatus = 0;    // 0-未支付 1-已支付
    std::string createTime;
    std::string paymentTime;
    
    // 关联的闲置商品（用于查询结果）
    IdleItem idleItem;
    
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Order, id, orderNumber, userId, idleId,
                                   addressId, orderStatus, paymentStatus,
                                   createTime, paymentTime, idleItem)
};

} // namespace models
