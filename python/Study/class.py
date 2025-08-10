# 在 Python 中，类的成员（包括属性和方法）不需要显式声明
# 通常在 __init__ 方法中通过 self.属性名 = 值 的方式创建
# 也可以在类的其他方法中，甚至类外部动态添加
# 1. 属性不需要显式声明
# 实例属性
# 通常在 __init__ 方法中通过 self.属性名 = 值 的方式创建
# 也可以在类的其他方法中，甚至类外部动态添加
class Person:
    def __init__(self, name):
        # 在这里动态创建name属性，无需提前声明
        self.name = name

p = Person("Alice")

# 在类外部动态添加新属性，无需在类中声明
p.age = 25
print(p.age)  # 输出：25

# 动态修改属性
p.name = "Bob"
print(p.name)  # 输出：Bob

# 类属性
# 直接在类中定义（无需声明类型），同样可以动态修改
class Car:
    # 类属性，直接定义即可，无需声明
    wheels = 4

# 动态修改类属性
Car.wheels = 6
print(Car.wheels)  # 输出：6

# 2. 方法不需要显式声明
# 方法在类中定义时，无需像静态语言那样声明返回值类型或参数类型
# 可以在运行时动态给类添加方法
class Student:
    # 方法定义无需声明参数类型和返回值类型
    def study(self, subject):
        print(f"学习{subject}")

# 动态给类添加新方法
def play(self, game):
    print(f"玩{game}")

Student.play = play

s = Student()
s.study("数学")  # 输出：学习数学
s.play("篮球")   # 输出：玩篮球（动态添加的方法）
# 私有属性的 “伪声明”：通过双下划线（如 __secret）可以模拟私有属性，但这不是真正的声明，而是 Python 的名称修饰机制

