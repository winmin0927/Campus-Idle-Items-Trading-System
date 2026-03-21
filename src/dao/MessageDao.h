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
            "INSERT INTO message (userId, idleId, content, createTime, toUser, toMessage) VALUES (?, ?, ?, ?, ?, ?)",
            record.userId, record.idleId, record.content, record.createTime, record.toUser, record.toMessage
        );
        return result.insertId();
    }
    
    std::optional<models::Message> selectByPrimaryKey(long id) {
        auto result = dbClient->execSqlSync("SELECT * FROM message WHERE id = ?", id);
        if (result.empty()) return std::nullopt;
        return mapRowToMessage(result[0]);
    }
    
    std::vector<models::Message> getMyMessage(long userId) {
        auto result = dbClient->execSqlSync("SELECT * FROM message WHERE userId = ?", userId);
        return mapResultToList(result);
    }
    
    std::vector<models::Message> getIdleMessage(long idleId) {
        auto result = dbClient->execSqlSync("SELECT * FROM message WHERE idleId = ?", idleId);
        return mapResultToList(result);
    }
    
    bool updateByPrimaryKey(const models::Message& record) {
        auto result = dbClient->execSqlSync(
            "UPDATE message SET userId = ?, idleId = ?, content = ?, createTime = ? WHERE id = ?",
            record.userId, record.idleId, record.content, record.createTime, record.id
        );
        return result.affectedRows() > 0;
    }
    
private:
    DbClientPtr dbClient;
    
    models::Message mapRowToMessage(const Row& row) {
        models::Message message;
        message.id = row["id"].as<long>();
        message.userId = row["userId"].as<long>();
        message.idleId = row["idleId"].as<long>();
        message.content = row["content"].as<std::string>();
        message.createTime = row["createTime"].as<std::string>();
        message.toUser = row["toUser"].as<long>();
        message.toMessage = row["toMessage"].as<long>();
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
