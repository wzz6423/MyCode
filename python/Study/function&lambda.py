# 1. 基本函数
def greet(name):
    return f"Hello, {name}"


# 2. 默认参数
def power(base, exp=2):
    return base ** exp


# 3. 变长参数
# *args 是一种可变参数语法，表示 “接收任意数量的位置参数”，这些参数会被打包成一个元组（tuple）
def sum_all(*args):  # 元组形式
    return sum(args)


# **kwargs 是一种可变参数语法，表示 “接收任意数量的关键字参数”（即形如 key=value 的参数），这些参数会被打包成一个字典（dict）
def print_info(**kwargs):  # 字典形式
    for k, v in kwargs.items():
        print(f"{k}: {v}")


# 4. Lambda函数
# lambda 参数: 表达式
# 可以带缺省参数
# example1
square = lambda x: x ** 2
# example2
# 高阶函数: 能接受其他函数作为参数的函数
numbers = [1, 2, 3, 4, 5]

# 使用 map() 对列表元素求平方
# map () 函数 ==》 不是 C++的那种std::unordered_map, 此处map是函数
# map(func, iterable) 的作用：
# 对可迭代对象（如列表）中的每个元素应用 func 函数
# 返回一个包含所有处理结果的迭代器
squared = list(map(lambda x: x ** 2, numbers))
print(squared)  # 输出: [1, 4, 9, 16, 25]

# 使用 filter() 筛选偶数
# filter(func, iterable) 的作用：
# 对可迭代对象中的每个元素应用 func 函数
# 只保留使 func 返回 True 的元素
# 返回一个包含筛选结果的迭代器
evens = list(filter(lambda x: x % 2 == 0, numbers))
print(evens)  # 输出: [2, 4]
# map返回所有，filter返回结果为true的
