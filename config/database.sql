-- 删除数据库
DROP DATABASE IF EXISTS campus_trade;

-- 创建数据库
CREATE DATABASE IF NOT EXISTS campus_trade CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

USE campus_trade;

-- 用户表
CREATE TABLE IF NOT EXISTS user (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    accountNumber VARCHAR(50) UNIQUE NOT NULL,
    userPassword VARCHAR(255) NOT NULL,
    nickname VARCHAR(50),
    avatar VARCHAR(10),
    signInTime DATETIME,
    userStatus TINYINT DEFAULT 0
);

-- 管理员表
CREATE TABLE IF NOT EXISTS admin (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    accountNumber VARCHAR(50) UNIQUE NOT NULL,
    adminPassword VARCHAR(255) NOT NULL,
    adminName VARCHAR(50)
);

-- 地址表
CREATE TABLE IF NOT EXISTS address (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    consigneeName VARCHAR(50),
    consigneePhone VARCHAR(20),
    provinceName VARCHAR(50),
    cityName VARCHAR(50),
    regionName VARCHAR(50),
    detailAddress VARCHAR(200),
    defaultFlag BOOLEAN DEFAULT FALSE,
    userId BIGINT
);

-- 闲置商品表
CREATE TABLE IF NOT EXISTS idle_item (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    userId BIGINT,
    idleName VARCHAR(100),
    idleDetails TEXT,
    pictureList TEXT,
    idlePrice DECIMAL(10,2),
    idlePlace VARCHAR(100),
    idleLabel INT,
    idleStatus TINYINT DEFAULT 1,
    releaseTime DATETIME
);

-- 订单表（注意 order 是 MySQL 保留字，需加反引号）
CREATE TABLE IF NOT EXISTS `order` (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    orderNumber VARCHAR(50) UNIQUE,
    userId BIGINT,
    idleId BIGINT,
    addressId BIGINT,
    orderStatus TINYINT DEFAULT 0,
    paymentStatus TINYINT DEFAULT 0,
    createTime DATETIME,
    paymentTime DATETIME
);

-- 订单地址表
CREATE TABLE IF NOT EXISTS order_address (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    orderId BIGINT,
    consigneeName VARCHAR(50),
    consigneePhone VARCHAR(20),
    provinceName VARCHAR(50),
    cityName VARCHAR(50),
    regionName VARCHAR(50),
    detailAddress VARCHAR(200)
);

-- 收藏表
CREATE TABLE IF NOT EXISTS favorite (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    userId BIGINT,
    idleId BIGINT,
    collectTime DATETIME
);

-- 留言表
CREATE TABLE IF NOT EXISTS message (
    id BIGINT PRIMARY KEY AUTO_INCREMENT,
    userId BIGINT,
    idleId BIGINT,
    content TEXT,
    createTime DATETIME,
    toUser BIGINT,
    toMessage BIGINT
);

-- ==================== 插入 5 条测试数据 ====================

-- 用户表数据
INSERT INTO user (id, accountNumber, userPassword, nickname, avatar, signInTime, userStatus) VALUES
(1, '20240001', 'password123', '张三', '', '2025-03-01 10:00:00', 0),
(2, '20240002', 'password123', '李四', '', '2025-03-02 11:30:00', 0),
(3, '20240003', 'password123', '王五', '', '2025-03-03 09:20:00', 1),
(4, '20240004', 'password123', '赵六', '', '2025-03-04 14:15:00', 0),
(5, '20240005', 'password123', '小明', '', '2025-03-05 08:45:00', 0);

-- 管理员表数据
INSERT INTO admin (id, accountNumber, adminPassword, adminName) VALUES
(1, 'admin01', 'adminpass', '超级管理员'),
(2, 'admin02', 'adminpass', '内容管理员'),
(3, 'admin03', 'adminpass', '订单管理员'),
(4, 'admin04', 'adminpass', '用户管理员'),
(5, 'admin05', 'adminpass', '审核员');

