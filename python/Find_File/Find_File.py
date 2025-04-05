import os

file_path = input("请输入您想查找的路径: ")
file_name = input("请输入您想查找文件的关键字: ")

for dirpath, _, filesname in os.walk(file_path):
    for f in filesname:
        if file_name in f:
            print(f"{dirpath}\{f}")