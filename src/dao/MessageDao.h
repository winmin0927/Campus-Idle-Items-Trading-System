#pragma once

#include <drogon/drogon.h>
#include <vector>
#include <optional>
#include "models/Message.h"

using namespace drogon;
using namespace drogon::orm;

namespace dao {

class MessageDao {
public:
    MessageDao() : dbClient(app().getDbClient()) {}
    
    bool deleteByPrimaryKey(long id) {
        auto result = dbClient->execSqlSync("DELETE FROM message WHERE id = ?", id);
        return result.affectedRows() > 0;
    }
    
    long insert(const models::Message& record) {
        auto result = dbClient->execSqlSync(
            "INSERT INTO message (user_id, idle_id, message_content, message_time) VALUES (?, ?, ?, ?)",
            record.userId, record.idleId, record.messageContent, record.messageTime
        );
        return result.insertId();
    }
    
    std::optional<models::Message> selectByPrimaryKey(long id) {
        auto result = dbClient->execSqlSync("SELECT * FROM message WHERE id = ?", id);
        if (result.empty()) return std::nullopt;
        return mapRowToMessage(result[0]);
    }
    
    std::vector<models::Message> getMyMessage(long userId) {
        auto result = dbClient->execSqlSync("SELECT * FROM message WHERE user_id = ?", userId);
        return mapResultToList(result);
    }
    
    std::vector<models::Message> getIdleMessage(long idleId) {
        auto result = dbClient->execSqlSync("SELECT * FROM message WHERE idle_id = ?", idleId);
        return mapResultToList(result);
    }
    
    bool updateByPrimaryKey(const models::Message& record) {
        auto result = dbClient->execSqlSync(
            "UPDATE message SET user_id = ?, idle_id = ?, message_content = ?, message_time = ? WHERE id = ?",
            record.userId, record.idleId, record.messageContent, record.messageTime, record.id
        );
        return result.affectedRows() > 0;
    }
    
private:
    DbClientPtr dbClient;
    
    models::Message mapRowToMessage(const Row& row) {
        models::Message message;
        message.id = row["id"].as<long>();
        message.userId = row["user_id"].as<long>();
        message.idleId = row["idle_id"].as<long>();
        message.messageContent = row["message_content"].as<std::string>();
        message.messageTime = row["message_time"].as<std::string>();
        return message;
    }
    
    std::vector<models::Message> mapResultToList(const Result& result) {
        std::vector<models::Message> list;
        for (auto row : result) {
            list.push_back(mapRowToMessage(row));
        }
        return list;
    }
};

} // namespace dao
