# **
# 指数运算符
result = 2 ** 3  # 表示 2 的 3 次方，结果为 8
print(9 ** 0.5)  # 表示 9 的平方根，结果为 3.0


# 关键字参数解包 **：用于将字典中的键值对解包为函数的关键字参数
def person_info(name, age, city):
    print(f"{name} is {age} years old, living in {city}")


info = {"name": "Alice", "age": 30, "city": "New York"}
person_info(**info)  # 等价于 person_info(name="Alice", age=30, city="New York")
# 此处字典中参数名必须匹配

# ========================================================================================================
