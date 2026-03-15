#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace models {

struct Message {
    long id = 0;
    long userId = 0;
    long idleId = 0;
    std::string content;
    std::string createTime;
    long toUser = 0;      // 回复目标用户ID（可选）
    long toMessage = 0;   // 回复的留言ID（可选）
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Message, id, userId, idleId, content, createTime, toUser, toMessage)
};

} // namespace models
