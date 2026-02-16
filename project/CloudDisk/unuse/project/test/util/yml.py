# yaml 模块封装

import os
import yaml


# 往 yaml 文件中写入数据
def write(filename, data):
    with open(os.getcwd() + "/data/" + filename, mode="a+", encoding="utf-8") as f:
        yaml.safe_dump(data, stream=f)


# 读取 yaml 文件中的数据
def read(filename, key):
    with open(os.getcwd() + "/data/" + filename, mode="r", encoding="utf-8") as f:
        data = yaml.safe_load(f)
        return data[key]


# 清空
def clear(filename):
    with open(os.getcwd() + "/data/" + filename, mode="w", encoding="utf-8") as f:
        f.truncate()
