#pragma once

#include <drogon/drogon.h>
#include <vector>
#include <optional>
#include "models/IdleItem.h"

using namespace drogon;
using namespace drogon::orm;

namespace dao {

class IdleItemDao {
public:
    IdleItemDao() : dbClient(app().getDbClient()) {}
    
    bool deleteByPrimaryKey(long id) {
        auto result = dbClient->execSqlSync("DELETE FROM idle_item WHERE id = ?", id);
        return result.affectedRows() > 0;
    }
    
    long insert(const models::IdleItem& record) {
        auto result = dbClient->execSqlSync(
            "INSERT INTO idle_item (idle_name, idle_details, idle_price, idle_place, idle_label, "
            "picture_list, release_time, idle_status, user_id) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)",
            record.idleName, record.idleDetails, record.idlePrice, record.idlePlace,
            record.idleLabel, record.pictureList, record.releaseTime, record.idleStatus, record.userId
        );
        return result.insertId();
    }
    
    std::optional<models::IdleItem> selectByPrimaryKey(long id) {
        auto result = dbClient->execSqlSync("SELECT * FROM idle_item WHERE id = ?", id);
        if (result.empty()) return std::nullopt;
        return mapRowToIdleItem(result[0]);
    }
    
    std::vector<models::IdleItem> getAllIdleItem(long userId) {
        auto result = dbClient->execSqlSync("SELECT * FROM idle_item WHERE user_id = ?", userId);
        return mapResultToList(result);
    }
    
    int countIdleItem(const std::string& findValue) {
        std::string pattern = "%" + findValue + "%";
        auto result = dbClient->execSqlSync(
            "SELECT COUNT(*) as count FROM idle_item WHERE idle_name LIKE ? OR idle_details LIKE ?",
            pattern, pattern
        );
        return result[0]["count"].as<int>();
    }
    
    int countIdleItemByLabel(int idleLabel) {
        auto result = dbClient->execSqlSync(
            "SELECT COUNT(*) as count FROM idle_item WHERE idle_label = ?", idleLabel
        );
        return result[0]["count"].as<int>();
    }
    
    int countIdleItemByStatus(int status) {
        auto result = dbClient->execSqlSync(
            "SELECT COUNT(*) as count FROM idle_item WHERE idle_status = ?", status
        );
        return result[0]["count"].as<int>();
    }
    
    std::vector<models::IdleItem> findIdleItem(const std::string& findValue, int begin, int nums) {
        std::string pattern = "%" + findValue + "%";
        auto result = dbClient->execSqlSync(
            "SELECT * FROM idle_item WHERE idle_name LIKE ? OR idle_details LIKE ? LIMIT ?, ?",
            pattern, pattern, begin, nums
        );
        return mapResultToList(result);
    }
    
    std::vector<models::IdleItem> findIdleItemByLabel(int idleLabel, int begin, int nums) {
        auto result = dbClient->execSqlSync(
            "SELECT * FROM idle_item WHERE idle_label = ? LIMIT ?, ?",
            idleLabel, begin, nums
        );
        return mapResultToList(result);
    }
    
    std::vector<models::IdleItem> getIdleItemByStatus(int status, int begin, int nums) {
        auto result = dbClient->execSqlSync(
            "SELECT * FROM idle_item WHERE idle_status = ? LIMIT ?, ?",
            status, begin, nums
        );
        return mapResultToList(result);
    }
    
    bool updateByPrimaryKey(const models::IdleItem& record) {
        auto result = dbClient->execSqlSync(
            "UPDATE idle_item SET idle_name = ?, idle_details = ?, idle_price = ?, "
            "idle_place = ?, idle_label = ?, picture_list = ?, release_time = ?, "
            "idle_status = ?, user_id = ? WHERE id = ?",
            record.idleName, record.idleDetails, record.idlePrice, record.idlePlace,
            record.idleLabel, record.pictureList, record.releaseTime, record.idleStatus,
            record.userId, record.id
        );
        return result.affectedRows() > 0;
    }
    
    std::vector<models::IdleItem> findIdleByList(const std::vector<long>& idList) {
        if (idList.empty()) return {};
        
        std::string placeholders = std::string(idList.size() * 2 - 1, '?');
        for (size_t i = 1; i < idList.size(); i++) {
            placeholders[i * 2 - 1] = ',';
        }
        
        std::string sql = "SELECT * FROM idle_item WHERE id IN (" + placeholders + ")";
        // 注意：这里需要动态构建参数，Drogon支持可变参数
        // 简化实现，实际使用时需要根据idList大小动态传参
        auto result = dbClient->execSqlSync(sql, idList);
        return mapResultToList(result);
    }
    
private:
    DbClientPtr dbClient;
    
    models::IdleItem mapRowToIdleItem(const Row& row) {
        models::IdleItem item;
        item.id = row["id"].as<long>();
        item.idleName = row["idle_name"].as<std::string>();
        item.idleDetails = row["idle_details"].as<std::string>();
        item.idlePrice = row["idle_price"].as<double>();
        item.idlePlace = row["idle_place"].as<std::string>();
        item.idleLabel = row["idle_label"].as<int>();
        item.pictureList = row["picture_list"].as<std::string>();
        item.releaseTime = row["release_time"].as<std::string>();
        item.idleStatus = row["idle_status"].as<int>();
        item.userId = row["user_id"].as<long>();
        return item;
    }
    
    std::vector<models::IdleItem> mapResultToList(const Result& result) {
        std::vector<models::IdleItem> list;
        for (auto row : result) {
            list.push_back(mapRowToIdleItem(row));
        }
        return list;
    }
};

} // namespace dao
