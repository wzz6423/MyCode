CREATE DATABASE IF NOT EXISTS cloud_disk
    DEFAULT CHARACTER SET utf8mb4
    DEFAULT COLLATE utf8mb4_unicode_ci;

USE cloud_disk;

CREATE TABLE IF NOT EXISTS users (
    id          BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    user_id     VARCHAR(64)  NOT NULL UNIQUE,
    nickname    VARCHAR(128) NOT NULL DEFAULT '',
    description VARCHAR(512) NOT NULL DEFAULT '',
    phone       VARCHAR(20)  NULL DEFAULT NULL,
    email       VARCHAR(128) NULL DEFAULT NULL,
    password    VARCHAR(256) NOT NULL,
    avatar_url  VARCHAR(512) NOT NULL DEFAULT '',
    created_at  DATETIME     NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at  DATETIME     NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    UNIQUE INDEX idx_phone (phone),
    UNIQUE INDEX idx_email (email)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE IF NOT EXISTS files (
    id             BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    file_id        VARCHAR(64)   NOT NULL UNIQUE,
    user_id        VARCHAR(64)   NOT NULL,
    file_name      VARCHAR(512)  NOT NULL,
    file_type      TINYINT UNSIGNED NOT NULL DEFAULT 0,
    file_size      BIGINT UNSIGNED  NOT NULL DEFAULT 0,
    storage_path   VARCHAR(1024) NOT NULL DEFAULT '',
    parent_path    VARCHAR(1024) NOT NULL DEFAULT '/',
    source         TINYINT UNSIGNED NOT NULL DEFAULT 0,
    source_user_id VARCHAR(64)   NOT NULL DEFAULT '',
    unique_flag    VARCHAR(128)  NOT NULL DEFAULT '',
    is_shared      TINYINT(1)    NOT NULL DEFAULT 0,
    is_public      TINYINT(1)    NOT NULL DEFAULT 0,
    created_at     DATETIME      NOT NULL DEFAULT CURRENT_TIMESTAMP,
    updated_at     DATETIME      NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    INDEX idx_user_path (user_id, parent_path(255)),
    INDEX idx_unique_flag (unique_flag),
    INDEX idx_public (is_public)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

CREATE TABLE IF NOT EXISTS shares (
    id            BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    share_id      VARCHAR(64)    NOT NULL UNIQUE,
    user_id       VARCHAR(64)    NOT NULL,
    file_id       VARCHAR(64)    NOT NULL,
    share_type    TINYINT UNSIGNED NOT NULL,
    message       VARCHAR(512)   NOT NULL DEFAULT '',
    expire_at     DATETIME       NULL,
    max_count     INT UNSIGNED   NOT NULL DEFAULT 0,
    current_count INT UNSIGNED   NOT NULL DEFAULT 0,
    created_at    DATETIME       NOT NULL DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_file (file_id),
    INDEX idx_user (user_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
