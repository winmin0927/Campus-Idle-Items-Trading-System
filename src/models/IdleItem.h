#pragma once

#include <string>
#include <json/json.h>
#include <nlohmann/json.hpp>

namespace models {

/*
闲置商品实体对象
*/
struct IdleItem {
    // 自增主键id
    long id = 0;

    // 发布用户ID
    long userId = 0;

    // 商品名称
    std::string idleName;

    // 商品详情
    std::string idleDetails;

    // 图片列表
    std::string pictureList;  // JSON字符串，存储图片URL列表

    // 价格
    double idlePrice = 0.0;

    // 发货地区
    std::string idlePlace;

    // 分类标签
    int idleLabel = 0;

    // 商品状态
    int idleStatus = 0;  // 1-可售 2-已下单 3-已售出等

    // 发布时间
    std::string releaseTime;

    // 将 IdleItem 的 Json::Value 对象转换为 nlohmann::json 对象
    static IdleItem VtoModel(Json::Value data) {
        IdleItem idleItem;
        idleItem.id = data["id"].asInt64();
        idleItem.userId = data["userId"].asInt64();
        idleItem.idleName = data["idleName"].asString();
        idleItem.idleDetails = data["idleDetails"].asString();
        idleItem.pictureList = data["pictureList"].asString();
        idleItem.idlePrice = data["idlePrice"].asDouble();
        idleItem.idlePlace = data["idlePlace"].asString();
        idleItem.idleLabel = data["idleLabel"].asInt();
        idleItem.idleStatus = data["idleStatus"].asInt();
        idleItem.releaseTime = data["releaseTime"].asString();
        return idleItem;
    }
    
    // JSON序列化/反序列化支持
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(IdleItem, id, userId, idleName, idleDetails,
        pictureList, idlePrice, idlePlace, idleLabel, idleStatus, releaseTime)
};

} // namespace models