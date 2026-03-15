#pragma once

#include <drogon/drogon.h>
#include <vector>
#include <optional>
#include "models/User.h"

using namespace drogon;
using namespace drogon::orm;

namespace dao {

class UserDao {
public:
    UserDao() : dbClient(app().getDbClient()) {}
    
    bool deleteByPrimaryKey(long id) {
        auto result = dbClient->execSqlSync("DELETE FROM user WHERE id = ?", id);
        return result.affectedRows() > 0;
    }
    
    long insert(const models::User& record) {
        auto result = dbClient->execSqlSync(
            "INSERT INTO user (account_number, user_password, nickname, avatar, sign_in_time, user_status) "
            "VALUES (?, ?, ?, ?, ?, ?)",
            record.accountNumber, record.userPassword, record.nickname,
            record.avatar, record.signInTime, record.userStatus
        );
        return result.insertId();
    }
    
    std::optional<models::User> userLogin(const std::string& accountNumber, const std::string& userPassword) {
        auto result = dbClient->execSqlSync(
            "SELECT * FROM user WHERE account_number = ? AND user_password = ?",
            accountNumber, userPassword
        );
        if (result.empty()) return std::nullopt;
        return mapRowToUser(result[0]);
    }
    
    std::optional<models::User> selectByPrimaryKey(long id) {
        auto result = dbClient->execSqlSync("SELECT * FROM user WHERE id = ?", id);
        if (result.empty()) return std::nullopt;
        return mapRowToUser(result[0]);
    }
    
    std::vector<models::User> getUserList() {
        auto result = dbClient->execSqlSync("SELECT * FROM user");
        return mapResultToList(result);
    }
    
    std::vector<models::User> findUserByList(const std::vector<long>& idList) {
        if (idList.empty()) return {};
        
        std::string placeholders;
        for (size_t i = 0; i < idList.size(); i++) {
            if (i > 0) placeholders += ",";
            placeholders += "?";
        }
        
        std::string sql = "SELECT * FROM user WHERE id IN (" + placeholders + ")";
        auto result = dbClient->execSqlSync(sql, idList);
        return mapResultToList(result);
    }
    
    std::vector<models::User> getNormalUser(int begin, int nums) {
        auto result = dbClient->execSqlSync(
            "SELECT * FROM user WHERE user_status = 0 LIMIT ?, ?", begin, nums
        );
        return mapResultToList(result);
    }
    
    std::vector<models::User> getBanUser(int begin, int nums) {
        auto result = dbClient->execSqlSync(
            "SELECT * FROM user WHERE user_status = 1 LIMIT ?, ?", begin, nums
        );
        return mapResultToList(result);
    }
    
    int countNormalUser() {
        auto result = dbClient->execSqlSync("SELECT COUNT(*) as count FROM user WHERE user_status = 0");
        return result[0]["count"].as<int>();
    }
    
    int countBanUser() {
        auto result = dbClient->execSqlSync("SELECT COUNT(*) as count FROM user WHERE user_status = 1");
        return result[0]["count"].as<int>();
    }
    
    bool updateByPrimaryKey(const models::User& record) {
        auto result = dbClient->execSqlSync(
            "UPDATE user SET account_number = ?, user_password = ?, nickname = ?, "
            "avatar = ?, sign_in_time = ?, user_status = ? WHERE id = ?",
            record.accountNumber, record.userPassword, record.nickname,
            record.avatar, record.signInTime, record.userStatus, record.id
        );
        return result.affectedRows() > 0;
    }bool updatePassword(const std::string& newPassword, const std::string& oldPassword, long id) {
        auto result = dbClient->execSqlSync(
            "UPDATE user SET user_password = ? WHERE id = ? AND user_password = ?",
            newPassword, id, oldPassword
        );
        return result.affectedRows() > 0;
    }
    
private:
    DbClientPtr dbClient;
    
    models::User mapRowToUser(const Row& row) {
        models::User user;
        user.id = row["id"].as<long>();
        user.accountNumber = row["account_number"].as<std::string>();
        user.userPassword = row["user_password"].as<std::string>();
        user.nickname = row["nickname"].as<std::string>();
        user.avatar = row["avatar"].as<std::string>();
        user.signInTime = row["sign_in_time"].as<std::string>();
        user.userStatus = row["user_status"].as<int>();
        return user;
    }
    
    std::vector<models::User> mapResultToList(const Result& result) {
        std::vector<models::User> list;
        for (auto row : result) {
            list.push_back(mapRowToUser(row));
        }
        return list;
    }
};

} // namespace dao
