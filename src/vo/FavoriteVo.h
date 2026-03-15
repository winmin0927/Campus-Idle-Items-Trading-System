#pragma once

#include <nlohmann/json.hpp>
#include "models/Favorite.h"
#include "models/IdleItem.h"

namespace vo {

struct FavoriteVo {
    long id = 0;
    long userId = 0;
    long idleId = 0;
    std::string collectTime;
    models::IdleItem idleItem;
    
    // 从Favorite构造
    FavoriteVo() = default;
    
    FavoriteVo(const models::Favorite& fav, const models::IdleItem& item)
        : id(fav.id), userId(fav.userId), idleId(fav.idleId),
          collectTime(fav.collectTime), idleItem(item) {}
    
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(FavoriteVo, id, userId, idleId, collectTime, idleItem)
};

} // namespace vo
