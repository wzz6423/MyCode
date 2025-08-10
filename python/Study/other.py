# 类型联合（Type Union，Python 3.10+）
# 类型联合是 Python 3.10 引入的语法糖，用于表示一个变量 / 返回值可以是多种类型中的一种，解决了传统 Union 类型写法繁琐的问题。
#
# 核心语法：
# 使用 | 符号分隔多个类型，例如 int | str 表示 “要么是 int 类型，要么是 str 类型”。
#
# 这与 Python 3.9 及之前版本的 Union[int, str] 功能完全一致，但语法更简洁（Union 需要从 typing 模块导入）。
import random

def get_value() -> int | str:  # 返回值可以是 int 或 str
    return "100" if random() > 0.5 else 100
# 函数 get_value 的返回值类型被声明为 int | str，表示调用该函数时，可能得到整数（如 100）或字符串（如 "100"）。
# 作用：帮助开发者和类型检查工具（如 mypy）明确变量的可能类型，减少类型错误


#  结构模式匹配（Structural Pattern Matching，Python 3.10+）
# 它可以匹配数据结构的形状（而非仅仅值），尤其适合处理复杂数据（如列表、字典、对象）
# match 待匹配对象:
#     case 模式1:
#         处理逻辑1
#     case 模式2:
#         处理逻辑2
#     case _:  # 通配符，匹配所有未被前面模式捕获的情况（类似 default）
#         默认逻辑
def handle_command(command):
    match command.split():  # 对命令字符串分割后的列表进行匹配
        case ["go", direction]:  # 匹配 ["go", 任意值] 结构
            print(f"向{direction}移动")
        case ["take", *items]:  # 匹配 ["take", 剩余所有元素] 结构（* 捕获多个元素）
            print(f"捡起{items}")
        case _:  # 匹配所有其他情况
            print("未知命令")
# 当 command 为 "go east" 时，split() 结果为 ["go", "east"]，匹配第一个 case，输出 “向东移动”。
# 当 command 为 "take apple book" 时，split() 结果为 ["take", "apple", "book"]，*items 捕获 ["apple", "book"]，
# 输出 “捡起 ['apple', 'book']”

# 支持匹配固定长度的序列（如 [x, y, z]）、带通配符的序列（如 [x, *y] 捕获剩余元素）。
# 支持匹配字典（如 {"name": n, "age": a} 捕获键对应的值）。
# 支持类型匹配（如 case int(x) 仅匹配整数并捕获值）


# Python 的模块与包管理
# 1. 模块（Module）
# 模块是 Python 代码的基本组织单位，本质是一个后缀为 .py 的文件，包含函数、类、变量等代码。
# 作用：将代码拆分到不同文件，实现逻辑隔离和复用。

# 使用模块：
# 通过 import 语句导入模块并使用其内容，常见用法：
# 1. 导入整个模块
import math  # 导入标准库模块 math
print(math.sqrt(4))  # 使用模块中的函数

# 2. 导入模块中的特定成员
from math import sqrt, pi
print(sqrt(4), pi)

# 3. 给模块/成员起别名
import math as m
from math import sqrt as square_root
print(m.pi, square_root(9))

# 4. 导入模块中所有成员（不推荐，可能引发命名冲突）
from math import *

#  包（Package）
# 包是多个相关模块的集合，本质是一个包含 __init__.py 文件的目录（Python 3.3+ 后 __init__.py 可省略，但建议保留用于标识包）。
# 作用：进一步组织模块，避免同名模块的冲突
# 1. 绝对导入（推荐）：从包的根目录开始导入
# my_package/          # 包目录
# ├── __init__.py      # 包初始化文件（可空，也可定义包的公共接口）
# ├── module1.py       # 模块1
# └── module2.py       # 模块2
from my_package.module1 import func1

# 2. 相对导入（仅在包内部使用）：用 . 表示当前目录，.. 表示父目录
# 在 module2.py 中导入同包的 module1
from .module1 import func1

# __init__.py 的作用：
# 标识该目录为 Python 包。
# 控制包的导入行为，例如定义 __all__ 变量指定 from package import * 时导入的模块
# my_package/__init__.py
__all__ = ["module1"]  # 仅允许导入 module1


