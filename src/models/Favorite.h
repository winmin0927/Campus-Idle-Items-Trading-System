#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace models {

struct Favorite {
    long id = 0;
    long userId = 0;
    long idleId = 0;
    std::string collectTime;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Favorite, id, userId, idleId, collectTime)
};

} // namespace models
