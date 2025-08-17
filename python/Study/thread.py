# 在Python中, 多线程可以通过threading模块实现, 用于同时执行多个任务, 提高程序效率(尤其适用于I/O密集型任务)

# 一、核心概念
# 线程: 程序执行的最小单元, 一个进程可以包含多个线程, 共享进程资源。
# threading.Thread: threading模块的核心类, 用于创建和管理线程。
# 启动线程: 通过start()方法启动线程, 而非直接调用线程函数。
# 阻塞主线程: 使用join()方法让主线程等待子线程执行完毕后再继续。


# 二、创建线程的两种方式
# 方式1: 直接实例化Thread类, 传入目标函数
import threading
import time

# 定义线程要执行的函数
def print_numbers(name, delay):
    for i in range(5):
        time.sleep(delay)  # 模拟任务耗时
        print(f"线程{name}: {i}")

# 创建线程(target为目标函数, args为函数参数)
thread1 = threading.Thread(target=print_numbers, args=("A", 0.5))
thread2 = threading.Thread(target=print_numbers, args=("B", 1))

# 启动线程
thread1.start()
thread2.start()

# 主线程等待子线程结束(可选, 不加则主线程可能先结束)
thread1.join()
thread2.join()

print("主线程结束")


# 方式2: 继承Thread类, 重写run()方法
import threading
import time

class MyThread(threading.Thread):
    def __init__(self, name, delay):
        super().__init__()  # 调用父类构造方法
        self.name = name
        self.delay = delay

    # 重写run()方法, 线程启动后会自动执行
    def run(self):
        for i in range(5):
            time.sleep(self.delay)
            print(f"线程{self.name}: {i}")

# 创建并启动线程
thread1 = MyThread("A", 0.5)
thread2 = MyThread("B", 1)
thread1.start()
thread2.start()

# 等待线程结束
thread1.join()
thread2.join()

print("主线程结束")

# 三、关键方法说明
# 1. start(): 启动线程, 调用线程的run()方法(无需手动调用run())。
# 2. join([timeout]): 阻塞当前线程(如主线程), 等待被调用线程执行完毕。timeout为可选超时时间(秒)。
# 3. is_alive(): 判断线程是否在运行。
# 4. setDaemon(True): 将线程设为守护线程(主线程结束时, 守护线程会被强制终止, 需在start()前设置)。


# 四、注意事项
# 全局解释器锁(GIL): Python的GIL限制了同一时间只有一个线程执行Python字节码, 因此多线程对CPU密集型任务提速有限(更适合I/O密集型任务, 如网络请求、文件读写)。
# 线程安全: 多个线程操作共享资源时可能出现冲突, 需用threading.Lock()加锁保护(示例如下): 

import threading

count = 0
lock = threading.Lock()  # 创建锁

def increment():
    global count
    for _ in range(100000):
        with lock:  # 自动获取和释放锁, 确保操作原子性
            count += 1

# 创建线程
t1 = threading.Thread(target=increment)
t2 = threading.Thread(target=increment)

t1.start()
t2.start()
t1.join()
t2.join()

print(count)  # 结果应为200000(无锁可能小于此值)
