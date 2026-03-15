#pragma once

#include <drogon/drogon.h>
#include <vector>
#include <optional>
#include "models/Order.h"

using namespace drogon;
using namespace drogon::orm;

namespace dao {

class OrderDao {
public:
    OrderDao() : dbClient(app().getDbClient()) {}
    
    bool deleteByPrimaryKey(long id) {
        auto result = dbClient->execSqlSync("DELETE FROM `order` WHERE id = ?", id);
        return result.affectedRows() > 0;
    }
    
    long insert(const models::Order& record) {
        auto result = dbClient->execSqlSync(
            "INSERT INTO `order` (order_id, idle_id, user_id, address_id, order_time, order_status) "
            "VALUES (?, ?, ?, ?, ?, ?)",
            record.orderId, record.idleId, record.userId, record.addressId,
            record.orderTime, record.orderStatus
        );
        return result.insertId();
    }
    
    std::optional<models::Order> selectByPrimaryKey(long id) {
        auto result = dbClient->execSqlSync("SELECT * FROM `order` WHERE id = ?", id);
        if (result.empty()) return std::nullopt;
        return mapRowToOrder(result[0]);
    }
    
    std::vector<models::Order> getMyOrder(long userId) {
        auto result = dbClient->execSqlSync("SELECT * FROM `order` WHERE user_id = ?", userId);
        return mapResultToList(result);
    }
    
    std::vector<models::Order> getAllOrder(int begin, int nums) {
        auto result = dbClient->execSqlSync("SELECT * FROM `order` LIMIT?, ?", begin, nums);
        return mapResultToList(result);
    }
    
    int countAllOrder() {
        auto result = dbClient->execSqlSync("SELECT COUNT(*) as count FROM `order`");
        return result[0]["count"].as<int>();
    }
    
    std::vector<models::Order> findOrderByIdleIdList(const std::vector<long>& idleIdList) {
        if (idleIdList.empty()) return {};
        
        std::string placeholders;
        for (size_t i = 0; i < idleIdList.size(); i++) {
            if (i > 0) placeholders += ",";
            placeholders += "?";
        }
        
        std::string sql = "SELECT * FROM `order` WHERE idle_id IN (" + placeholders + ")";
        auto result = dbClient->execSqlSync(sql, idleIdList);
        return mapResultToList(result);
    }
    
    bool updateByPrimaryKey(const models::Order& record) {
        auto result = dbClient->execSqlSync(
            "UPDATE `order` SET order_id = ?, idle_id = ?, user_id = ?, "
            "address_id = ?, order_time = ?, order_status = ? WHERE id = ?",
            record.orderId, record.idleId, record.userId, record.addressId,
            record.orderTime, record.orderStatus, record.id
        );
        return result.affectedRows() > 0;
    }
    
private:
    DbClientPtr dbClient;
    
    models::Order mapRowToOrder(const Row& row) {
        models::Order order;
        order.id = row["id"].as<long>();
        order.orderId = row["order_id"].as<std::string>();
        order.idleId = row["idle_id"].as<long>();
        order.userId = row["user_id"].as<long>();
        order.addressId = row["address_id"].as<long>();
        order.orderTime = row["order_time"].as<std::string>();
        order.orderStatus = row["order_status"].as<int>();
        return order;
    }
    
    std::vector<models::Order> mapResultToList(const Result& result) {
        std::vector<models::Order> list;
        for (auto row : result) {
            list.push_back(mapRowToOrder(row));
        }
        return list;
    }
};

} // namespace dao
