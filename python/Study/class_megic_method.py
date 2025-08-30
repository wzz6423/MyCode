# 在Python中, 以双下划线__开头和结尾的方法被称为魔术方法(Magic Methods)或特殊方法(Special Methods)
# 这些方法由Python解释器自动调用, 用于实现对象的特定行为, 例如运算符重载、对象初始化、字符串表示等

# 1. 对象创建与销毁
# __new__(cls, *args, **kwargs)创建对象时调用, 负责实例化对象(在__init__之前执行), 通常用于不可变类型(如int、str)的自定义
# __init__(self, *args, **kwargs)初始化对象时调用, 用于设置对象的初始状态(构造方法)
# __del__(self)对象被销毁时调用(析构方法), 用于释放资源, 但不推荐依赖它(垃圾回收机制不确定)

# 2. 字符串表示
# __str__(self)当使用str(obj)或print(obj)时调用, 返回对象的“友好可读”字符串
# __repr__(self)当使用repr(obj)或在交互式解释器中输入对象时调用, 返回对象的“官方”字符串表示(通常可用于重建对象)
# __format__(self, format_spec)当使用format(obj, format_spec)时调用, 用于自定义格式化输出

# 3. 运算符重载
# 算术运算符
# __add__(self, other)：self + other
# __sub__(self, other)：self  other
# __mul__(self, other)：self * other
# __truediv__(self, other)：self / other
# __floordiv__(self, other)：self // other
# __mod__(self, other)：self % other
# __pow__(self, other)：self ** other

# 反向算术运算符(当左操作数不支持时调用)
# __radd__(self, other)：other + self
# __rsub__(self, other)：other  self
# ...(其他类似)

# 比较运算符
# 结合装饰器 @functools.total_ordering
# 简化类的比较方法实现：只需定义__eq__和一个其他比较方法(如__lt__)，自动生成其余比较方法(__le__、__gt__等)
# __eq__(self, other)：self == other
# __ne__(self, other)：self != other
# __lt__(self, other)：self < other
# __gt__(self, other)：self > other
# __le__(self, other)：self <= other
# __ge__(self, other)：self >= other

# 4. 容器相关
# __len__(self)当使用len(obj)时调用, 返回容器中元素的数量
# __getitem__(self, key)当使用obj[key]时调用, 用于获取容器中的元素(支持索引、切片)
# __setitem__(self, key, value)当使用 obj[key] = value 时调用, 用于设置容器中的元素
# __delitem__(self, key)当使用del obj[key]时调用, 用于删除容器中的元素
# __contains__(self, item)当使用item in obj时调用, 判断元素是否在容器中

# 5. 迭代相关
# __iter__(self)当使用iter(obj)时调用, 返回迭代器对象(需实现__next__方法)
# __next__(self)迭代器中调用, 返回下一个元素, 结束时抛出StopIteration
# __reversed__(self)当使用reversed(obj)时调用, 返回反向迭代器

# 6. 调用与属性访问
# __call__(self, *args, **kwargs)当将对象当作函数调用时(如obj())调用 # 仿函数
# __getattr__(self, name)当访问不存在的属性时调用(用于动态属性)
# __getattribute__(self, name)当访问属性时调用(包括存在的和不存在的), 用于自定义属性访问行为
# __setattr__(self, name, value)当设置属性时调用(self.name = value)
# __delattr__(self, name)当删除属性时调用(del self.name)
# __dir__(self)当使用dir(obj)时调用, 返回对象的属性列表

# 7. 上下文管理器
# __enter__(self)进入 with 语句块时调用, 返回上下文对象
# __exit__(self, exc_type, exc_val, exc_tb)退出 with 语句块时调用, 处理异常或清理资源

# 8. 其他常用方法
# __hash__(self)当使 hash(obj) 时调用, 返回对象的哈希值(用于字典键等)
# 注意：若定义了__eq__, 通常需要同时定义__hash__

# __bool__(self)当判断对象真假时(如 if obj)调用, 返回True或False(默认非空对象为True)
# __sizeof__(self)当使用sys.getsizeof(obj)时调用, 返回对象的内存大小(字节)

# ==========================================================================================
# 示例：自定义一个简单的类
class MyNumber:
    def __init__(self, value):
        self.value = value

    def __add__(self, other):
        return MyNumber(self.value + other.value)

    def __str__(self):
        return f"MyNumber({self.value})"


a = MyNumber(10)
b = MyNumber(20)
c = a + b  # 调用 __add__
print(c)  # 调用 __str__, 输出 "MyNumber(30)"

# ==========================================================================================
# 1. 初始化与字符串表示
class Person:
    def __init__(self, name, age):  # 初始化方法
        self.name = name
        self.age = age

    def __str__(self):  # 友好字符串表示
        return f"Person: {self.name}, {self.age}岁"

    def __repr__(self):  # 官方字符串表示
        return f"Person('{self.name}', {self.age})"


p = Person("Alice", 30)
print(p)  # 调用 __str__ → "Person: Alice, 30岁"
print(repr(p))  # 调用 __repr__ → "Person('Alice', 30)"

# 2. 运算符重载
class Vector:
    def __init__(self, x, y):
        self.x = x
        self.y = y

    # 加法重载
    def __add__(self, other):
        return Vector(self.x + other.x, self.y + other.y)

    # 比较重载
    def __eq__(self, other):
        return self.x == other.x and self.y == other.y

    def __str__(self):
        return f"Vector({self.x}, {self.y})"


v1 = Vector(2, 3)
v2 = Vector(4, 5)
v3 = v1 + v2  # 调用 __add__
print(v3)  # 输出 "Vector(6, 8)"
print(v1 == v2)  # 调用 __eq__ → False

# 3. 容器行为模拟
class MyList:
    def __init__(self):
        self.data = []

    def __len__(self):  # 支持 len()
        return len(self.data)

    def __getitem__(self, index):  # 支持 [] 访问
        return self.data[index]

    def __setitem__(self, index, value):  # 支持 [] 赋值
        self.data[index] = value

    def __contains__(self, item):  # 支持 in 操作
        return item in self.data


ml = MyList()
ml.data = [1, 2, 3]
print(len(ml))  # 调用 __len__ → 3
print(ml[1])  # 调用 __getitem__ → 2
ml[0] = 100
print(ml[0])  # 调用 __setitem__ → 100
print(2 in ml)  # 调用 __contains__ → True

# 4. 迭代器行为
class CountDown:
    def __init__(self, start):
        self.start = start

    def __iter__(self):  # 返回迭代器
        self.current = self.start
        return self

    def __next__(self):  # 迭代逻辑
        if self.current < 0:
            raise StopIteration
        result = self.current
        self.current -= 1
        return result


# 使用迭代器
for i in CountDown(3):
    print(i)  # 输出: 3, 2, 1, 0

# 5. 上下文管理器（with 语句）
class FileHandler:
    def __init__(self, filename, mode):
        self.filename = filename
        self.mode = mode

    def __enter__(self):  # 进入 with 块
        self.file = open(self.filename, self.mode)
        return self.file

    def __exit__(self, exc_type, exc_val, exc_tb):  # 退出 with 块
        self.file.close()
        # 可以在这里处理异常


# 使用上下文管理器
with FileHandler("test.txt", "w") as f:
    f.write("Hello, Magic Methods!")
# 文件会自动关闭

# 6. 调用行为（模拟函数）
class Adder:
    def __init__(self, num):
        self.num = num

    def __call__(self, x):  # 允许对象像函数一样被调用
        return self.num + x


add5 = Adder(5)
print(add5(3))  # 调用 __call__ → 8
print(add5(10))  # → 15
