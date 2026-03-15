#pragma once

#include <nlohmann/json.hpp>
#include "models/Message.h"
#include "models/User.h"
#include "models/IdleItem.h"

namespace vo {

struct MessageVo {
    models::Message message;
    models::User fromUser;      // 留言发送者
    models::User toUser;        // 回复目标用户（可选）
    models::Message toMessage;  // 回复的留言（可选）
    models::IdleItem idleItem;  // 关联的闲置商品
    
    MessageVo() = default;
    
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MessageVo, message, fromUser, toUser, toMessage, idleItem)
};

} // namespace vo
