# 异常处理
# try:
#     # 可能会发生异常的代码块
#     risky_operation()
# except ExceptionType1:
#     # 处理特定类型的异常
#     handle_exception_type1()
# except ExceptionType2 as e:
#     # 捕获异常并获取异常信息
#     print(f"发生错误: {e}")
# else:
#     # 如果没有发生异常，会执行这里的代码
#     print("一切正常")
# finally:
#     # 无论是否发生异常，都会执行这里的代码
#     print("清理工作")

# 常见异常
# TypeError：类型错误（如字符串和数字相加）
# ValueError：值错误（如int("abc")）
# ZeroDivisionError：除零错误
# IndexError：索引越界
# KeyError：字典键不存在
# FileNotFoundError：文件未找到

# example
# 示例1：基本异常处理
try:
    num = int(input("请输入一个数字: "))
    result = 10 / num
    print(f"10除以{num}的结果是: {result}")
except ValueError:
    print("输入错误：请输入有效的数字")
except ZeroDivisionError:
    print("错误：不能除以零")
except Exception as e:
    print(f"发生未知错误: {e}")
else:
    print("计算成功完成")
finally:
    print("程序执行结束")
# 示例2：主动抛出异常
def check_age(age):
    if age < 0:
        # 主动抛出异常
        raise ValueError("年龄不能为负数")
    return age

try:
    check_age(-5)
except ValueError as e:
    print(f"错误: {e}")

# 自定义异常类型
class CustomError(Exception):
    """自定义异常类"""
    def __init__(self, message):
        self.message = message

try:
    raise CustomError("这是一个自定义异常")
except CustomError as e:
    print(f"捕获到自定义异常: {e.message}")