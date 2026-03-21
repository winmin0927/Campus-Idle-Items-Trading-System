#pragma once

#include <mutex>
#include <vector>
#include <spdlog/spdlog.h>
#include "dao/OrderDao.h"
#include "dao/IdleItemDao.h"
#include "dao/OrderAddressDao.h"
#include "models/Order.h"
#include "utils/OrderTask.h"
#include "utils/OrderTaskHandler.h"
#include "vo/PageVo.h"

namespace services {

class OrderService {
private:
    dao::OrderDao orderDao;
    dao::IdleItemDao idleItemDao;
    dao::OrderAddressDao orderAddressDao;
    
    // 分段锁，防止超卖
    static constexpr int LOCK_SIZE = 100;
    std::vector<std::mutex> locks{LOCK_SIZE};

public:
    // 添加订单
    bool addOrder(models::Order& order) {
        int lockIndex = order.idleId % LOCK_SIZE;
        std::lock_guard<std::mutex> guard(locks[lockIndex]);
        
        return addOrderHelp(order, lockIndex);
    }

    // 更新订单
    bool updateOrder(models::Order order) {
        auto original = orderDao.selectByPrimaryKey(order.id);
        if (!original.has_value()) {
            return false;
        }

        // 保留原有的订单号和创建时间（这些字段不应被修改）
        order.orderNumber = original->orderNumber;
        order.createTime = original->createTime;
        
        // 如果前端没有传递这些字段，使用原值
        if (order.userId == 0) {
            order.userId = original->userId;
        }
        if (order.idleId == 0) {
            order.idleId = original->idleId;
        }
        if (order.addressId == 0) {
            order.addressId = original->addressId;
        }

        return orderDao.updateByPrimaryKey(order);
    }

    // 获取订单信息
    std::optional<models::Order> getOrder(long id) {
        auto order = orderDao.selectByPrimaryKey(id);
        if (!order.has_value()) {
            return std::nullopt;
        }

        // 加载关联的闲置商品信息
        auto idleItem = idleItemDao.selectByPrimaryKey(order->idleId);
        if (idleItem.has_value()) {
            order->idleItem = *idleItem;
        }

        return order;
    }

    // 获取我的订单（我购买的）
    std::vector<models::Order> getMyOrder(long userId) {
        auto orders = orderDao.selectByUserId(userId);
        
        // 加载每个订单的闲置商品信息
        for (auto& order : orders) {
            auto idleItem = idleItemDao.selectByPrimaryKey(order.idleId);
            if (idleItem.has_value()) {
                order.idleItem = *idleItem;
            }
        }
        
        return orders;
    }

    // 获取我卖出的商品订单
    std::vector<models::Order> getMySoldIdle(long userId) {
        // 先获取用户的所有闲置商品
        auto myIdleItems = idleItemDao.getAllIdleItem(userId);
        
        std::vector<models::Order> soldOrders;
        for (const auto& idleItem : myIdleItems) {
            auto orders = orderDao.selectByIdleId(idleItem.id);
            for (auto& order : orders) {
                order.idleItem = idleItem;
                soldOrders.push_back(order);
            }
        }
        
        return soldOrders;
    }

    vo::PageVo<models::Order> getAllOrder(int page, int nums) {
        int offset = (page - 1) * nums;
        auto records = orderDao.selectAll(offset, nums);
        int total = orderDao.countAll();

        vo::PageVo<models::Order> pageVo;
        pageVo.list = records;
        pageVo.count = total;
        return pageVo;
    }

    // 根据 ID 删除订单
    bool deleteOrder(long id) {
        return orderDao.deleteByPrimaryKey(id);
    }

private:
    // 添加订单的辅助方法
    bool addOrderHelp(models::Order& order, long lockIndex) {
        // 检查商品状态
        auto idleItem = idleItemDao.selectByPrimaryKey(order.idleId);
        if (!idleItem.has_value()) {
            spdlog::warn("Idle item not found: {}", order.idleId);
            return false;
        }

        // 检查商品是否可售（状态为1）
        if (idleItem->idleStatus != 1) {
            spdlog::warn("Idle item {} is not available, status: {}", 
                        order.idleId, idleItem->idleStatus);
            return false;
        }

        // 更新商品状态为已下单（状态2）
        idleItem->idleStatus = 2;
        if (!idleItemDao.updateByPrimaryKey(*idleItem)) {
            spdlog::error("Failed to update idle item status: {}", order.idleId);
            return false;
        }

        // 插入订单
        long orderId = orderDao.insert(order);
        if (orderId <= 0) {
            // 回滚商品状态
            idleItem->idleStatus = 1;
            idleItemDao.updateByPrimaryKey(*idleItem);
            spdlog::error("Failed to insert order");
            return false;
        }

        order.id = orderId;

        // 添加延迟取消任务（30分钟后自动取消未支付订单）
        if (order.paymentStatus == 0) {
            try {
                myutils::OrderTask task(order,30 * 60);
                myutils::OrderTaskHandler::getInstance().addTask(task);
                spdlog::info("Added cancel task for order: {}", order.orderNumber);
            } catch (const std::exception& e) {
                // 任务添加失败不影响订单创建
                spdlog::warn("Failed to add order cancel task: {}", e.what());
            }
        }

        return true;
    }
};

} // namespace services
