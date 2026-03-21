#pragma once

#include <drogon/drogon.h>
#include<vector>
#include <optional>
#include "models/Favorite.h"

using namespace drogon;
using namespace drogon::orm;

namespace dao {

class FavoriteDao {
public:
    FavoriteDao() : dbClient(app().getDbClient()) {}
    
    bool deleteByPrimaryKey(long id) {
        auto result = dbClient->execSqlSync("DELETE FROM favorite WHERE id = ?", id);
        return result.affectedRows() > 0;
    }
    
    long insert(const models::Favorite& record) {
        auto result = dbClient->execSqlSync(
            "INSERT INTO favorite (userId, idleId, collectTime) VALUES (?, ?, ?)",
            record.userId, record.idleId, record.collectTime
        );
        return result.insertId();
    }
    
    std::optional<models::Favorite> selectByPrimaryKey(long id) {
        auto result = dbClient->execSqlSync("SELECT * FROM favorite WHERE id = ?", id);
        if (result.empty()) return std::nullopt;
        
        auto row = result[0];
        models::Favorite favorite;
        favorite.id = row["id"].as<long>();
        favorite.userId = row["userId"].as<long>();
        favorite.idleId = row["idleId"].as<long>();
        favorite.collectTime = row["collectTime"].as<std::string>();
        return favorite;
    }
    
    std::vector<models::Favorite> getMyFavorite(long userId) {
        auto result = dbClient->execSqlSync("SELECT * FROM favorite WHERE userId = ?", userId);
        std::vector<models::Favorite> list;
        
        for (auto row : result) {
            models::Favorite favorite;
            favorite.id = row["id"].as<long>();
            favorite.userId = row["userId"].as<long>();
            favorite.idleId = row["idleId"].as<long>();
            favorite.collectTime = row["collectTime"].as<std::string>();
            list.push_back(favorite);
        }
        return list;
    }
    
    int checkFavorite(long userId, long idleId) {
        auto result = dbClient->execSqlSync(
            "SELECT COUNT(*) as count FROM favorite WHERE userId = ? AND idleId = ?",
            userId, idleId
        );
        return result[0]["count"].as<int>();
    }
    
    bool updateByPrimaryKey(const models::Favorite& record) {
        auto result = dbClient->execSqlSync(
            "UPDATE favorite SET userId = ?, idleId = ?, collectTime = ? WHERE id = ?",
            record.userId, record.idleId, record.collectTime, record.id
        );
        return result.affectedRows() > 0;
    }
    
private:
    DbClientPtr dbClient;
};

} // namespace dao