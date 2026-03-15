#pragma once

#include <vector>
#include <unordered_map>
#include <mutex>
#include "models/Order.h"
#include "vo/PageVo.h"
#include "dao/OrderDao.h"
#include "dao/IdleItemDao.h"
#include "utils/OrderTaskHandler.h"

namespace services {

class OrderService {
public:
    OrderService() : orderDao(), idleItemDao() {// 初始化分段锁
        if (locks.empty()) {
            for (int i = 0; i < 100; i++) {
                locks.emplace_back(std::make_unique<std::mutex>());
            }
        }
    }
    
    bool addOrder(models::Order& order) {
        auto idleItem = idleItemDao.selectByPrimaryKey(order.idleId);
        if (!idleItem.has_value() || idleItem->idleStatus != 1) {
            return false;
        }
        
        int key = order.idleId % 100;
        std::lock_guard<std::mutex> lock(*locks[key]);
        
        return addOrderHelp(order, idleItem->userId);
    }
    
    std::optional<models::Order> getOrder(long id) {
        auto order = orderDao.selectByPrimaryKey(id);
        if (order.has_value()) {
            auto idleItem = idleItemDao.selectByPrimaryKey(order->idleId);
            // 注意：需要扩展Order模型或使用VO来包含IdleItem
        }
        return order;
    }
    
    bool updateOrder(models::Order order) {
        // 不可修改的字段
        auto original = orderDao.selectByPrimaryKey(order.id);
        if (!original.has_value()) return false;
        
        order.orderId = original->orderId;
        order.userId = original->userId;
        order.idleId = original->idleId;
        order.orderTime = original->orderTime;
        
        if (order.orderStatus == 4) {
            // 取消订单
            if (original->orderStatus != 0) {
                return false;
            }
            
            auto idleItem = idleItemDao.selectByPrimaryKey(original->idleId);
            if (!idleItem.has_value()) return false;
            
            if (idleItem->idleStatus == 2) {
                // 恢复商品状态为可售
                models::IdleItem updateIdle;
                updateIdle.id = original->idleId;
                updateIdle.idleStatus = 1;
                
                bool orderUpdated = orderDao.updateByPrimaryKey(order);
                bool idleUpdated = idleItemDao.updateByPrimaryKey(updateIdle);
                
                return orderUpdated && idleUpdated;
            }
        }
        
        return orderDao.updateByPrimaryKey(order);
    }
    
    std::vector<models::Order> getMyOrder(long userId) {
        return orderDao.getMyOrder(userId);
    }
    
    std::vector<models::Order> getMySoldIdle(long userId) {
        auto idleList = idleItemDao.getAllIdleItem(userId);
        if (idleList.empty()) return {};
        
        std::vector<long> idleIdList;
        for (const auto& idle : idleList) {
            idleIdList.push_back(idle.id);
        }
        
        return orderDao.findOrderByIdleIdList(idleIdList);
    }
    
    vo::PageVo<models::Order> getAllOrder(int page, int nums) {
        auto list = orderDao.getAllOrder((page - 1) * nums, nums);
        int count = orderDao.countAllOrder();
        return vo::PageVo<models::Order>(list, count);
    }
    
    bool deleteOrder(long id) {
        return orderDao.deleteByPrimaryKey(id);
    }
    
private:
    dao::OrderDao orderDao;
    dao::IdleItemDao idleItemDao;
    static std::vector<std::unique_ptr<std::mutex>> locks;
    
    bool addOrderHelp(models::Order& order, long sellerId) {
        // 再次检查商品状态
        auto idleItem = idleItemDao.selectByPrimaryKey(order.idleId);
        if (!idleItem.has_value() || idleItem->idleStatus != 1) {
            return false;
        }
        
        // 更新商品状态为已下单
        models::IdleItem updateIdle;
        updateIdle.id = order.idleId;
        updateIdle.idleStatus = 2;
        
        if (idleItemDao.updateByPrimaryKey(updateIdle)) {
            long orderId = orderDao.insert(order);
            if (orderId > 0) {
                order.id = orderId;
                order.orderStatus = 4; // 待支付
                
                // 添加延迟取消任务（30分钟）
                utils::OrderTask task(order, 30 * 60);
                utils::OrderTaskHandler::getInstance().addOrder(task);
                
                return true;
            }
        }
        
        return false;
    }
};

// 静态成员初始化
std::vector<std::unique_ptr<std::mutex>> OrderService::locks;

} // namespace services
