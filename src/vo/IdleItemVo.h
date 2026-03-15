#pragma once

#include <nlohmann/json.hpp>
#include "models/IdleItem.h"
#include "models/User.h"

namespace vo {

struct IdleItemVo {
    models::IdleItem idleItem;
    models::User user;
    
    IdleItemVo() = default;
    IdleItemVo(const models::IdleItem& item, const models::User& u)
        : idleItem(item), user(u) {}
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(IdleItemVo, idleItem, user)
};

} // namespace vo
