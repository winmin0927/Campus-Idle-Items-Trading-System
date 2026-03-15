#pragma once

#include <stdexcept>
#include <unordered_map>
#include <string>

namespace exception {

/**
 * 参数异常类
 */
class ParamException : public std::runtime_error {
public:
    explicit ParamException(const std::unordered_map<std::string, std::string>& errorMap)
        : std::runtime_error("Parameter validation failed"), map(errorMap) {}
    
    const std::unordered_map<std::string, std::string>& getMap() const {
        return map;
    }
    
private:
    std::unordered_map<std::string, std::string> map;
};

} // namespace exception
