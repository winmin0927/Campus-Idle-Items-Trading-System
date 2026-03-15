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
    
    // 根据主键删除
    bool deleteByPrimaryKey(long id) {
        auto result = dbClient->execSqlSync("DELETE FROM favorite WHERE id = ?", id);
        return result.affectedRows() > 0;
    }
    
    // 插入记录
    long insert(const models::Favorite& record) {
        auto result = dbClient->execSqlSync(
            "INSERT INTO favorite (user_id, idle_id, collect_time) VALUES (?, ?, ?)",
            record.userId, record.idleId, record.collectTime
        );
        return result.insertId();
    }
    
    // 根据主键查询
    std::optional<models::Favorite> selectByPrimaryKey(long id) {
        auto result = dbClient->execSqlSync("SELECT * FROM favorite WHERE id = ?", id);
        if (result.empty()) return std::nullopt;
        
        auto row = result[0];
        models::Favorite favorite;
        favorite.id = row["id"].as<long>();
        favorite.userId = row["user_id"].as<long>();
        favorite.idleId = row["idle_id"].as<long>();
        favorite.collectTime = row["collect_time"].as<std::string>();
        return favorite;
    }
    
    // 获取用户的收藏列表
    std::vector<models::Favorite> getMyFavorite(long userId) {
        auto result = dbClient->execSqlSync("SELECT * FROM favorite WHERE user_id = ?", userId);
        std::vector<models::Favorite> list;
        
        for (auto row : result) {
            models::Favorite favorite;
            favorite.id = row["id"].as<long>();
            favorite.userId = row["user_id"].as<long>();
            favorite.idleId = row["idle_id"].as<long>();
            favorite.collectTime = row["collect_time"].as<std::string>();
            list.push_back(favorite);
        }
        return list;
    }
    
    // 检查是否已收藏
    int checkFavorite(long userId, long idleId) {
        auto result = dbClient->execSqlSync(
            "SELECT COUNT(*) as count FROM favorite WHERE user_id = ? AND idle_id = ?",
            userId, idleId
        );
        return result[0]["count"].as<int>();
    }
    
    // 更新记录
    bool updateByPrimaryKey(const models::Favorite& record) {
        auto result = dbClient->execSqlSync(
            "UPDATE favorite SET user_id = ?, idle_id = ?, collect_time = ? WHERE id = ?",
            record.userId, record.idleId, record.collectTime, record.id
        );
        return result.affectedRows() > 0;
    }
    
private:
    DbClientPtr dbClient;
};

} // namespace dao
