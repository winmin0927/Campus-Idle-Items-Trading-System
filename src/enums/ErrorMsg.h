#pragma once

#include <string>

namespace enums {

class ErrorMsg {
public:
    static const ErrorMsg EMAIL_LOGIN_ERROR;
    static const ErrorMsg COOKIE_ERROR;
    static const ErrorMsg PARAM_ERROR;
    static const ErrorMsg SYSTEM_ERROR;
    static const ErrorMsg FILE_UPLOAD_ERROR;
    static const ErrorMsg FAVORITE_EXIT;
    static const ErrorMsg REGISTER_ERROR;
    static const ErrorMsg ACCOUNT_BAN;
    static const ErrorMsg PASSWORD_RESET_ERROR;
    
    std::string getMessage() const { return message; }
    
private:
    ErrorMsg(const std::string& msg) : message(msg) {}
    std::string message;
};

inline const ErrorMsg ErrorMsg::EMAIL_LOGIN_ERROR("账号或密码错误");
inline const ErrorMsg ErrorMsg::COOKIE_ERROR("登录异常 请重新登录");
inline const ErrorMsg ErrorMsg::PARAM_ERROR("参数错误");
inline const ErrorMsg ErrorMsg::SYSTEM_ERROR("系统错误");
inline const ErrorMsg ErrorMsg::FILE_UPLOAD_ERROR("文件上传失败");
inline const ErrorMsg ErrorMsg::FAVORITE_EXIT("该商品已收藏");
inline const ErrorMsg ErrorMsg::REGISTER_ERROR("注册失败");
inline const ErrorMsg ErrorMsg::ACCOUNT_BAN("账号已被封禁");
inline const ErrorMsg ErrorMsg::PASSWORD_RESET_ERROR("密码修改失败");

} // namespace enums
