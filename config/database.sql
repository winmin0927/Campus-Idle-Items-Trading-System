-- 创建数据库
CREATE DATABASE IF NOT EXISTS campus_trade CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

USE campus_trade;

-- 用户表
CREATE TABLE user (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    account_number VARCHAR(50) UNIQUE NOT NULL,
    user_password VARCHAR(255) NOT NULL,
    nickname VARCHAR(50),
    avatar VARCHAR(500),
    sign_in_time DATETIME,
    user_status TINYINT DEFAULT 0
);

-- 管理员表
CREATE TABLE admin (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    account_number VARCHAR(50) UNIQUE NOT NULL,
    admin_password VARCHAR(255) NOT NULL,
    admin_name VARCHAR(50)
);

-- 地址表
CREATE TABLE address (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    consignee_name VARCHAR(50),
    consignee_phone VARCHAR(20),
    province_name VARCHAR(50),
    city_name VARCHAR(50),
    region_name VARCHAR(50),
    detail_address VARCHAR(200),
    default_flag BOOLEAN DEFAULT FALSE,
    user_id BIGINT
);

-- 闲置商品表
CREATE TABLE idle_item (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    user_id BIGINT,
    idle_name VARCHAR(100),
    idle_details TEXT,
    picture_list TEXT,
    idle_price DECIMAL(10,2),
    idle_place VARCHAR(100),
    idle_label INT,
    idle_status TINYINT DEFAULT 1,
    release_time DATETIME
);

-- 订单表
CREATE TABLE `order` (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    order_number VARCHAR(50) UNIQUE,
    user_id BIGINT,
    idle_id BIGINT,
    address_id BIGINT,
    order_status TINYINT DEFAULT 0,
    payment_status TINYINT DEFAULT 0,
    create_time DATETIME,
    payment_time DATETIME
);

-- 订单地址表
CREATE TABLE order_address (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    order_id BIGINT,
    consignee_name VARCHAR(50),
    consignee_phone VARCHAR(20),
    province_name VARCHAR(50),
    city_name VARCHAR(50),
    region_name VARCHAR(50),
    detail_address VARCHAR(200)
);

-- 收藏表
CREATE TABLE favorite (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    user_id BIGINT,
    idle_id BIGINT,
    create_time DATETIME
);

-- 留言表
CREATE TABLE message (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    user_id BIGINT,
    idle_id BIGINT,
    content TEXT,
    create_time DATETIME,
    to_user BIGINT,
    to_message BIGINT
);