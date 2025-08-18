# 日志模块脚手架封装

import logging
import os
import time


# 日志过滤器
class infoFilter(logging.Filter):
    def filter(self, record):
        return record.levelno == logging.INFO


class errorFilter(logging.Filter):
    def filter(self, record):
        return record.levelno == logging.ERROR


class warningFilter(logging.Filter):
    def filter(self, record):
        return record.levelno == logging.WARNING


class criticalFilter(logging.Filter):
    def filter(self, record):
        return record.levelno == logging.CRITICAL


class logger:
    # 获取日志对象
    @classmethod
    def logger(cls):
        # 创建日志对象
        cls.logger = logging.getLogger(__name__)
        # 设置日志级别
        cls.logger.setLevel(logging.DEBUG)

        # 保证 logs 目录存在
        logsPath = "./logs/"
        if os.path.exists(logsPath):
            os.mkdir(logsPath)

        # 将日志输出到文件
        """
        ./logs/
        
        time.log
        time-info.log
        time-error.log
        time-warning.log
        time-critical.log
        """

        now = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())
        logName = logsPath + now + ".log"
        infoName = logsPath + now + "-info.log"
        errorName = logsPath + now + "-error.log"
        warningName = logsPath + now + "-warning.log"
        criticalName = logsPath + now + "-critical.log"

        # 创建文件处理器
        allHandler = logging.FileHandler(logName, encoding="utf-8")
        infoHandler = logging.FileHandler(infoName, encoding="utf-8")
        errorHandler = logging.FileHandler(errorName, encoding="utf-8")
        warningHandler = logging.FileHandler(warningName, encoding="utf-8")
        criticalHandler = logging.FileHandler(criticalName, encoding="utf-8")

        # 设置日志格式
        formatter = logging.Formatter(
            "%(asctime)s - %(levelname)s - [%(name)s] - [%(filename)s - %(funcName)s:%(lineno)d] - %(message)s")
        allHandler.setFormatter(formatter)
        infoHandler.setFormatter(formatter)
        errorHandler.setFormatter(formatter)
        warningHandler.setFormatter(formatter)
        criticalHandler.setFormatter(formatter)

        # 添加日志过滤器
        infoHandler.addFilter(infoFilter())
        errorHandler.addFilter(errorFilter())
        warningHandler.addFilter(warningFilter())
        criticalHandler.addFilter(criticalFilter())

        # 添加文件处理器
        cls.logger.addHandler(allHandler)
        cls.logger.addHandler(infoHandler)
        cls.logger.addHandler(errorHandler)
        cls.logger.addHandler(warningHandler)
        cls.logger.addHandler(criticalHandler)

        return cls.logger