# 定义一个基础类（父类）
class Animal:
    # 类属性：所有该类的实例共享此属性
    # 属于类本身，而非某个具体实例
    kingdom = "动物界"
    _total_count = 0  # 受保护的类属性（约定用单下划线，不建议类外直接修改）

    # 构造方法（初始化方法）
    # 当创建类的实例时，会自动调用此方法
    # self 代表当前实例本身，必须作为第一个参数
    def __init__(self, name, age=1):
        # 实例属性：每个实例独立拥有的属性
        self.name = name  # 公开属性：可以直接通过实例访问和修改

        # 私有属性：双下划线开头，Python会进行名称修饰
        # 类外部无法直接访问（如 animal.__age 会报错）
        # 用于封装不希望外部直接修改的数据
        self.__age = age

        # 操作类属性：每创建一个实例，总数加1
        Animal._total_count += 1

    # 实例方法：用于操作实例属性的函数
    # 第一个参数必须是self，代表调用该方法的实例
    def get_age(self):
        """获取私有属性__age的值（getter方法）"""
        return self.__age

    def set_age(self, age):
        """设置私有属性__age的值（setter方法），包含数据校验"""
        if age > 0 and isinstance(age, int):  # 确保年龄是正整数
            self.__age = age
        else:
            # 如果数据不合法，抛出异常
            raise ValueError("年龄必须是正整数")

    # Python 不支持函数重载
    # 使用 @ 的语法是装饰器
    # 用于在不修改原函数代码的前提下，动态增强函数 / 方法的功能。
    # @装饰器名放在函数定义上方，相当于将函数作为参数传递给装饰器，并替换为装饰器的返回值
    # @classmethod: 将方法转换为类方法（调用时传入类本身作为第一个参数cls）
    # @staticmethod: 将方法转换为静态方法（不依赖实例或类，无默认参数self/cls）
    # @functools.wraps: 保留被装饰函数的元数据（如__name__、__doc__），常用于自定义装饰器中。
    # @functools.lru_cache: 缓存函数调用结果（基于参数），实现 “记忆化”，优化重复调用的性能。
    # @functools.total_ordering: 简化类的比较方法实现：只需定义__eq__和一个其他比较方法（如__lt__），自动生成其余比较方法（__le__、__gt__等）。
    # @abc.abstractmethod: 声明抽象方法，抽象基类（ABC）的子类必须实现该方法，否则无法实例化。
    # @contextlib.contextmanager: 将生成器函数转换为上下文管理器（支持with语句），简化资源管理（如文件、锁）。
    # @functools.singledispatch: 实现单分派泛型函数：根据第一个参数的类型，自动选择对应的函数实现。
    # @functools.singledispatchmethod: 用于类方法的单分派（支持 @ classmethod / @ staticmethod结合）。
    # @typing.overload: 配合类型提示，声明函数的 “重载签名”（无实际实现），用于标注不同参数组合的返回值类型

    #有一个计算加法的函数
    # def add(a, b):
    #     return a + b
    # 想给它增加 “打印输入参数” 和 “打印计算结果” 的功能，但不想改动add函数内部的代码。
    # 就可以用装饰器来实现：通过外部代码 “包装” 原函数，添加新功能，原函数本身的代码保持不变。
    # 定义一个装饰器（功能：打印函数调用信息）
    # def log_decorator(func):
    #     def wrapper(*args, **kwargs):
    #         print(f"调用函数：{func.__name__}，参数：{args}")
    #         result = func(*args, **kwargs)  # 调用原函数
    #         print(f"函数返回值：{result}")
    #         return result
    #
    #     return wrapper
    #
    # # 使用 @ 语法应用装饰器
    # @log_decorator
    # def add(a, b):
    #     return a + b
    # @log_decorator 就是装饰器的语法糖，它等价于手动执行：add = log_decorator(add)
    # 当执行 @ log_decorator时，Python会把下面的add函数作为参数传给log_decorator函数。
    # log_decorator函数内部定义了一个新函数wrapper（这个函数包含了增强的功能，比如打印日志），并将
    # wrapper作为返回值。最终，原来的add函数被替换成了wrapper函数。
    # 所以当调用add(1, 2)时，实际执行的是wrapper(1, 2)，从而既保留了原函数的加法功能，又新增了日志打印功能。

    # 装饰器参数必须是*args, **kwargs, *args是位置参数(直接按照形参位置传参)，**kwargs是关键字参数(按照参数名传参)
    # 所以装饰器可以接收任意数量的位置参数和关键字参数
    # 位置参数与传参类型是普通参数/元组/列表/字典/...无关，如果要使用字典+关键字参数传参不能直接传而要写为**字典名
    # 无论使用装饰器还是普通函数， 只要是使用字典+关键字参数传参都不能直接传而要写为**字典名

    # 属性装饰器：将方法转换为属性访问方式
    # 相当于更优雅的getter和setter
    @property
    def age(self):
        """以属性方式获取年龄，替代get_age()方法"""
        return self.__age

    @age.setter
    def age(self, age):
        """以属性方式设置年龄，替代set_age()方法"""
        if age > 0 and isinstance(age, int):
            self.__age = age
        else:
            raise ValueError("年龄必须是正整数")

    # 类方法：用于操作类属性的方法
    # 用@classmethod装饰，第一个参数是cls，代表类本身
    @classmethod
    def get_total_count(cls):
        """返回当前创建的所有Animal实例的总数"""
        return cls._total_count  # cls代表调用该方法的类（Animal或其子类）

    @classmethod
    def update_kingdom(cls, new_kingdom):
        """修改类属性kingdom的值"""
        cls.kingdom = new_kingdom

    # 静态方法：与类和实例都无关的工具方法
    # 用@staticmethod装饰，没有默认参数（不需要self或cls）
    @staticmethod
    def is_adult(age):
        """判断动物是否成年（通用逻辑，与特定实例无关）"""
        return age >= 3  # 假设3岁以上为成年

    # 普通实例方法：定义类的行为
    def make_sound(self):
        """动物发出声音的通用方法"""
        print(f"{self.name}发出了声音")

    # 魔术方法（特殊方法）：以双下划线开头和结尾
    # 用于实现Python的一些内置功能
    def __str__(self):
        """
        定义实例的字符串表示
        当使用print()打印实例或str()转换时调用
        应返回用户友好的描述
        """
        return f"{self.kingdom}的{self.name}，{self.age}岁"

    def __repr__(self):
        """
        定义实例的官方字符串表示
        当在解释器中直接输入实例或repr()转换时调用
        应返回能准确表示实例的字符串（通常可用于重建实例）
        """
        return f"Animal(name='{self.name}', age={self.age})"

    def __del__(self):
        """
        析构方法：实例被销毁时自动调用
        通常用于释放资源（如文件、网络连接等）
        """
        Animal._total_count -= 1  # 实例销毁时，总数减1
        # print(f"{self.name}被销毁了")  # 可以取消注释查看效果


