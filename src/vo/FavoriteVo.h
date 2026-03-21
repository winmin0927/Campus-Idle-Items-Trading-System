#pragma once

#include <nlohmann/json.hpp>
#include "models/Favorite.h"
#include "models/IdleItem.h"

namespace vo {

/*
收藏夹视图对象，包含收藏信息和对应的闲置商品信息
*/
struct FavoriteVo {
    // 收藏信息id
    long id = 0;

    // 用户id
    long userId = 0;

    // 闲置商品id
    long idleId = 0;

    // 收藏时间
    std::string collectTime;

    // 对应的闲置商品信息
    models::IdleItem idleItem;
    
    // 从Favorite构造
    FavoriteVo() = default;
    
    FavoriteVo(const models::Favorite& fav, const models::IdleItem& item)
        : id(fav.id), userId(fav.userId), idleId(fav.idleId),
          collectTime(fav.collectTime), idleItem(item) {}
    
    // 定义JSON序列化和反序列化的规则
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(FavoriteVo, id, userId, idleId, collectTime, idleItem)
};

} // namespace vo
