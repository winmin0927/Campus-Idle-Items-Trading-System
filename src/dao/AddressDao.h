#pragma once

#include<drogon/drogon.h>
#include <vector>
#include <optional>
#include "models/Address.h"

using namespace drogon;
using namespace drogon::orm;

namespace dao {

class AddressDao {
public:
    AddressDao() : dbClient(app().getDbClient()) {}
    
    bool deleteByPrimaryKeyAndUser(long id, long userId) {
        auto result = dbClient->execSqlSync(
            "DELETE FROM address WHERE id = ? AND userId = ?", id, userId
        );
        return result.affectedRows() > 0;
    }
    
    long insert(const models::Address& record) {
        auto result = dbClient->execSqlSync(
            "INSERT INTO address (consigneeName, consigneePhone, provinceName, cityName, "
            "regionName, detailAddress, defaultFlag, userId) VALUES (?, ?, ?, ?, ?, ?, ?, ?)",
            record.consigneeName, record.consigneePhone, record.provinceName, record.cityName,
            record.regionName, record.detailAddress, record.defaultFlag, record.userId
        );
        return result.insertId();
    }
    
    std::optional<models::Address> selectByPrimaryKey(long id) {
        auto result = dbClient->execSqlSync("SELECT * FROM address WHERE id = ?", id);
        if (result.empty()) return std::nullopt;
        return mapRowToAddress(result[0]);
    }
    
    std::vector<models::Address> getAddressByUser(long userId) {
        auto result = dbClient->execSqlSync("SELECT * FROM address WHERE userId = ?", userId);
        return mapResultToList(result);
    }
    
    std::vector<models::Address> getDefaultAddress(long userId) {
        auto result = dbClient->execSqlSync(
            "SELECT * FROM address WHERE userId = ? AND defaultFlag = 1", userId
        );
        return mapResultToList(result);
    }
    
    bool updateByPrimaryKey(const models::Address& record) {
        auto result = dbClient->execSqlSync(
            "UPDATE address SET consigneeName = ?, consigneePhone = ?, provinceName = ?, "
            "cityName = ?, regionName = ?, detailAddress = ?, defaultFlag = ?, userId = ? WHERE id = ?",
            record.consigneeName, record.consigneePhone, record.provinceName, record.cityName,
            record.regionName, record.detailAddress, record.defaultFlag, record.userId, record.id
        );
        return result.affectedRows() > 0;
    }
    
    bool updateByUserIdSelective(const models::Address& record) {
        auto result = dbClient->execSqlSync(
            "UPDATE address SET defaultFlag = ? WHERE userId = ?",
            record.defaultFlag, record.userId
        );
        return result.affectedRows() > 0;
    }
    
private:
    DbClientPtr dbClient;
    
    models::Address mapRowToAddress(const Row& row) {
        models::Address address;
        address.id = row["id"].as<long>();
        address.consigneeName = row["consigneeName"].as<std::string>();
        address.consigneePhone = row["consigneePhone"].as<std::string>();
        address.provinceName = row["provinceName"].as<std::string>();
        address.cityName = row["cityName"].as<std::string>();
        address.regionName = row["regionName"].as<std::string>();
        address.detailAddress = row["detailAddress"].as<std::string>();
        address.defaultFlag = row["defaultFlag"].as<bool>();
        address.userId = row["userId"].as<long>();
        return address;
    }
    
    std::vector<models::Address> mapResultToList(const Result& result) {
        std::vector<models::Address> list;
        for (auto row : result) {
            list.push_back(mapRowToAddress(row));
        }
        return list;
    }
};

} // namespace dao