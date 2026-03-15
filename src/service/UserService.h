#pragma once

#include <optional>
#include "models/User.h"
#include "vo/PageVo.h"
#include "dao/UserDao.h"

namespace services {

class UserService {
public:
    UserService() : userDao() {}
    
    std::optional<models::User> getUser(long id) {
        return userDao.selectByPrimaryKey(id);
    }
    
    std::optional<models::User> userLogin(const std::string& accountNumber, const std::string& userPassword) {
        return userDao.userLogin(accountNumber, userPassword);
    }
    
    bool userSignIn(const models::User& user) {
        return userDao.insert(user) > 0;
    }
    
    bool updateUserInfo(const models::User& user) {
        return userDao.updateByPrimaryKey(user);
    }
    
    bool updatePassword(const std::string& newPassword, const std::string& oldPassword, long id) {
        return userDao.updatePassword(newPassword, oldPassword, id);
    }
    
    vo::PageVo<models::User> getUserByStatus(int status, int page, int nums) {
        std::vector<models::User> list;
        int count = 0;
        
        if (status == 0) {
            count = userDao.countNormalUser();
            list = userDao.getNormalUser((page - 1) * nums, nums);
        } else {
            count = userDao.countBanUser();
            list = userDao.getBanUser((page - 1) * nums, nums);
        }
        
        return vo::PageVo<models::User>(list, count);
    }
    
private:
    dao::UserDao userDao;
};

} // namespace services
