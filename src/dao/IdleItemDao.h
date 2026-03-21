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
            "INSERT INTO idle_item (userId, idleName, idleDetails, pictureList, idlePrice, "
            "idlePlace, idleLabel, idleStatus, releaseTime) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)",
            record.userId, record.idleName, record.idleDetails, record.pictureList, record.idlePrice, record.idlePlace,
            record.idleLabel, record.idleStatus, record.releaseTime
        );
        return result.insertId();
    }
    
    std::optional<models::IdleItem> selectByPrimaryKey(long id) {
        auto result = dbClient->execSqlSync("SELECT * FROM idle_item WHERE id = ?", id);
        if (result.empty()) return std::nullopt;
        return mapRowToIdleItem(result[0]);
    }
    
    std::vector<models::IdleItem> getAllIdleItem(long userId) {
        auto result = dbClient->execSqlSync("SELECT * FROM idle_item WHERE userId = ?", userId);
        return mapResultToList(result);
    }
    
    int countIdleItem(const std::string& findValue) {
        std::string pattern = "%" + findValue + "%";
        auto result = dbClient->execSqlSync(
            "SELECT COUNT(*) as count FROM idle_item WHERE idleName LIKE ? OR idleDetails LIKE ?",
            pattern, pattern
        );
        return result[0]["count"].as<int>();
    }
    
    int countIdleItemByLabel(int idleLabel) {
        auto result = dbClient->execSqlSync(
            "SELECT COUNT(*) as count FROM idle_item WHERE idleLabel = ?", idleLabel
        );
        return result[0]["count"].as<int>();
    }
    
    int countIdleItemByStatus(int status) {
        auto result = dbClient->execSqlSync(
            "SELECT COUNT(*) as count FROM idle_item WHERE idleStatus = ?", status
        );
        return result[0]["count"].as<int>();
    }
    
    std::vector<models::IdleItem> findIdleItem(const std::string& findValue, int begin, int nums) {
        std::string pattern = "%" + findValue + "%";
        auto result = dbClient->execSqlSync(
            "SELECT * FROM idle_item WHERE idleName LIKE ? OR idleDetails LIKE ? LIMIT ?, ?",
            pattern, pattern, begin, nums
        );
        return mapResultToList(result);
    }
    
    std::vector<models::IdleItem> findIdleItemByLabel(int idleLabel, int begin, int nums) {
        auto result = dbClient->execSqlSync(
            "SELECT * FROM idle_item WHERE idleLabel = ? LIMIT ?, ?",
            idleLabel, begin, nums
        );
        return mapResultToList(result);
    }
    
    std::vector<models::IdleItem> getIdleItemByStatus(int status, int begin, int nums) {
        auto result = dbClient->execSqlSync(
            "SELECT * FROM idle_item WHERE idleStatus = ? LIMIT ?, ?",
            status, begin, nums
        );
        return mapResultToList(result);
    }
    
    bool updateByPrimaryKey(const models::IdleItem& record) {
        auto result = dbClient->execSqlSync(
            "UPDATE idle_item SET userId = ?, idleName = ?, idleDetails = ?, pictureList = ?,"
            " idlePrice = ?, idlePlace = ?, idleLabel = ?, idleStatus = ?, releaseTime = ?"
            " WHERE id = ?",
            record.userId, record.idleName, record.idleDetails, record.pictureList, record.idlePrice,
            record.idlePlace, record.idleLabel, record.idleStatus, record.releaseTime, record.id
        );
        return result.affectedRows() > 0;
    }
    
    std::vector<models::IdleItem> findIdleByList(const std::vector<long>& idList) {
        if (idList.empty()) return {};

        std::string placeholders;
        for (size_t i = 0; i < idList.size(); ++i) {
            if (!placeholders.empty()) placeholders += ",";
            placeholders += "?";
        }
        std::string sql = "SELECT * FROM idle_item WHERE id IN (" + placeholders + ")";

        auto binder = *dbClient << sql;
        for (auto id : idList) {
            binder << id;
        }

        binder << Mode::Blocking;
        Result result(nullptr);
        binder >> [&result](const Result &r) { result = r; };
        binder.exec();
        
        return mapResultToList(result);
    }
    
private:
    DbClientPtr dbClient;
    
    models::IdleItem mapRowToIdleItem(const Row& row) {
        models::IdleItem item;
        item.id = row["id"].as<long>();
        item.userId = row["userId"].as<long>();
        item.idleName = row["idleName"].as<std::string>();
        item.idleDetails = row["idleDetails"].as<std::string>();
        item.pictureList = row["pictureList"].as<std::string>();
        item.idlePrice = row["idlePrice"].as<double>();
        item.idlePlace = row["idlePlace"].as<std::string>();
        item.idleLabel = row["idleLabel"].as<int>();
        item.idleStatus = row["idleStatus"].as<int>();
        item.releaseTime = row["releaseTime"].as<std::string>();
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