# 继承：Dog类继承自Animal类
# 子类会获得父类的所有属性和方法，还可以添加自己的特性
class Dog(Animal):
    # 子类的类属性：仅属于Dog类及其实例
    species = "犬科"

    # 子类的构造方法
    def __init__(self, name, breed, age=1):
        # 调用父类的构造方法，初始化继承的属性
        # super() 用于获取父类对象
        super().__init__(name, age)

        # 子类特有的实例属性
        self.breed = breed  # 犬种

    # 方法重写：子类重新定义父类的方法
    # 实现多态：相同方法名在不同类中有不同实现
    def make_sound(self):
        """重写父类方法，实现狗的特有叫声"""
        print(f"{self.name}（{self.breed}）汪汪叫！")

    # 子类特有的方法：父类没有的功能
    def fetch(self, item):
        """狗的特有行为：叼东西"""
        print(f"{self.name}叼来了{item}")


# 另一个子类：猫
class Cat(Animal):
    species = "猫科"

    def __init__(self, name, color, age=1):
        super().__init__(name, age)
        self.color = color  # 猫的颜色

    # 重写父类方法
    def make_sound(self):
        print(f"{self.name}（{self.color}色）喵喵叫！")

    # 子类特有方法
    def climb(self):
        print(f"{self.name}爬上了树")


# 多继承示例（继承自Dog类）
# 这里为简化，只继承一个父类，多继承语法为：class 子类(父类1, 父类2)
class ServiceDog(Dog):
    """服务犬类：继承自Dog，有特殊服务功能"""

    def __init__(self, name, breed, service_type):
        # 调用父类（Dog）的构造方法
        super().__init__(name, breed)
        self.service_type = service_type  # 服务类型：如导盲、搜救等

    # 再次重写方法，实现更具体的行为
    def make_sound(self):
        print(f"{self.name}（{self.service_type}犬）发出警示声！")

    # 服务犬特有方法
    def work(self):
        print(f"{self.name}正在执行{self.service_type}工作")

# 当一个类继承多个父类（多继承）时，使用 super() 调用父类构造方法的写法需要结合 MRO（方法解析顺序，Method Resolution Order） 规则
# 基础写法：super().__init__()
# 在多继承中，super() 会按照当前类的 MRO 顺序 自动寻找下一个父类，并调用其构造方法。无需显式指定父类名，而是由 Python 自动处理继承链
class A:
    def __init__(self):
        print("A的构造方法")

class B:
    def __init__(self):
        print("B的构造方法")

class C(A, B):  # C继承A和B，MRO顺序为：C → A → B → object
    def __init__(self):
        super().__init__()  # 按MRO调用下一个父类（A）的构造方法
        print("C的构造方法")

# 实例化C
c = C()

# 说明：
# C 的 MRO 顺序是 [C, A, B, object]，因此 super().__init__() 在 C 的构造方法中会优先调用 A 的构造方法。

# 若父类构造方法有参数：需显式传递参数
# 如果父类的 __init__ 方法需要参数，super() 调用时必须传递对应参数，且参数需匹配 MRO 链中下一个父类的构造方法签名

class A:
    def __init__(self, x):
        self.x = x
        print(f"A的构造方法，x={x}")

class B:
    def __init__(self, y):
        self.y = y
        print(f"B的构造方法，y={y}")

class C(A, B):
    def __init__(self, x, y):
        # 按MRO调用A的构造方法（需传入x）
        super().__init__(x)
        # 若需调用B的构造方法，不能直接用super()（会按MRO找A的下一个父类，即B）
        # 但此时需注意：A的构造方法是否调用了自己的super()？
        B.__init__(self, y)  # 显式调用B的构造方法（需传入self）
        print("C的构造方法")

c = C(10, 20)

