#pragma once

#include <string>
#include <json/json.h>
#include <nlohmann/json.hpp>

namespace models {

/*
收藏相关实体对象
*/
struct Favorite {
    // 自增主键id
    long id = 0;

    // 用户主键id
    long userId = 0;

    // 闲置商品id
    long idleId = 0;

    // 收藏时间
    std::string collectTime;

    // 将 Favorite 的 Json::Value 对象转换为 nlohmann::json 对象
    static Favorite VtoModel(Json::Value data) {
        Favorite favorite;
        favorite.id = data["id"].asInt64();
        favorite.userId = data["userId"].asInt64();
        favorite.idleId = data["idleId"].asInt64();
        favorite.collectTime = data["collectTime"].asString();
        return favorite;
    }

    // 定义JSON序列化和反序列化的规则
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Favorite, id, userId, idleId, collectTime)
};

} // namespace models