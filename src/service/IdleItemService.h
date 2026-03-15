#pragma once

#include <vector>
#include <unordered_map>
#include "models/IdleItem.h"
#include "vo/PageVo.h"
#include "vo/IdleItemVo.h"
#include "dao/IdleItemDao.h"
#include "dao/UserDao.h"

namespace services {

class IdleItemService {
public:
    IdleItemService() : idleItemDao(), userDao() {}
    
    bool addIdleItem(const models::IdleItem& idleItem) {
        return idleItemDao.insert(idleItem) > 0;
    }
    
    std::optional<vo::IdleItemVo> getIdleItem(long id) {
        auto idleItem = idleItemDao.selectByPrimaryKey(id);
        if (!idleItem.has_value()) return std::nullopt;
        
        auto user = userDao.selectByPrimaryKey(idleItem->userId);
        if (user.has_value()) {
            return vo::IdleItemVo(*idleItem, *user);
        }
        return std::nullopt;
    }
    
    std::vector<models::IdleItem> getAllIdleItem(long userId) {
        return idleItemDao.getAllIdleItem(userId);
    }
    
    vo::PageVo<vo::IdleItemVo> findIdleItem(const std::string& findValue, int page, int nums) {
        auto list = idleItemDao.findIdleItem(findValue, (page - 1) * nums, nums);
        auto result = attachUserInfo(list);
        int count = idleItemDao.countIdleItem(findValue);
        return vo::PageVo<vo::IdleItemVo>(result, count);
    }
    
    vo::PageVo<vo::IdleItemVo> findIdleItemByLabel(int idleLabel, int page, int nums) {
        auto list = idleItemDao.findIdleItemByLabel(idleLabel, (page - 1) * nums, nums);
        auto result = attachUserInfo(list);
        int count = idleItemDao.countIdleItemByLabel(idleLabel);
        return vo::PageVo<vo::IdleItemVo>(result, count);
    }
    
    bool updateIdleItem(const models::IdleItem& idleItem) {
        return idleItemDao.updateByPrimaryKey(idleItem);
    }
    
    vo::PageVo<vo::IdleItemVo> adminGetIdleList(int status, int page, int nums) {
        auto list = idleItemDao.getIdleItemByStatus(status, (page - 1) * nums, nums);
        auto result = attachUserInfo(list);
        int count = idleItemDao.countIdleItemByStatus(status);
        return vo::PageVo<vo::IdleItemVo>(result, count);
    }
    
private:
    dao::IdleItemDao idleItemDao;
    dao::UserDao userDao;
    
    std::vector<vo::IdleItemVo> attachUserInfo(const std::vector<models::IdleItem>& list) {
        std::vector<vo::IdleItemVo> result;
        if (list.empty()) return result;
        
        std::vector<long> userIdList;
        for (const auto& item : list) {
            userIdList.push_back(item.userId);
        }
        
        auto userList = userDao.findUserByList(userIdList);
        std::unordered_map<long, models::User> userMap;
        for (const auto& user : userList) {
            userMap[user.id] = user;
        }
        
        for (const auto& item : list) {
            auto it = userMap.find(item.userId);
            if (it != userMap.end()) {
                result.emplace_back(item, it->second);
            }
        }
        
        return result;
    }
};

} // namespace services
