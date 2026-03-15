#pragma once

#include <vector>
#include <nlohmann/json.hpp>

namespace vo {

template<typename T>
struct PageVo {
    std::vector<T> list;
    int count;
    
    PageVo() : count(0) {}
    
    PageVo(const std::vector<T>& l, int c) : list(l), count(c) {}
    
    // JSON序列化
    friend void to_json(nlohmann::json& j, const PageVo<T>& p) {
        j = nlohmann::json{{"list", p.list}, {"count", p.count}};
    }
    
    friend void from_json(const nlohmann::json& j, PageVo<T>& p) {
        j.at("list").get_to(p.list);
        j.at("count").get_to(p.count);
    }
};

} // namespace vo