# 由于 A 的构造方法没有调用 super().__init__()，MRO 链会在 A 处中断，不会自动调用 B 的构造方法。因此需要显式调用 B.__init__(self, y)。
# 显式调用时必须传入 self（因为是直接调用父类方法，而非通过实例调用）

# 关键：理解 MRO 顺序
# 多继承中 super() 的行为完全由 MRO 决定。可以通过 类名.__mro__ 查看 MRO 顺序：
print(C.__mro__)
# 输出：(<class '__main__.C'>, <class '__main__.A'>, <class '__main__.B'>, <class 'object'>)
# MRO 确保每个父类只会被调用一次，避免多继承中的重复调用问题。
# 若所有父类的构造方法都通过 super() 调用下一个父类（形成链条），则会自动按 MRO 顺序执行所有父类的构造方法

# 在多继承中完全可以不使用 super()，而是通过显式调用每个父类的构造方法（类似 B.__init__(self, y) 的写法）来初始化所有父类。
# 这种方式更直接，尤其适合父类构造方法参数差异较大的场景
class A:
    def __init__(self, x):
        self.x = x
        print(f"A的构造方法，x={x}")

class B:
    def __init__(self, y):
        self.y = y
        print(f"B的构造方法，y={y}")

class C(A, B):
    def __init__(self, x, y):
        # 显式调用A的构造方法，传入self和x
        A.__init__(self, x)
        # 显式调用B的构造方法，传入self和y
        B.__init__(self, y)
        print("C的构造方法")

c = C(10, 20)


# 1.__name__ 是 Python 的内置变量：
# 当一个 Python 文件被直接运行时，该文件的 __name__ 变量会被自动设置为 "__main__"。
# 当一个 Python 文件被导入为模块时，该文件的 __name__ 变量会被设置为模块名（即文件名，如 "script"）。
# 2.if __name__ == "__main__": 的作用：
# 会让缩进内的代码只在 “文件被直接运行” 时执行，而在 “文件被导入为模块” 时不执行。
# 相当于其他语言（如 C/C++、Java）中的 main 函数，作为程序的入口点。

# 测试代码：实际使用这些类
if __name__ == "__main__":
    # 创建Animal类的实例
    animal = Animal("通用动物", 2)
    print(animal)  # 调用__str__方法
    print(repr(animal))  # 调用__repr__方法
    animal.make_sound()  # 调用实例方法

    # 创建Dog类的实例
    dog = Dog("旺财", "金毛", 3)
    print(dog.kingdom)  # 访问继承的类属性
    print(dog.species)  # 访问子类的类属性
    dog.make_sound()  # 调用重写的方法
    dog.fetch("球")  # 调用子类特有方法

    # 创建Cat类的实例
    cat = Cat("咪咪", "黑白", 2)
    cat.make_sound()
    cat.climb()

    # 创建ServiceDog类的实例
    guide_dog = ServiceDog(" Lucky", "拉布拉多", "导盲")
    guide_dog.make_sound()
    guide_dog.work()
    guide_dog.fetch("导盲鞍")  # 调用从Dog类继承的方法

    # 测试属性访问
    print(f"狗的年龄：{dog.age}")  # 通过property访问
    dog.age = 4  # 通过setter修改
    print(f"修改后狗的年龄：{dog.age}")

    # 测试类方法
    print(f"当前动物总数：{Animal.get_total_count()}")

    # 测试静态方法
    print(f"狗是否成年：{Animal.is_adult(dog.age)}")
    print(f"猫是否成年：{Animal.is_adult(cat.age)}")

    # 测试多态：同一方法在不同实例上的不同表现
    print("\n多态演示：")
    animals = [animal, dog, cat, guide_dog]
    for a in animals:
        a.make_sound()  # 自动调用对应类的make_sound方法

# Python 中，MRO（Method Resolution Order，方法解析顺序） 是解决多继承场景下 “方法 / 属性查找顺序” 的核心机制。
# 当一个类继承多个父类时，Python 需要明确：当调用一个方法或访问一个属性时，应该按照什么顺序去父类中查找？MRO 就是这个 “查找顺序表”
# 在单继承中（一个类只继承一个父类），查找顺序很简单：子类 → 父类 → 祖父类 → ... → object（所有类的基类）。
# 但在多继承中（一个类继承多个父类），顺序会变得复杂。例如：
class A:
    def func(self):
        print("A的func")

