#pragma once

#include <drogon/drogon.h>
#include <optional>
#include "models/OrderAddress.h"

using namespace drogon;
using namespace drogon::orm;

namespace dao {

class OrderAddressDao {
public:
    OrderAddressDao() : dbClient(app().getDbClient()) {}
    
    bool deleteByPrimaryKey(long id) {
        auto result = dbClient->execSqlSync("DELETE FROM order_address WHERE id = ?", id);
        return result.affectedRows() > 0;
    }
    
    long insert(const models::OrderAddress& record) {
        auto result = dbClient->execSqlSync(
            "INSERT INTO order_address (order_id, consignee_name, consignee_phone, "
            "province_name, city_name, region_name, detail_address) VALUES (?, ?, ?, ?, ?, ?, ?)",
            record.orderId, record.consigneeName, record.consigneePhone,
            record.provinceName, record.cityName, record.regionName, record.detailAddress
        );
        return result.insertId();
    }
    
    std::optional<models::OrderAddress> selectByPrimaryKey(long id) {
        auto result = dbClient->execSqlSync("SELECT * FROM order_address WHERE id = ?", id);
        if (result.empty()) return std::nullopt;
        return mapRowToOrderAddress(result[0]);
    }
    
    std::optional<models::OrderAddress> selectByOrderId(long orderId) {
        auto result = dbClient->execSqlSync("SELECT * FROM order_address WHERE order_id = ?", orderId);
        if (result.empty()) return std::nullopt;
        return mapRowToOrderAddress(result[0]);
    }
    
    bool updateByPrimaryKey(const models::OrderAddress& record) {
        auto result = dbClient->execSqlSync(
            "UPDATE order_address SET order_id = ?, consignee_name = ?, consignee_phone = ?, "
            "province_name = ?, city_name = ?, region_name = ?, detail_address = ? WHERE id = ?",
            record.orderId, record.consigneeName, record.consigneePhone,
            record.provinceName, record.cityName, record.regionName, record.detailAddress, record.id
        );
        return result.affectedRows() > 0;
    }
    
private:
    DbClientPtr dbClient;
    
    models::OrderAddress mapRowToOrderAddress(const Row& row) {
        models::OrderAddress addr;
        addr.id = row["id"].as<long>();
        addr.orderId = row["order_id"].as<long>();
        addr.consigneeName = row["consignee_name"].as<std::string>();
        addr.consigneePhone = row["consignee_phone"].as<std::string>();
        addr.provinceName = row["province_name"].as<std::string>();
        addr.cityName = row["city_name"].as<std::string>();
        addr.regionName = row["region_name"].as<std::string>();
        addr.detailAddress = row["detail_address"].as<std::string>();
        return addr;
    }
};

} // namespace dao
