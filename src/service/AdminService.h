#pragma once

#include <optional>
#include "models/Admin.h"
#include "vo/PageVo.h"
#include "dao/AdminDao.h"

namespace services {

class AdminService {
public:
    AdminService() : adminDao() {}
    
    std::optional<models::Admin> login(const std::string& accountNumber, const std::string& adminPassword) {
        return adminDao.login(accountNumber, adminPassword);
    }
    
    vo::PageVo<models::Admin> getAdminList(int page, int nums) {
        int begin = (page - 1) * nums;
        auto list = adminDao.getList(begin, nums);
        int count = adminDao.getCount();
        return vo::PageVo<models::Admin>(list, count);
    }
    
    bool addAdmin(const models::Admin& admin) {
        return adminDao.insert(admin) > 0;
    }
    
private:
    dao::AdminDao adminDao;
};

} // namespace services
