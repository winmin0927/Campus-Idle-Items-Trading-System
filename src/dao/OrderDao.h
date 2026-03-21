#pragma once

#include <drogon/drogon.h>
#include "models/Order.h"
#include <optional>
#include <vector>

using namespace drogon;
using namespace drogon::orm;

namespace dao {

class OrderDao {
private:
    DbClientPtr dbClient;

public:
    OrderDao() : dbClient(app().getDbClient()) {}

    long insert(const models::Order& record) {
        if (record.paymentTime.empty()) {
            auto result = dbClient->execSqlSync(
                "INSERT INTO `order` (orderNumber, idleId, userId, addressId, "
                "createTime, orderStatus, paymentStatus) "
                "VALUES (?, ?, ?, ?, ?, ?, ?)",
                record.orderNumber, record.idleId, record.userId, record.addressId,
                record.createTime, record.orderStatus, record.paymentStatus
            );
            return result.insertId();
        } else {
            auto result = dbClient->execSqlSync(
                "INSERT INTO `order` (orderNumber, idleId, userId, addressId, "
                "createTime, orderStatus, paymentStatus, paymentTime) "
                "VALUES (?, ?, ?, ?, ?, ?, ?, ?)",
                record.orderNumber, record.idleId, record.userId, record.addressId,
                record.createTime, record.orderStatus, record.paymentStatus, record.paymentTime
            );
            return result.insertId();
        }
    }

    std::optional<models::Order> selectByPrimaryKey(long id) {
        auto result = dbClient->execSqlSync(
            "SELECT * FROM `order` WHERE id = ?", id
        );
        if (result.empty()) {
            return std::nullopt;
        }
        return mapRowToOrder(result[0]);
    }

    std::vector<models::Order> selectByUserId(long userId) {
        auto result = dbClient->execSqlSync(
            "SELECT * FROM `order` WHERE userId = ? ORDER BY createTime DESC",
            userId
        );
        std::vector<models::Order> orders;
        for (const auto& row : result) {
            orders.push_back(mapRowToOrder(row));
        }
        return orders;
    }

    std::vector<models::Order> selectByIdleId(long idleId) {
        auto result = dbClient->execSqlSync(
            "SELECT * FROM `order` WHERE idleId = ? ORDER BY createTime DESC",
            idleId
        );
        std::vector<models::Order> orders;
        for (const auto& row : result) {
            orders.push_back(mapRowToOrder(row));
        }
        return orders;
    }

    std::optional<models::Order> selectByOrderNumber(const std::string& orderNumber) {
        auto result = dbClient->execSqlSync(
            "SELECT * FROM `order` WHERE orderNumber = ?", orderNumber
        );
        if (result.empty()) {
            return std::nullopt;
        }
        return mapRowToOrder(result[0]);
    }

    std::vector<models::Order> selectAll(int offset, int limit) {
        auto result = dbClient->execSqlSync(
            "SELECT * FROM `order` ORDER BY createTime DESC LIMIT ?, ?",
            offset, limit
        );
        std::vector<models::Order> orders;
        for (const auto& row : result) {
            orders.push_back(mapRowToOrder(row));
        }
        return orders;
    }

    int countAll() {
        auto result = dbClient->execSqlSync("SELECT COUNT(*) as count FROM `order`");
        return result[0]["count"].as<int>();
    }

    bool updateByPrimaryKey(const models::Order& record) {
        if (record.paymentTime.empty()) {
            auto result = dbClient->execSqlSync(
                "UPDATE `order` SET orderNumber = ?, idleId = ?, userId = ?, "
                "addressId = ?, createTime = ?, orderStatus = ?, "
                "paymentStatus = ? WHERE id = ?",
                record.orderNumber, record.idleId, record.userId, record.addressId,
                record.createTime, record.orderStatus, record.paymentStatus, record.id
            );
            return result.affectedRows() > 0;
        } else {
            auto result = dbClient->execSqlSync(
                "UPDATE `order` SET orderNumber = ?, idleId = ?, userId = ?, "
                "addressId = ?, createTime = ?, orderStatus = ?, "
                "paymentStatus = ?, paymentTime = ? WHERE id = ?",
                record.orderNumber, record.idleId, record.userId, record.addressId,
                record.createTime, record.orderStatus, record.paymentStatus,
                record.paymentTime, record.id
            );
            return result.affectedRows() > 0;
        }
    }

    bool deleteByPrimaryKey(long id) {
        auto result = dbClient->execSqlSync(
            "DELETE FROM `order` WHERE id = ?", id
        );
        return result.affectedRows() > 0;
    }

private:
    models::Order mapRowToOrder(const Row& row) {
        models::Order order;
        order.id = row["id"].as<long>();
        order.orderNumber = row["orderNumber"].as<std::string>();
        order.idleId = row["idleId"].as<long>();
        order.userId = row["userId"].as<long>();
        order.addressId = row["addressId"].as<long>();
        order.createTime = row["createTime"].as<std::string>();
        order.orderStatus = row["orderStatus"].as<int>();
        order.paymentStatus = row["paymentStatus"].as<int>();
        
        if (!row["paymentTime"].isNull()) {
            order.paymentTime = row["paymentTime"].as<std::string>();
        }
        
        return order;
    }
};

} // namespace dao