#pragma once

#include <vector>
#include <unordered_map>
#include "models/Favorite.h"
#include "vo/FavoriteVo.h"
#include "dao/FavoriteDao.h"
#include "dao/IdleItemDao.h"

namespace services {

class FavoriteService {
public:
    FavoriteService() : favoriteDao(), idleItemDao() {}
    
    bool addFavorite(const models::Favorite& favorite) {
        return favoriteDao.insert(favorite) > 0;
    }
    
    bool deleteFavorite(long id) {
        return favoriteDao.deleteByPrimaryKey(id);
    }
    
    int isFavorite(long userId, long idleId) {
        return favoriteDao.checkFavorite(userId, idleId);
    }
    
    std::vector<vo::FavoriteVo> getAllFavorite(long userId) {
        auto list = favoriteDao.getMyFavorite(userId);
        std::vector<vo::FavoriteVo> result;
        
        if (!list.empty()) {
            std::vector<long> idleIdList;
            for (const auto& fav : list) {
                idleIdList.push_back(fav.idleId);
            }
            
            auto idleItemList = idleItemDao.findIdleByList(idleIdList);
            std::unordered_map<long, models::IdleItem> idleMap;
            for (const auto& item : idleItemList) {
                idleMap[item.id] = item;
            }
            
            for (const auto& fav : list) {
                auto it = idleMap.find(fav.idleId);
                if (it != idleMap.end()) {
                    result.emplace_back(fav, it->second);
                }
            }
        }
        
        return result;
    }
    
private:
    dao::FavoriteDao favoriteDao;
    dao::IdleItemDao idleItemDao;
};

} // namespace services
