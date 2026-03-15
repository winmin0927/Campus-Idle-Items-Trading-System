#pragma once

#include <vector>
#include <unordered_map>
#include "models/Message.h"
#include "vo/MessageVo.h"
#include "dao/MessageDao.h"
#include "dao/UserDao.h"
#include "dao/IdleItemDao.h"

namespace services {

class MessageService {
public:
    MessageService() : messageDao(), userDao(), idleItemDao() {}
    
    bool addMessage(const models::Message& message) {
        return messageDao.insert(message) > 0;
    }
    
    bool deleteMessage(long id) {
        return messageDao.deleteByPrimaryKey(id);
    }
    
    std::optional<models::Message> getMessage(long id) {
        return messageDao.selectByPrimaryKey(id);
    }
    
    std::vector<vo::MessageVo> getAllMyMessage(long userId) {
        auto list = messageDao.getMyMessage(userId);
        std::vector<vo::MessageVo> result;
        if (list.empty()) return result;
        
        // 获取用户信息
        std::vector<long> userIdList;
        for (const auto& msg : list) {
            userIdList.push_back(msg.userId);
        }
        auto userList = userDao.findUserByList(userIdList);
        std::unordered_map<long, models::User> userMap;
        for (const auto& user : userList) {
            userMap[user.id] = user;
        }
        
        // 获取闲置商品信息
        std::vector<long> idleIdList;
        for (const auto& msg : list) {
            idleIdList.push_back(msg.idleId);
        }
        auto idleList = idleItemDao.findIdleByList(idleIdList);
        std::unordered_map<long, models::IdleItem> idleMap;
        for (const auto& idle : idleList) {
            idleMap[idle.id] = idle;
        }
        
        // 组装结果
        for (const auto& msg : list) {
            vo::MessageVo vo;
            vo.message = msg;
            auto userIt = userMap.find(msg.userId);
            if (userIt != userMap.end()) {
                vo.fromUser = userIt->second;
            }
            auto idleIt = idleMap.find(msg.idleId);
            if (idleIt != idleMap.end()) {
                vo.idleItem = idleIt->second;
            }
            result.push_back(vo);
        }
        
        return result;
    }
    
    std::vector<vo::MessageVo> getAllIdleMessage(long idleId) {
        auto list = messageDao.getIdleMessage(idleId);
        std::vector<vo::MessageVo> result;
        if (list.empty()) return result;
        
        // 获取用户信息
        std::vector<long> userIdList;
        for (const auto& msg : list) {
            userIdList.push_back(msg.userId);
        }
        auto userList = userDao.findUserByList(userIdList);
        std::unordered_map<long, models::User> userMap;
        for (const auto& user : userList) {
            userMap[user.id] = user;
        }
        
        // 构建留言映射（用于回复关系）
        std::unordered_map<long, models::Message> msgMap;
        for (const auto& msg : list) {
            msgMap[msg.id] = msg;
        }
        
        // 组装结果
        for (const auto& msg : list) {
            vo::MessageVo vo;
            vo.message = msg;
            auto userIt = userMap.find(msg.userId);
            if (userIt != userMap.end()) {
                vo.fromUser = userIt->second;
            }
            
            // 处理回复关系（假设Message有toMessage和toUser字段）
            // 注意：需要根据实际的Message模型调整
            
            result.push_back(vo);
        }
        
        return result;
    }
    
private:
    dao::MessageDao messageDao;
    dao::UserDao userDao;
    dao::IdleItemDao idleItemDao;
};

} // namespace services