-- 地址表数据（确保每个用户有一个地址）
INSERT INTO address (id, consigneeName, consigneePhone, provinceName, cityName, regionName, detailAddress, defaultFlag, userId) VALUES
(1, '张三', '13800138001', '北京市', '北京市', '海淀区', '中关村大街1号', TRUE, 1),
(2, '李四', '13800138002', '上海市', '上海市', '浦东新区', '陆家嘴环路100号', TRUE, 2),
(3, '王五', '13800138003', '广东省', '广州市', '天河区', '体育西路100号', TRUE, 3),
(4, '赵六', '13800138004', '广东省', '深圳市', '南山区', '科技园路1号', TRUE, 4),
(5, '小明', '13800138005', '北京市', '北京市', '朝阳区', '建国门外大街1号', TRUE, 5);

-- 闲置商品表数据
INSERT INTO idle_item (id, userId, idleName, idleDetails, pictureList, idlePrice, idlePlace, idleLabel, idleStatus, releaseTime) VALUES
(1, 1, 'ThinkPad 笔记本电脑', '九成新，i5处理器，8G内存，256G固态', '', 2500.00, '北京', 1, 1, '2025-03-10 10:00:00'),
(2, 2, 'iPhone 12', '国行，电池健康85%，带原装充电器', '', 3200.00, '上海', 1, 1, '2025-03-11 14:30:00'),
(3, 3, '高等数学教材', '全新未使用，第六版', '', 30.00, '广州', 2, 1, '2025-03-12 09:00:00'),
(4, 4, '山地自行车', '21速，铝合金车架，九成新', '', 800.00, '深圳', 3, 1, '2025-03-13 16:20:00'),
(5, 5, '篮球', '斯伯丁7号球，室内外通用，使用次数少', '', 100.00, '北京', 4, 1, '2025-03-14 11:10:00');

-- 订单表数据
INSERT INTO `order` (id, orderNumber, userId, idleId, addressId, orderStatus, paymentStatus, createTime, paymentTime) VALUES
(1, 'ORD20250318001', 2, 1, 2, 1, 1, '2025-03-15 09:30:00', '2025-03-15 09:35:00'),
(2, 'ORD20250318002', 3, 2, 3, 2, 1, '2025-03-16 10:20:00', '2025-03-16 10:25:00'),
(3, 'ORD20250318003', 4, 3, 4, 0, 0, '2025-03-17 11:00:00', NULL);

-- 订单地址表数据（订单收货地址快照）
INSERT INTO order_address (id, orderId, consigneeName, consigneePhone, provinceName, cityName, regionName, detailAddress) VALUES
(1, 1, '李四', '13800138002', '上海市', '上海市', '浦东新区', '陆家嘴环路100号'),
(2, 2, '王五', '13800138003', '广东省', '广州市', '天河区', '体育西路100号'),
(3, 3, '赵六', '13800138004', '广东省', '深圳市', '南山区', '科技园路1号'),
(4, 4, '小明', '13800138005', '北京市', '北京市', '朝阳区', '建国门外大街1号'),
(5, 5, '张三', '13800138001', '北京市', '北京市', '海淀区', '中关村大街1号');

-- 收藏表数据
INSERT INTO favorite (id, userId, idleId, collectTime) VALUES
(1, 1, 2, '2025-03-16 12:00:00'),
(2, 2, 3, '2025-03-16 13:00:00'),
(3, 3, 4, '2025-03-17 09:30:00'),
(4, 4, 5, '2025-03-17 10:20:00'),
(5, 5, 1, '2025-03-18 08:15:00');

-- 留言表数据（包含回复）
INSERT INTO message (id, userId, idleId, content, createTime, toUser, toMessage) VALUES
(1, 2, 1, '请问电脑还在吗？', '2025-03-15 10:00:00', NULL, NULL),
(2, 1, 1, '还在的，需要的话可以私聊', '2025-03-15 10:05:00', 2, 1),
(3, 3, 2, '手机有划痕吗？', '2025-03-16 11:00:00', NULL, NULL),
(4, 2, 2, '没有，一直戴壳使用', '2025-03-16 11:10:00', 3, 3),
(5, 5, 4, '自行车能小刀吗？', '2025-03-18 09:30:00', NULL, NULL);