#pragma once

#include <string>
#include <json/json.h>
#include <nlohmann/json.hpp>
#include "IdleItem.h"

namespace models {

/*
订单实体对象
*/
struct Order {
    // 自增主键id
    long id = 0;

    // 订单编号
    std::string orderNumber;

    // 购买用户ID
    long userId = 0;

    // 购买的闲置商品ID
    long idleId = 0;

    // 收货地址ID
    long addressId = 0;

    // 订单状态
    int orderStatus = 0;      // 0-待支付 1-已支付 2-已发货 3-已完成 4-已取消

    // 支付状态
    int paymentStatus = 0;    // 0-未支付 1-已支付

    // 创建时间
    std::string createTime;

    // 支付时间
    std::string paymentTime;
    
    // 关联的闲置商品（用于查询结果）
    IdleItem idleItem;

    // 将 Order 的 Json::Value 对象转换为 nlohmann::json 对象
    static Order VtoModel(Json::Value data) {
        Order order;
        order.id = data["id"].asInt64();
        order.orderNumber = data["orderNumber"].asString();
        order.userId = data["userId"].asInt64();
        order.idleId = data["idleId"].asInt64();
        order.addressId = data["addressId"].asInt64();
        order.orderStatus = data["orderStatus"].asInt();
        order.paymentStatus = data["paymentStatus"].asInt();
        order.createTime = data["createTime"].asString();
        order.paymentTime = data["paymentTime"].asString();
        // 处理嵌套的 IdleItem 对象，直接调用其自身的 VtoModel 函数
        if (data.isMember("idleItem") && data["idleItem"].isObject()) {
            order.idleItem = IdleItem().VtoModel(data["idleItem"]);
        } else {
            order.idleItem = IdleItem(); // 如果没有提供 idleItem 数据，则使用默认构造的对象
        }
        return order;
    }
    
    // JSON序列化/反序列化支持
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Order, id, orderNumber, userId, idleId,
        addressId, orderStatus, paymentStatus, createTime, paymentTime, idleItem)
};

} // namespace models