class B:
    def func(self):
        print("B的func")

class C(A, B):  # C同时继承A和B
    pass

c = C()
c.func()  # 应该调用A的func还是B的func？
# 此时必须有一个明确的规则来决定调用哪个父类的方法 —— 这就是 MRO 的作用。

# 规则：
# 以当前类为起点（MRO 列表的第一个元素是自身）。
# 依次处理每个父类的 MRO 列表，按 “父类声明顺序” 合并，同时确保：
# 若一个类出现在多个父类的 MRO 中，只保留最后一次出现前的位置（避免重复）。
# 确保父类的 MRO 顺序在子类中不被打乱。

# example1
class A: pass
class B: pass
class C(A, B): pass  # 声明顺序：A在前，B在后

print(C.__mro__)
# 输出：(<class '__main__.C'>, <class '__main__.A'>, <class '__main__.B'>, <class 'object'>)
# MRO 顺序：C → A → B → object。
# 符合 “父类声明顺序”（A 先于 B），且最终继承 object（所有类的基类）

# example2
# 菱形继承（钻石继承）
# 最典型的复杂场景：子类继承两个父类，而这两个父类又继承同一个基类
class O: pass
class A(O): pass
class B(O): pass
class C(A, B): pass  # C继承A和B，A和B都继承O

print(C.__mro__)
# 输出：(<class '__main__.C'>, <class '__main__.A'>, <class '__main__.B'>, <class '__main__.O'>, <class 'object'>)
# MRO 顺序：C → A → B → O → object。
# 关键点：O 只出现一次，且在 A 和 B 之后

# example3
# 带方法的多继承
# 结合方法调用，看 MRO 如何决定执行哪个父类的方法：
class A:
    def func(self):
        print("A的func")

class B:
    def func(self):
        print("B的func")

class C(A, B):
    pass

c = C()
c.func()  # 输出：A的func（因为A在MRO中比B靠前）
print(C.__mro__)  # (C, A, B, object)

class A:
    def func(self):
        print("A的func")

class B:
    def func(self):
        print("B的func")

class C(B, A):  # 继承顺序：B在前，A在后
    pass

c = C()
c.func()  # 调用B的func（因为B在MRO中比A靠前）
print(C.__mro__)  # 输出MRO顺序

# MRO 与 super() 的关系
# super() 的行为完全依赖 MRO：
# super() 在当前类的 MRO 列表中，查找 “下一个类”，并调用该类的方法。
# 例如在 C 的方法中调用 super().func()，会按 MRO 顺序找到 A 的 func；若 A 中也有 super().func()，
# 则继续按 MRO 找到 B 的 func，以此类推

# MRO 是多继承的 “导航图”：决定方法 / 属性的查找顺序，避免命名冲突
# 继承声明顺序是 MRO（方法解析顺序）的核心影响因素之一，但 MRO 并非只由继承顺序决定，还会结合父类自身的 MRO
# 来确保整体顺序的合理性（遵循 C3 线性化算法的规则）
# 继承声明顺序是基础
# 在多继承中，子类声明时父类的顺序（如 class C(B, A) 中 B 在 A 之前）会直接影响 MRO，排在前面的父类会优先出现在 MRO 中。
# 例如：
# class C(B, A) 的 MRO 中，B 一定在 A 之前；
# class C(A, B) 的 MRO 中，A 一定在 B 之前。
# MRO 还会合并父类自身的 MRO
# 当父类本身也有继承关系时，MRO 会将父类的 MRO 列表 “合并” 进来，同时保证：
# 父类自己的 MRO 顺序不被打乱（单调性）；
# 所有类在 MRO 中只出现一次（去重）
class X: pass
class A(X): pass  # A的MRO：[A, X, object]
class B(X): pass  # B的MRO：[B, X, object]
class C(A, B): pass  # C继承A和B
# C 的 MRO 是 [C, A, B, X, object]
# 这里不仅遵循了 A 在 B 前的声明顺序，还合并了 A 的 MRO（A, X）和 B 的 MRO（B, X），同时确保 X 只出现一次
