cmake_minimum_required(VERSION 3.15)
project(campus_trade_backend)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# 查找依赖
find_package(Drogon CONFIG REQUIRED)
find_package(spdlog REQUIRED)
find_package(yaml-cpp REQUIRED)

# 包含目录
include_directories(${PROJECT_SOURCE_DIR}/src)

# 收集所有源文件
file(GLOB_RECURSE SOURCES 
    "src/controllers/*.cpp"
    "src/services/*.cpp"
    "src/dao/*.cpp"
    "src/utils/*.cpp"
    "src/config/*.cpp"
)

# 可执行文件
add_executable(${PROJECT_NAME} src/main.cpp ${SOURCES})

# 链接库
target_link_libraries(${PROJECT_NAME} PRIVATE Drogon::Drogon
    spdlog::spdlog
    yaml-cpp
)

# 安装
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(DIRECTORY config/ DESTINATION etc/${PROJECT_NAME})
