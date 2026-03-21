#pragma once

#include <drogon/drogon.h>
#include <vector>
#include <optional>
#include "models/Admin.h"

using namespace drogon;
using namespace drogon::orm;

namespace dao {

class AdminDao {
public:
    AdminDao() : dbClient(app().getDbClient()) {}
    
    bool deleteByPrimaryKey(long id) {
        auto result = dbClient->execSqlSync("DELETE FROM admin WHERE id = ?", id);
        return result.affectedRows() > 0;
    }
    
    long insert(const models::Admin& record) {
        auto result = dbClient->execSqlSync(
            "INSERT INTO admin (accountNumber, adminPassword, adminName) VALUES (?, ?, ?)",
            record.accountNumber, record.adminPassword, record.adminName
        );
        return result.insertId();
    }
    
    std::optional<models::Admin> selectByPrimaryKey(long id) {
        auto result = dbClient->execSqlSync("SELECT * FROM admin WHERE id = ?", id);
        if (result.empty()) return std::nullopt;
        return mapRowToAdmin(result[0]);
    }
    
    bool updateByPrimaryKey(const models::Admin& record) {
        auto result = dbClient->execSqlSync(
            "UPDATE admin SET accountNumber = ?, adminPassword = ?, adminName = ? WHERE id = ?",
            record.accountNumber, record.adminPassword, record.adminName, record.id
        );
        return result.affectedRows() > 0;
    }
    
    std::optional<models::Admin> login(const std::string& accountNumber, const std::string& adminPassword) {
        auto result = dbClient->execSqlSync(
            "SELECT * FROM admin WHERE accountNumber = ? AND adminPassword = ?",
            accountNumber, adminPassword
        );
        if (result.empty()) return std::nullopt;
        return mapRowToAdmin(result[0]);
    }
    
    std::vector<models::Admin> getList(int begin, int nums) {
        auto result = dbClient->execSqlSync("SELECT * FROM admin LIMIT ?, ?", begin, nums);
        std::vector<models::Admin> list;
        for (auto row : result) {
            list.push_back(mapRowToAdmin(row));
        }
        return list;
    }
    
    int getCount() {
        auto result = dbClient->execSqlSync("SELECT COUNT(*) as count FROM admin");
        return result[0]["count"].as<int>();
    }
    
private:
    DbClientPtr dbClient;
    
    models::Admin mapRowToAdmin(const Row& row) {
        models::Admin admin;
        admin.id = row["id"].as<long>();
        admin.accountNumber = row["accountNumber"].as<std::string>();
        admin.adminPassword = row["adminPassword"].as<std::string>();
        admin.adminName = row["adminName"].as<std::string>();
        return admin;
    }
};

} // namespace dao