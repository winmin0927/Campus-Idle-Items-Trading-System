#pragma once

#include <string>
#include <json/json.h>
#include <nlohmann/json.hpp>

namespace models {

/*
留言实体对象
*/
struct Message {
    // 自增主键id
    long id = 0;

    // 发布用户ID
    long userId = 0;

    // 留言对应的闲置商品ID
    long idleId = 0;

    // 留言内容
    std::string content;

    // 留言创建时间
    std::string createTime;

    // 回复目标用户ID（可选）
    long toUser = 0;

    // 回复目标留言ID（可选）
    long toMessage = 0;

    // 将 Message 的 Json::Value 对象转换为 nlohmann::json 对象
    static Message VtoModel(Json::Value data) {
        Message message;
        message.id = data["id"].asInt64();
        message.userId = data["userId"].asInt64();
        message.idleId = data["idleId"].asInt64();
        message.content = data["content"].asString();
        message.createTime = data["createTime"].asString();
        message.toUser = data["toUser"].asInt64();
        message.toMessage = data["toMessage"].asInt64();
        return message;
    }

    // JSON序列化/反序列化支持
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Message, id, userId, idleId, content, createTime, toUser, toMessage)
};

} // namespace models