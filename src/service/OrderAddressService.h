#pragma once

#include <optional>
#include "models/OrderAddress.h"
#include "dao/OrderAddressDao.h"

namespace services {

class OrderAddressService {
public:
    OrderAddressService() : orderAddressDao() {}
    
    bool addOrderAddress(const models::OrderAddress& orderAddress) {
        return orderAddressDao.insert(orderAddress) > 0;
    }
    
    bool updateOrderAddress(models::OrderAddress orderAddress) {
        orderAddress.orderId = 0; // 不允许修改orderId
        return orderAddressDao.updateByPrimaryKey(orderAddress);
    }
    
    std::optional<models::OrderAddress> getOrderAddress(long orderId) {
        return orderAddressDao.selectByOrderId(orderId);
    }
    
private:
    dao::OrderAddressDao orderAddressDao;
};

} // namespace services
