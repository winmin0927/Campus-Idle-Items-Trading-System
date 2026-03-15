#pragma once

#include <vector>
#include <optional>
#include "models/Address.h"
#include "dao/AddressDao.h"

namespace services {

class AddressService {
public:
    AddressService() : addressDao() {}
    
    std::vector<models::Address> getAddressByUser(long userId) {
        return addressDao.getAddressByUser(userId);
    }
    
    std::optional<models::Address> getAddressById(long id, long userId) {
        auto address = addressDao.selectByPrimaryKey(id);
        if (address.has_value() && address->userId == userId) {
            return address;
        }
        return std::nullopt;
    }
    
    bool addAddress(models::Address& address) {
        if (address.defaultFlag) {
            // 将该用户的所有地址设为非默认
            models::Address updateAddr;
            updateAddr.defaultFlag = false;
            updateAddr.userId = address.userId;
            addressDao.updateByUserIdSelective(updateAddr);
        } else {
            // 检查是否有默认地址，若无则将当前地址设为默认
            auto defaultList = addressDao.getDefaultAddress(address.userId);
            if (defaultList.empty()) {
                address.defaultFlag = true;
            }
        }
        return addressDao.insert(address) > 0;
    }
    
    bool updateAddress(models::Address& address) {
        if (address.defaultFlag) {
            // 将该用户的所有地址设为非默认
            models::Address updateAddr;
            updateAddr.defaultFlag = false;
            updateAddr.userId = address.userId;
            addressDao.updateByUserIdSelective(updateAddr);
        } else {
            // 若取消默认地址，则将第一个地址设为默认
            auto list = addressDao.getAddressByUser(address.userId);
            for (const auto& addr : list) {
                if (addr.defaultFlag && addr.id == address.id) {
                    // 当前地址是默认地址且要取消，将第一个地址设为默认
                    if (!list.empty()) {
                        models::Address firstAddr;
                        firstAddr.id = list[0].id;
                        firstAddr.defaultFlag = true;
                        addressDao.updateByPrimaryKey(firstAddr);
                    }
                    break;
                }
            }
        }
        return addressDao.updateByPrimaryKey(address);
    }
    
    bool deleteAddress(long id, long userId) {
        return addressDao.deleteByPrimaryKeyAndUser(id, userId);
    }
    
private:
    dao::AddressDao addressDao;
};

} // namespace services
