# Go 语言（Golang）学习手册

---

## 目录

- [Go 语言（Golang）学习手册](#go-语言golang学习手册)
  - [目录](#目录)
  - [一、环境搭建与入门](#一环境搭建与入门)
    - [1. 安装 Go](#1-安装-go)
    - [2. 重要环境变量](#2-重要环境变量)
    - [3. 推荐开发工具](#3-推荐开发工具)
    - [4. 第一个 Go 程序](#4-第一个-go-程序)
  - [二、基础语法](#二基础语法)
    - [1. 变量（Variable）](#1-变量variable)
    - [2. 常量（Constant）](#2-常量constant)
    - [3. 基本数据类型（Basic Types）](#3-基本数据类型basic-types)
    - [4. 类型转换（Type Conversion）](#4-类型转换type-conversion)
    - [5. 指针（Pointer）](#5-指针pointer)
  - [三、控制流](#三控制流)
    - [1. 条件语句 if/else](#1-条件语句-ifelse)
    - [2. switch 语句](#2-switch-语句)
    - [3. for 循环](#3-for-循环)
    - [4. for-range 遍历](#4-for-range-遍历)
  - [四、函数](#四函数)
    - [1. 函数基础](#1-函数基础)
    - [2. 函数作为值（First-class Function）](#2-函数作为值first-class-function)
    - [3. 闭包（Closure）](#3-闭包closure)
    - [4. defer 关键字](#4-defer-关键字)
  - [五、复合数据类型](#五复合数据类型)
    - [1. 数组（Array）](#1-数组array)
    - [2. 切片（Slice）](#2-切片slice)
    - [3. 映射（Map）](#3-映射map)
    - [4. 结构体（Struct）](#4-结构体struct)
    - [2. 接口（Interface）](#2-接口interface)
    - [3. 类型断言与类型开关](#3-类型断言与类型开关)
    - [4. 空接口（interface{}）与 any](#4-空接口interface与-any)
  - [七、错误处理](#七错误处理)
    - [1. error 接口](#1-error-接口)
    - [2. 自定义错误类型](#2-自定义错误类型)
    - [3. 错误包装（Error Wrapping）](#3-错误包装error-wrapping)
    - [4. panic 与 recover](#4-panic-与-recover)
  - [八、并发编程](#八并发编程)
    - [1. Goroutine](#1-goroutine)
    - [2. Channel（通道）](#2-channel通道)
    - [3. select 语句](#3-select-语句)
    - [4. sync 包](#4-sync-包)
  - [九、包与模块管理](#九包与模块管理)
    - [1. 包（Package）](#1-包package)
    - [2. Go Modules（模块管理）](#2-go-modules模块管理)
  - [十、Go 常用命令](#十go-常用命令)
  - [十一、常用标准库](#十一常用标准库)
    - [1. fmt — 格式化输入输出](#1-fmt--格式化输入输出)
    - [2. strings — 字符串操作](#2-strings--字符串操作)
    - [3. strconv — 类型转换](#3-strconv--类型转换)
    - [4. math — 数学运算](#4-math--数学运算)
    - [5. time — 时间处理](#5-time--时间处理)
    - [6. os — 操作系统接口](#6-os--操作系统接口)
    - [7. bufio — 缓冲 I/O](#7-bufio--缓冲-io)
    - [8. io — 基础 I/O 接口](#8-io--基础-io-接口)
    - [9. encoding/json — JSON 处理](#9-encodingjson--json-处理)
    - [9. net/http — HTTP 编程](#9-nethttp--http-编程)
    - [10. sort — 排序](#10-sort--排序)
    - [11. regexp — 正则表达式](#11-regexp--正则表达式)
    - [12. log — 日志](#12-log--日志)
    - [13. path/filepath — 文件路径](#13-pathfilepath--文件路径)
  - [十二、泛型（Go 1.18+）](#十二泛型go-118)
  - [附录：常用第三方库推荐](#附录常用第三方库推荐)
  - [十三、配置文件解析（JSON / YAML）](#十三配置文件解析json--yaml)
    - [1. JSON 配置文件读取](#1-json-配置文件读取)
    - [2. YAML 配置文件读取](#2-yaml-配置文件读取)
    - [3. 使用 Viper 管理配置（推荐）](#3-使用-viper-管理配置推荐)
    - [1. 连接数据库与模型定义](#1-连接数据库与模型定义)
    - [2. CRUD 操作](#2-crud-操作)
    - [3. 高级查询](#3-高级查询)
  - [十五、常用中间件交互](#十五常用中间件交互)
    - [1. Redis（go-redis）](#1-redisgo-redis)
    - [2. 消息队列](#2-消息队列)
      - [RabbitMQ（amqp091-go）](#rabbitmqamqp091-go)
      - [Kafka（kafka-go）](#kafkakafka-go)
    - [3. HTTP 中间件（以 Gin 为例）](#3-http-中间件以-gin-为例)

---

## 一、环境搭建与入门

### 1. 安装 Go

1. 访问 [https://golang.org/dl/](https://golang.org/dl/) 下载对应系统的安装包
2. 安装完成后，打开终端验证：

```bash
go version
# 输出示例：go version go1.22.0 windows/amd64
```

### 2. 重要环境变量

| 变量 | 说明 |
|------|------|
| `GOROOT` | Go 安装目录，安装时自动设置 |
| `GOPATH` | 工作区目录，默认 `~/go`，存放下载的包 |
| `GOMODCACHE` | 模块缓存目录，默认 `$GOPATH/pkg/mod` |
| `GOPROXY` | 模块代理，国内推荐设置为 `https://goproxy.cn,direct` |

```bash
# 查看所有 Go 环境变量
go env

# 设置国内代理（推荐，加速依赖下载）
go env -w GOPROXY=https://goproxy.cn,direct
```

### 3. 推荐开发工具

- **VS Code** + Go 插件（免费，轻量）
- **GoLand**（JetBrains 出品，功能强大，收费）

### 4. 第一个 Go 程序

新建文件 `hello.go`：

```go
package main  // 每个可执行程序必须有 main 包

import "fmt"  // 导入标准库 fmt（format 的缩写，提供格式化输入输出）

func main() {  // 程序入口函数，固定写法
    fmt.Println("Hello, Go!")
}
```

运行方式：

```bash
go run hello.go        # 直接运行（不生成可执行文件）
go build hello.go      # 编译生成可执行文件
go build -o myapp .    # 编译当前目录，输出文件名为 myapp
```

**程序结构说明：**
- `package main`：声明包名。Go 程序由包（package）组成，可执行程序的入口包必须叫 `main`
- `import`：导入需要用到的包
- `func main()`：程序从这里开始执行

---

## 二、基础语法

### 1. 变量（Variable）

Go 是**静态类型**语言，变量必须有明确的类型。

```go
package main

import "fmt"

func main() {
    // 方式1：完整声明（var 变量名 类型）
    var age int
    age = 25

    // 方式2：声明并赋值
    var name string = "Alice"

    // 方式3：短变量声明（:= 自动推断类型，只能在函数内使用）
    height := 1.75   // 推断为 float64
    isOK := true     // 推断为 bool

    // 方式4：一次声明多个变量
    var a, b int = 10, 20
    x, y := "Go", 3.14

    // 方式5：批量声明（常用于包级变量）
    var (
        city    string = "Beijing"
        country string = "China"
        year    int    = 2024
    )

    fmt.Println(age, name, height, isOK)
    fmt.Println(a, b, x, y)
    fmt.Println(city, country, year)
}
```

> **注意**：Go 中声明的变量**必须使用**，否则编译报错。未使用的导入包同样报错。

### 2. 常量（Constant）

常量在编译时确定，运行时不可修改。

```go
package main

import "fmt"

// 单个常量
const Pi = 3.14159

// 批量常量
const (
    StatusOK       = 200
    StatusNotFound = 404
)

// iota：枚举常量，从 0 开始自动递增
const (
    Sunday = iota  // 0
    Monday         // 1
    Tuesday        // 2
    Wednesday      // 3
    Thursday       // 4
    Friday         // 5
    Saturday       // 6
)

// iota 也可以参与运算
const (
    KB = 1 << (10 * (iota + 1))  // 1 << 10 = 1024
    MB                            // 1 << 20
    GB                            // 1 << 30
)

func main() {
    fmt.Println(Pi, StatusOK, StatusNotFound)
    fmt.Println(Sunday, Monday, Saturday)  // 0 1 6
    fmt.Println(KB, MB, GB)               // 1024 1048576 1073741824
}
```

### 3. 基本数据类型（Basic Types）

```go
package main

import "fmt"

func main() {
    // ── 整数类型 ──
    // int/uint：平台相关（32位系统为32位，64位系统为64位），日常首选
    var i int = -100
    var u uint = 100

    // 固定大小整数（需要精确控制大小时使用）
    var i8  int8  = 127          // -128 ~ 127
    var i16 int16 = 32767        // -32768 ~ 32767
    var i32 int32 = 2147483647
    var i64 int64 = 9223372036854775807

    // byte = uint8（常用于处理字节数据）
    var b byte = 255

    // rune = int32（表示一个 Unicode 字符，处理中文等多字节字符时使用）
    var r rune = '中'

    // ── 浮点类型 ──
    var f32 float32 = 3.14          // 精度约 7 位小数
    var f64 float64 = 3.141592653   // 精度约 15 位小数，日常首选

    // ── 复数类型（较少用）──
    var c complex128 = 1 + 2i

    // ── 布尔类型 ──
    var isReady bool = true   // 只有 true 和 false，不能用 0/1 代替

    // ── 字符串类型 ──
    var s string = "Hello, 世界"
    // 字符串是不可变的字节序列（UTF-8 编码）
    // 用双引号 "" 表示普通字符串，用反引号 `` 表示原始字符串（可换行，不处理转义）
    rawStr := `第一行
第二行
路径：C:\Users\test`

    fmt.Println(i, u, i8, i16, i32, i64, b, r)
    fmt.Println(f32, f64, c)
    fmt.Println(isReady, s, rawStr)
}
```

**类型速查表：**

| 类型 | 大小 | 范围/说明 |
|------|------|-----------|
| `int8` | 1字节 | -128 ~ 127 |
| `int16` | 2字节 | -32768 ~ 32767 |
| `int32` / `rune` | 4字节 | -2^31 ~ 2^31-1 |
| `int64` | 8字节 | -2^63 ~ 2^63-1 |
| `uint8` / `byte` | 1字节 | 0 ~ 255 |
| `float32` | 4字节 | 约7位精度 |
| `float64` | 8字节 | 约15位精度，推荐 |
| `bool` | 1字节 | true / false |
| `string` | - | UTF-8字节序列，不可变 |

### 4. 类型转换（Type Conversion）

Go **不支持隐式类型转换**，必须显式转换。

```go
package main

import "fmt"

func main() {
    var i int = 42
    var f float64 = float64(i)   // int → float64
    var u uint = uint(f)          // float64 → uint

    // 字符串与字节切片互转
    s := "Hello"
    bs := []byte(s)    // string → []byte
    s2 := string(bs)   // []byte → string

    // rune 切片（处理中文字符串）
    chinese := "你好世界"
    runes := []rune(chinese)
    fmt.Println(len(chinese))  // 12（字节数，每个汉字3字节）
    fmt.Println(len(runes))    // 4（字符数）

    fmt.Println(i, f, u, s2)
}
```

> 字符串与数字之间的转换需要用 `strconv` 包，见[常用标准库](#十一常用标准库)章节。

### 5. 指针（Pointer）

指针存储的是变量的**内存地址**，而不是值本身。

```go
package main

import "fmt"

func main() {
    x := 10

    // & 取地址（address-of operator）
    p := &x
    fmt.Println(p)   // 输出内存地址，如 0xc0000b4008
    fmt.Println(*p)  // * 解引用（dereference），输出 10

    // 通过指针修改原变量
    *p = 20
    fmt.Println(x)   // 20，x 被修改了

    // new() 分配内存，返回指针
    q := new(int)    // q 是 *int 类型，指向一个值为 0 的 int
    *q = 100
    fmt.Println(*q)  // 100
}

// 指针的典型用途：函数中修改外部变量
func increment(n *int) {
    *n++
}
```

**值传递 vs 指针传递：**

```go
// 值传递：函数内修改不影响外部
func addOne(n int) {
    n++  // 只修改了副本
}

// 指针传递：函数内修改影响外部
func addOnePtr(n *int) {
    *n++  // 修改了原变量
}

func main() {
    a := 5
    addOne(a)
    fmt.Println(a)    // 5，未变

    addOnePtr(&a)
    fmt.Println(a)    // 6，已变
}
```

---

## 三、控制流

### 1. 条件语句 if/else

```go
package main

import "fmt"

func main() {
    score := 85

    // 基本 if-else
    if score >= 60 {
        fmt.Println("及格")
    } else {
        fmt.Println("不及格")
    }

    // if-else if-else 链
    if score >= 90 {
        fmt.Println("A")
    } else if score >= 80 {
        fmt.Println("B")
    } else if score >= 70 {
        fmt.Println("C")
    } else {
        fmt.Println("D")
    }

    // if 初始化语句：变量 n 的作用域仅限于 if-else 块内
    if n := score % 10; n > 5 {
        fmt.Println("后半段")
    } else {
        fmt.Println("前半段")
    }
}
```

### 2. switch 语句

Go 的 switch 默认**不需要 break**，每个 case 执行完自动退出。

```go
package main

import "fmt"

func main() {
    day := 3

    // 基本 switch
    switch day {
    case 1:
        fmt.Println("Monday")
    case 2:
        fmt.Println("Tuesday")
    case 3:
        fmt.Println("Wednesday")
    case 6, 7:          // 多个值合并到一个 case
        fmt.Println("Weekend")
    default:
        fmt.Println("Other")
    }

    // 无条件 switch（等价于多个 if-else）
    temp := 25
    switch {
    case temp < 0:
        fmt.Println("冰点以下")
    case temp < 20:
        fmt.Println("凉爽")
    case temp < 35:
        fmt.Println("舒适")
    default:
        fmt.Println("炎热")
    }

    // fallthrough：强制执行下一个 case（较少用）
    n := 1
    switch n {
    case 1:
        fmt.Println("one")
        fallthrough
    case 2:
        fmt.Println("two")  // 也会被打印
    }
}
```

### 3. for 循环

Go 只有 `for` 一种循环关键字，可以模拟 while、do-while 等。

```go
package main

import "fmt"

func main() {
    // 标准三段式 for
    for i := 0; i < 5; i++ {
        fmt.Print(i, " ")  // 0 1 2 3 4
    }
    fmt.Println()

    // 只有条件（类似 while）
    n := 1
    for n < 100 {
        n *= 2
    }
    fmt.Println(n)  // 128

    // 无限循环 + break
    count := 0
    for {
        count++
        if count >= 3 {
            break
        }
    }

    // continue：跳过本次迭代
    for i := 0; i < 10; i++ {
        if i%2 == 0 {
            continue  // 跳过偶数
        }
        fmt.Print(i, " ")  // 1 3 5 7 9
    }
    fmt.Println()
}
```

### 4. for-range 遍历

`for-range` 是 Go 遍历集合的惯用方式，适用于数组、切片、字符串、map、channel。

```go
package main

import "fmt"

func main() {
    // 遍历切片：返回 索引, 值
    fruits := []string{"apple", "banana", "cherry"}
    for i, v := range fruits {
        fmt.Printf("[%d] %s\n", i, v)
    }

    // 用 _ 忽略不需要的返回值
    for _, v := range fruits { fmt.Println(v) }
    for i := range fruits    { fmt.Println(i) }

    // 遍历字符串：返回 字节索引, rune（Unicode字符）
    // 中文字符占3字节，索引不连续
    for i, c := range "Go语言" {
        fmt.Printf("索引%d: %c\n", i, c)
    }

    // 遍历 map：返回 key, value（顺序随机！）
    scores := map[string]int{"Alice": 90, "Bob": 85}
    for name, score := range scores {
        fmt.Printf("%s: %d\n", name, score)
    }
}
```

> **注意**：`for-range` 中的值是**副本**，修改它不会影响原集合。若要修改，需通过索引操作（`slice[i] = newVal`）。

---

## 四、函数

### 1. 函数基础

```go
package main

import "fmt"

// 基本函数：func 函数名(参数列表) 返回类型
func add(a, b int) int {  // 相同类型参数可合并
    return a + b
}

// 多返回值（Go 的特色）
func divide(a, b float64) (float64, error) {
    if b == 0 {
        return 0, fmt.Errorf("除数不能为零")
    }
    return a / b, nil
}

// 命名返回值：可以直接 return（裸返回）
func minMax(nums []int) (min, max int) {
    min, max = nums[0], nums[0]
    for _, n := range nums[1:] {
        if n < min { min = n }
        if n > max { max = n }
    }
    return  // 裸返回，自动返回 min 和 max
}

// 可变参数（variadic）：...类型
func sum(nums ...int) int {
    total := 0
    for _, n := range nums {
        total += n
    }
    return total
}

func main() {
    fmt.Println(add(3, 5))           // 8

    result, err := divide(10, 3)
    if err != nil {
        fmt.Println("错误:", err)
    } else {
        fmt.Printf("%.4f\n", result) // 3.3333
    }

    min, max := minMax([]int{3, 1, 4, 1, 5, 9})
    fmt.Println(min, max)            // 1 9

    fmt.Println(sum(1, 2, 3, 4, 5)) // 15

    // 将切片展开传给可变参数函数
    nums := []int{10, 20, 30}
    fmt.Println(sum(nums...))        // 60
}
```

### 2. 函数作为值（First-class Function）

Go 中函数是"一等公民"，可以赋值给变量、作为参数传递、作为返回值。

```go
package main

import "fmt"

// 接收函数作为参数（高阶函数 higher-order function）
func apply(f func(int, int) int, a, b int) int {
    return f(a, b)
}

// 返回函数（函数工厂）
func multiplier(factor int) func(int) int {
    return func(n int) int {
        return n * factor
    }
}

func main() {
    // 匿名函数赋值给变量
    add := func(a, b int) int { return a + b }
    fmt.Println(apply(add, 3, 4))  // 7

    // 直接传入匿名函数
    fmt.Println(apply(func(a, b int) int { return a * b }, 3, 4))  // 12

    // 函数工厂
    double := multiplier(2)
    triple := multiplier(3)
    fmt.Println(double(5), triple(5))  // 10 15
}
```

### 3. 闭包（Closure）

闭包是一个函数，它**捕获并记住**了其外部作用域的变量，即使外部函数已经返回。

```go
package main

import "fmt"

// 计数器工厂：每次调用返回递增的值
func makeCounter() func() int {
    count := 0  // 被闭包捕获的变量
    return func() int {
        count++
        return count
    }
}

func main() {
    c1 := makeCounter()
    c2 := makeCounter()  // 独立的计数器，有自己的 count

    fmt.Println(c1(), c1(), c1())  // 1 2 3
    fmt.Println(c2())              // 1（独立的）
}
```

> 闭包的关键：内部函数持有外部变量的**引用**，而不是副本。

### 4. defer 关键字

`defer` 将函数调用推迟到**当前函数返回前**执行，常用于资源清理。

```go
package main

import "fmt"

func main() {
    fmt.Println("开始")
    defer fmt.Println("最后执行")  // 推迟到 main 返回前
    fmt.Println("结束")
    // 输出：开始 → 结束 → 最后执行
}
```

**多个 defer：后进先出（LIFO）**

```go
func main() {
    defer fmt.Println("1")
    defer fmt.Println("2")
    defer fmt.Println("3")
    // 输出：3 → 2 → 1
}
```

**典型用途：确保资源释放**

```go
import "os"

func readFile(path string) error {
    f, err := os.Open(path)
    if err != nil { return err }
    defer f.Close()  // 无论后续是否出错，函数返回时一定关闭文件

    // ... 读取文件内容 ...
    return nil
}
```

**defer + recover 捕获 panic：**

```go
func safeDiv(a, b int) (result int, err error) {
    defer func() {
        if r := recover(); r != nil {
            err = fmt.Errorf("捕获到异常: %v", r)
        }
    }()
    return a / b, nil  // b=0 时触发 panic
}

func main() {
    result, err := safeDiv(10, 0)
    fmt.Println(result, err)
    // 0 捕获到异常: runtime error: integer divide by zero
}
```

---

## 五、复合数据类型

### 1. 数组（Array）

数组是**固定长度**的同类型元素序列，长度是类型的一部分（`[3]int` 和 `[5]int` 是不同类型）。

```go
package main

import "fmt"

func main() {
    var a [3]int                    // 默认值全为 0
    b := [3]int{1, 2, 3}
    c := [...]int{10, 20, 30, 40}  // ... 让编译器自动计算长度

    a[0] = 100
    fmt.Println(a, b, c, len(c))  // [100 0 0] [1 2 3] [10 20 30 40] 4

    // 数组是值类型：赋值会复制整个数组
    d := b
    d[0] = 999
    fmt.Println(b[0], d[0])  // 1 999（b 未被修改）
}
```

> 实际开发中数组用得较少，更多使用切片（Slice）。

### 2. 切片（Slice）

切片是对底层数组的**动态视图**，长度可变，是 Go 中最常用的序列类型。

```go
package main

import "fmt"

func main() {
    s := []int{1, 2, 3, 4, 5}

    // make([]类型, 长度, 容量)
    s2 := make([]int, 3, 5)
    fmt.Printf("len=%d cap=%d %v\n", len(s2), cap(s2), s2)

    // 切片操作（左闭右开）
    fmt.Println(s[1:3])  // [2 3]
    fmt.Println(s[:2])   // [1 2]
    fmt.Println(s[3:])   // [4 5]

    // append：追加元素（容量不足时自动扩容）
    s = append(s, 6, 7)
    extra := []int{8, 9}
    s = append(s, extra...)  // 追加另一个切片用 ...
    fmt.Println(s)

    // copy：复制切片（不共享底层数组）
    dst := make([]int, 3)
    copy(dst, s)
    fmt.Println(dst)  // [1 2 3]

    // 删除索引 i 的元素
    i := 2
    s = append(s[:i], s[i+1:]...)
    fmt.Println(s)
}
```

**切片共享底层数组的陷阱：**

```go
a := []int{1, 2, 3, 4, 5}
b := a[1:3]   // b 和 a 共享同一底层数组
b[0] = 100
fmt.Println(a)  // [1 100 3 4 5]（a 也被修改了！）

// 避免共享：创建独立副本
c := append([]int{}, a[1:3]...)
```

### 3. 映射（Map）

Map 是键值对（key-value）的无序集合，类似其他语言的字典/哈希表。

```go
package main

import "fmt"

func main() {
    // make 创建
    ages := make(map[string]int)
    ages["Alice"] = 30
    ages["Bob"] = 25

    // 字面量初始化
    scores := map[string]int{"Math": 90, "English": 85}

    // 读取（键不存在时返回零值）
    fmt.Println(ages["Alice"])  // 30
    fmt.Println(ages["Dave"])   // 0

    // 检查键是否存在（ok idiom）
    val, ok := ages["Dave"]
    if !ok {
        fmt.Println("Dave 不存在", val)
    }

    // 修改、删除
    ages["Bob"] = 26
    delete(scores, "English")

    // 遍历（顺序随机！）
    for k, v := range ages {
        fmt.Printf("%s: %d\n", k, v)
    }
    fmt.Println("总数:", len(ages))
}
```

> map 是引用类型，函数传参不会复制整个 map。map **不是并发安全**的，并发读写需加锁。

### 4. 结构体（Struct）

结构体将多个不同类型的字段组合在一起，是 Go 实现面向对象的基础。

```go
package main

import "fmt"

type Address struct {
    Street, City string
}

type Person struct {
    Name  string
    Age   int
    Email string
    Address       // 嵌入（匿名字段），可直接访问 Address 的字段
}

func main() {
    p := Person{
        Name:    "Alice",
        Age:     30,
        Email:   "alice@example.com",
        Address: Address{Street: "123 Main St", City: "NYC"},
    }

    fmt.Println(p.Name, p.City)  // 直接访问嵌入字段

    // 结构体指针（Go 自动解引用，无需 (*p).Age）
    pp := &Person{Name: "Bob", Age: 25}
    pp.Age++
    fmt.Println(pp.Age)  // 26

    // 匿名结构体（临时使用）
    point := struct{ X, Y int }{X: 10, Y: 20}
    fmt.Println(point)
}
```

**结构体标签（Struct Tag）：**

```go
type User struct {
    ID       int    `json:"id"       yaml:"id"`
    Name     string `json:"name"     yaml:"name"`
    Password string `json:"-"        yaml:"-"`              // 序列化时忽略该字段
    Email    string `json:"email,omitempty" yaml:"email,omitempty"` // 零值时忽略（见下方说明）
}
```

**`omitempty` 详解：** 序列化时，如果字段值为**零值**（数字 `0`、字符串 `""`、布尔 `false`、nil 指针/切片/map），则该字段不会出现在输出中。

```go
u1 := User{ID: 1, Name: "Alice", Email: ""}
// json.Marshal(u1) → {"id":1,"name":"Alice"}
// Email 为空字符串（零值），被 omitempty 忽略

u2 := User{ID: 2, Name: "Bob", Email: "bob@example.com"}
// json.Marshal(u2) → {"id":2,"name":"Bob","email":"bob@example.com"}
// Email 非零值，正常输出
```

**从 JSON / YAML 解析到结构体：**

```go
import (
    "encoding/json"
    "gopkg.in/yaml.v3"
)

// 从 JSON 解析
jsonStr := `{"id":1,"name":"Alice","email":"alice@example.com"}`
var u1 User
json.Unmarshal([]byte(jsonStr), &u1)

// 从 YAML 解析
yamlStr := `
id: 2
name: Bob
email: bob@example.com
`
var u2 User
yaml.Unmarshal([]byte(yamlStr), &u2)
```

---

## 六、面向对象特性

### 1. 方法（Method）

Go 没有类（class），但可以给任意类型定义方法。方法就是带有**接收者（receiver）**的函数。

```go
package main

import (
    "fmt"
    "math"
)

type Circle struct {
    Radius float64
}

// 值接收者：操作的是结构体的副本，不能修改原值
func (c Circle) Area() float64 {
    return math.Pi * c.Radius * c.Radius
}

// 指针接收者：操作原结构体，可以修改字段
func (c *Circle) Scale(factor float64) {
    c.Radius *= factor
}

func (c Circle) String() string {
    return fmt.Sprintf("Circle(r=%.2f)", c.Radius)
}

func main() {
    c := Circle{Radius: 5}
    fmt.Println(c.Area())   // 78.54
    fmt.Println(c)          // Circle(r=5.00)

    c.Scale(2)              // Go 自动取地址：(&c).Scale(2)
    fmt.Println(c.Radius)   // 10
}
```

**值接收者 vs 指针接收者：**

| | 值接收者 | 指针接收者 |
|--|---------|-----------|
| 是否修改原值 | 否（操作副本） | 是 |
| 调用方式 | 值和指针都可调用 | 值和指针都可调用（Go 自动转换） |
| 适用场景 | 只读操作、小结构体 | 需要修改字段、大结构体（避免复制） |

### 2. 接口（Interface）

接口定义了一组**方法签名**（只有声明，没有实现）。任何类型只要实现了接口的所有方法，就**自动满足**该接口（隐式实现，无需 `implements` 关键字）。

```go
package main

import (
    "fmt"
    "math"
)

// 定义接口
type Shape interface {
    Area() float64
    Perimeter() float64
}

type Rectangle struct{ Width, Height float64 }
type Circle struct{ Radius float64 }

// Rectangle 实现 Shape 接口
func (r Rectangle) Area() float64      { return r.Width * r.Height }
func (r Rectangle) Perimeter() float64 { return 2 * (r.Width + r.Height) }

// Circle 实现 Shape 接口
func (c Circle) Area() float64      { return math.Pi * c.Radius * c.Radius }
func (c Circle) Perimeter() float64 { return 2 * math.Pi * c.Radius }

// 接收接口类型的函数（多态）
func printInfo(s Shape) {
    fmt.Printf("面积: %.2f, 周长: %.2f\n", s.Area(), s.Perimeter())
}

func main() {
    shapes := []Shape{
        Rectangle{Width: 5, Height: 3},
        Circle{Radius: 4},
    }
    for _, s := range shapes {
        printInfo(s)
    }
}
```

### 3. 类型断言与类型开关

```go
package main

import "fmt"

func describe(i interface{}) {
    // 类型断言（type assertion）：i.(Type)
    // 安全写法：val, ok := i.(Type)
    if s, ok := i.(string); ok {
        fmt.Printf("字符串，长度=%d: %s\n", len(s), s)
        return
    }

    // 类型开关（type switch）：更优雅地处理多种类型
    switch v := i.(type) {
    case int:
        fmt.Printf("整数: %d\n", v)
    case float64:
        fmt.Printf("浮点数: %.2f\n", v)
    case bool:
        fmt.Printf("布尔: %t\n", v)
    case []int:
        fmt.Printf("int切片，长度=%d\n", len(v))
    default:
        fmt.Printf("未知类型: %T\n", v)
    }
}

func main() {
    describe(42)
    describe(3.14)
    describe("hello")
    describe(true)
    describe([]int{1, 2, 3})
}
```

### 4. 空接口（interface{}）与 any

空接口不包含任何方法，因此**所有类型都实现了空接口**，可以存储任意类型的值。

```go
// interface{} 和 any 完全等价（Go 1.18+ 推荐用 any）
func printAny(v any) {
    fmt.Printf("值: %v, 类型: %T\n", v, v)
}

// 存储任意类型的切片
data := []any{1, "hello", 3.14, true, []int{1, 2}}
for _, v := range data {
    printAny(v)
}

// JSON 解析常用 map[string]any
var result map[string]any
```

> 空接口虽然灵活，但失去了编译期类型检查。能用具体类型时尽量用具体类型，或使用泛型（Go 1.18+）。

---

## 七、错误处理

### 1. error 接口

Go 的错误处理不使用 try/catch，而是通过**返回值**显式传递错误。

```go
package main

import (
    "errors"
    "fmt"
)

// error 是内置接口：type error interface { Error() string }
func divide(a, b float64) (float64, error) {
    if b == 0 {
        return 0, errors.New("除数不能为零")
    }
    return a / b, nil
}

func main() {
    // 惯用模式：立即检查错误
    result, err := divide(10, 0)
    if err != nil {
        fmt.Println("错误:", err)
        return
    }
    fmt.Println(result)
}
```

### 2. 自定义错误类型

```go
package main

import "fmt"

// 自定义错误类型（实现 error 接口）
type ValidationError struct {
    Field   string
    Message string
}

func (e *ValidationError) Error() string {
    return fmt.Sprintf("字段 %s 验证失败: %s", e.Field, e.Message)
}

func validateAge(age int) error {
    if age < 0 || age > 150 {
        return &ValidationError{Field: "age", Message: "必须在 0-150 之间"}
    }
    return nil
}

func main() {
    err := validateAge(-1)
    if err != nil {
        // errors.As：从错误链中提取特定类型的错误
        var ve *ValidationError
        if errors.As(err, &ve) {
            fmt.Println("字段:", ve.Field)
            fmt.Println("原因:", ve.Message)
        }
    }
}
```

### 3. 错误包装（Error Wrapping）

```go
import (
    "errors"
    "fmt"
    "os"
)

func readConfig(path string) error {
    data, err := os.ReadFile(path)
    if err != nil {
        // %w 包装错误，保留原始错误链
        return fmt.Errorf("读取配置文件失败: %w", err)
    }
    _ = data
    return nil
}

func main() {
    err := readConfig("config.json")
    if err != nil {
        fmt.Println(err)
        // errors.Is：检查错误链中是否包含特定错误
        if errors.Is(err, os.ErrNotExist) {
            fmt.Println("文件不存在")
        }
    }
}
```

### 4. panic 与 recover

`panic` 用于不可恢复的严重错误，`recover` 在 `defer` 中捕获 panic。

```go
package main

import "fmt"

func mustPositive(n int) int {
    if n <= 0 {
        panic(fmt.Sprintf("必须为正数，得到: %d", n))
    }
    return n
}

// 用 recover 防止 panic 崩溃程序
func safeCall(f func()) (err error) {
    defer func() {
        if r := recover(); r != nil {
            err = fmt.Errorf("panic: %v", r)
        }
    }()
    f()
    return nil
}

func main() {
    err := safeCall(func() {
        mustPositive(-1)
    })
    fmt.Println(err)  // panic: 必须为正数，得到: -1
}
```

**错误处理最佳实践：**
- 可预期的错误（文件不存在、参数非法）→ 返回 `error`
- 不可恢复的严重错误（程序初始化失败）→ 使用 `panic`
- 在程序顶层（如 HTTP handler）用 `recover` 防止崩溃
- 用 `%w` 包装错误，保留错误链，方便 `errors.Is` / `errors.As` 检查

---

## 八、并发编程

Go 的并发模型基于 **CSP（Communicating Sequential Processes）**，核心是 goroutine 和 channel。口诀：**"不要通过共享内存来通信，而要通过通信来共享内存"**。

### 1. Goroutine

Goroutine 是 Go 的轻量级线程，由 Go 运行时管理，创建成本极低（初始栈约 2KB）。

```go
package main

import (
    "fmt"
    "time"
)

func task(name string, n int) {
    for i := 0; i < n; i++ {
        fmt.Printf("[%s] 第%d次\n", name, i+1)
        time.Sleep(100 * time.Millisecond)
    }
}

func main() {
    go task("A", 3)  // 启动 goroutine（go 关键字）
    go task("B", 3)

    // main 函数是主 goroutine，若它退出，所有子 goroutine 也会终止
    time.Sleep(1 * time.Second)  // 等待子 goroutine 完成
    fmt.Println("完成")
}
```

### 2. Channel（通道）

Channel 是 goroutine 之间安全传递数据的管道。

```go
package main

import "fmt"

func sum(nums []int, ch chan int) {
    total := 0
    for _, n := range nums {
        total += n
    }
    ch <- total  // 发送数据到 channel
}

func main() {
    nums := []int{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}

    ch := make(chan int)  // 无缓冲 channel

    go sum(nums[:5], ch)   // 计算前5个
    go sum(nums[5:], ch)   // 计算后5个

    a, b := <-ch, <-ch     // 从 channel 接收（阻塞直到有数据）
    fmt.Println(a + b)     // 55
}
```

**带缓冲的 Channel：**

```go
// 缓冲大小为3，发送方在缓冲满之前不会阻塞
ch := make(chan string, 3)
ch <- "first"
ch <- "second"
ch <- "third"
// ch <- "fourth"  // 会阻塞，缓冲已满

fmt.Println(<-ch)  // "first"
fmt.Println(<-ch)  // "second"
```

**关闭 Channel 与 range 遍历：**

```go
func producer(ch chan<- int) {  // chan<- 只写 channel
    for i := 1; i <= 5; i++ {
        ch <- i
    }
    close(ch)  // 关闭 channel，通知接收方没有更多数据
}

func main() {
    ch := make(chan int)
    go producer(ch)

    for v := range ch {  // range 自动检测 channel 关闭
        fmt.Println(v)
    }
}
```

### 3. select 语句

`select` 用于同时监听多个 channel，哪个 channel 就绪就执行哪个分支（随机选择）。

```go
package main

import (
    "fmt"
    "time"
)

func main() {
    ch1 := make(chan string)
    ch2 := make(chan string)

    go func() {
        time.Sleep(1 * time.Second)
        ch1 <- "来自 ch1"
    }()
    go func() {
        time.Sleep(2 * time.Second)
        ch2 <- "来自 ch2"
    }()

    // 等待两个 channel
    for i := 0; i < 2; i++ {
        select {
        case msg := <-ch1:
            fmt.Println(msg)
        case msg := <-ch2:
            fmt.Println(msg)
        }
    }
}
```

**带超时的 select：**

```go
select {
case result := <-ch:
    fmt.Println("收到:", result)
case <-time.After(3 * time.Second):
    fmt.Println("超时！")
}
```

### 4. sync 包

当必须共享内存时，使用 `sync` 包提供的同步原语。

```go
package main

import (
    "fmt"
    "sync"
)

// Mutex（互斥锁）：保护共享数据
type SafeCounter struct {
    mu    sync.Mutex
    count int
}

func (c *SafeCounter) Inc() {
    c.mu.Lock()
    defer c.mu.Unlock()
    c.count++
}

func (c *SafeCounter) Value() int {
    c.mu.Lock()
    defer c.mu.Unlock()
    return c.count
}

// WaitGroup：等待一组 goroutine 完成
func main() {
    counter := &SafeCounter{}
    var wg sync.WaitGroup

    for i := 0; i < 1000; i++ {
        wg.Add(1)  // 计数+1
        go func() {
            defer wg.Done()  // 计数-1
            counter.Inc()
        }()
    }

    wg.Wait()  // 阻塞直到计数归零
    fmt.Println(counter.Value())  // 1000
}
```

**sync.Once：只执行一次（常用于单例初始化）**

```go
var (
    instance *Config
    once     sync.Once
)

func GetConfig() *Config {
    once.Do(func() {
        instance = &Config{} // 只会执行一次，即使并发调用
    })
    return instance
}
```

**sync.RWMutex：读写锁（读多写少场景性能更好）**

```go
var mu sync.RWMutex
var data = make(map[string]string)

func read(key string) string {
    mu.RLock()         // 读锁（允许多个 goroutine 同时读）
    defer mu.RUnlock()
    return data[key]
}

func write(key, val string) {
    mu.Lock()          // 写锁（独占）
    defer mu.Unlock()
    data[key] = val
}
```

---

## 九、包与模块管理

### 1. 包（Package）

Go 程序由包组成。同一目录下的所有 `.go` 文件属于同一个包。

```
myproject/
├── go.mod
├── main.go          (package main)
└── mathutil/
    ├── add.go       (package mathutil)
    └── multiply.go  (package mathutil)
```

```go
// mathutil/add.go
package mathutil

// 首字母大写 = 导出（public），可被其他包使用
func Add(a, b int) int { return a + b }

// 首字母小写 = 未导出（private），只能在包内使用
func helper() {}
```

```go
// main.go
package main

import (
    "fmt"
    "myproject/mathutil"  // 导入路径 = 模块名 + 包目录
)

func main() {
    fmt.Println(mathutil.Add(3, 5))
}
```

**导入别名：**

```go
import (
    "fmt"
    m "myproject/mathutil"  // 别名
    _ "some/package"        // 只执行 init()，不使用包内符号
    . "other/package"       // 直接使用包内符号（不推荐）
)
```

### 2. Go Modules（模块管理）

Go 1.11+ 推荐使用 Go Modules 管理依赖。

```bash
# 初始化模块（生成 go.mod）
go mod init github.com/yourname/yourproject

# go.mod 内容示例：
# module github.com/yourname/yourproject
# go 1.22
# require (
#     github.com/gin-gonic/gin v1.9.1
# )
```

**go.mod 与 go.sum：**
- `go.mod`：记录模块路径、Go 版本、直接依赖及版本
- `go.sum`：记录所有依赖（含间接依赖）的哈希值，确保可重现构建

---

## 十、Go 常用命令

```bash
# ── 运行与构建 ──
go run main.go              # 直接运行
go run .                    # 运行当前目录
go build .                  # 编译当前目录
go build -o app .           # 编译并指定输出文件名
go install .                # 编译并安装到 $GOPATH/bin

# 交叉编译（在 Linux 上编译 Windows 程序）
GOOS=windows GOARCH=amd64 go build -o app.exe .

# ── 测试 ──
go test ./...               # 运行所有测试
go test -v ./...            # 详细输出
go test -run TestFuncName   # 运行指定测试函数
go test -bench=.            # 运行基准测试
go test -cover              # 显示测试覆盖率

# ── 依赖管理 ──
go mod init <module-name>   # 初始化模块
go get <package>            # 添加/更新依赖
go get <package>@v1.2.3     # 指定版本
go get <package>@latest     # 最新版本
go mod tidy                 # 整理依赖（添加缺失、删除多余）
go mod download             # 下载所有依赖到本地缓存
go mod vendor               # 将依赖复制到 vendor 目录
go list -m all              # 列出所有依赖

# ── 代码质量 ──
go fmt ./...                # 格式化代码（强烈推荐，Go 有统一风格）
go vet ./...                # 静态检查（发现常见错误）
go doc fmt.Println          # 查看文档
godoc -http=:6060           # 启动本地文档服务器

# ── 工具 ──
go env                      # 查看 Go 环境变量
go env -w GOPROXY=...       # 设置环境变量
go version                  # 查看 Go 版本
go clean -modcache          # 清除模块缓存
```

---

## 十一、常用标准库

### 1. fmt — 格式化输入输出

```go
import "fmt"

// 打印
fmt.Print("无换行")
fmt.Println("有换行")
fmt.Printf("格式化: %s %d %.2f %t %v\n", "str", 42, 3.14, true, anyVal)

// 格式化为字符串（不打印）
s := fmt.Sprintf("Hello, %s!", "World")

// 打印到 stderr
fmt.Fprintln(os.Stderr, "错误信息")

// 扫描输入
var name string
var age int
fmt.Scan(&name)                    // 读取一个值
fmt.Scanf("%s %d", &name, &age)   // 按格式读取
fmt.Scanln(&name, &age)           // 读取一行
```

**常用格式化动词（format verbs）：**

| 动词 | 说明 | 示例 |
|------|------|------|
| `%v` | 默认格式（万能） | `{Alice 30}` |
| `%+v` | 结构体带字段名 | `{Name:Alice Age:30}` |
| `%#v` | Go 语法表示 | `main.Person{Name:"Alice"}` |
| `%T` | 类型 | `main.Person` |
| `%d` | 十进制整数 | `42` |
| `%b` | 二进制 | `101010` |
| `%x` | 十六进制 | `2a` |
| `%f` | 浮点数 | `3.140000` |
| `%.2f` | 保留2位小数 | `3.14` |
| `%s` | 字符串 | `hello` |
| `%q` | 带引号字符串 | `"hello"` |
| `%p` | 指针地址 | `0xc000...` |
| `%w` | 包装错误（仅 Errorf） | - |

### 2. strings — 字符串操作

```go
import "strings"

s := "Hello, World!"

strings.Contains(s, "World")        // true（是否包含子串）
strings.HasPrefix(s, "Hello")       // true（是否以...开头）
strings.HasSuffix(s, "!")           // true（是否以...结尾）
strings.Count(s, "l")               // 3（子串出现次数）
strings.Index(s, "World")           // 7（子串首次出现的索引，-1表示不存在）
strings.LastIndex(s, "l")           // 10

strings.ToUpper(s)                  // "HELLO, WORLD!"
strings.ToLower(s)                  // "hello, world!"
strings.TrimSpace("  hello  ")      // "hello"（去除首尾空白）
strings.Trim(s, "!")                // "Hello, World"（去除首尾指定字符）
strings.TrimLeft(s, "H")            // "ello, World!"
strings.TrimPrefix(s, "Hello, ")    // "World!"
strings.TrimSuffix(s, "!")          // "Hello, World"

strings.Replace(s, "l", "L", 2)    // "HeLLo, World!"（替换前2个）
strings.ReplaceAll(s, "l", "L")     // "HeLLo, WorLd!"（替换所有）

strings.Split("a,b,c", ",")         // ["a" "b" "c"]
strings.SplitN("a,b,c", ",", 2)     // ["a" "b,c"]（最多分割成2份）
strings.Join([]string{"a","b","c"}, "-")  // "a-b-c"

strings.Repeat("ab", 3)             // "ababab"
strings.Fields("  foo bar  baz  ")  // ["foo" "bar" "baz"]（按空白分割）

// 高效字符串拼接（避免 + 拼接的性能问题）
var sb strings.Builder
sb.WriteString("Hello")
sb.WriteString(", ")
sb.WriteString("World")
result := sb.String()  // "Hello, World"
```

### 3. strconv — 类型转换

```go
import "strconv"

// 数字 → 字符串
strconv.Itoa(42)                    // "42"（int to ASCII）
strconv.FormatFloat(3.14, 'f', 2, 64)  // "3.14"
strconv.FormatBool(true)            // "true"
strconv.FormatInt(255, 16)          // "ff"（转十六进制）

// 字符串 → 数字
n, err := strconv.Atoi("42")        // "42" → 42（ASCII to int）
f, err := strconv.ParseFloat("3.14", 64)
b, err := strconv.ParseBool("true")
i, err := strconv.ParseInt("ff", 16, 64)  // 十六进制字符串 → int64

// 检查字符串是否为数字
_, err = strconv.Atoi("abc")
if err != nil {
    fmt.Println("不是数字")
}
```

### 4. math — 数学运算

```go
import "math"

math.Abs(-3.14)          // 3.14（绝对值）
math.Ceil(1.2)           // 2（向上取整）
math.Floor(1.8)          // 1（向下取整）
math.Round(1.5)          // 2（四舍五入）
math.Sqrt(16)            // 4（平方根）
math.Pow(2, 10)          // 1024（幂运算）
math.Log(math.E)         // 1（自然对数）
math.Log2(8)             // 3
math.Log10(100)          // 2
math.Max(3.0, 5.0)       // 5
math.Min(3.0, 5.0)       // 3
math.Pi                  // 3.141592653589793
math.MaxInt64            // int64 最大值
math.MaxFloat64          // float64 最大值
```

### 5. time — 时间处理

```go
import "time"

// 获取当前时间
now := time.Now()
fmt.Println(now)                    // 2024-01-15 10:30:00.123456789 +0800 CST

// 时间格式化（Go 用特定参考时间：2006-01-02 15:04:05）
now.Format("2006-01-02")            // "2024-01-15"
now.Format("2006-01-02 15:04:05")   // "2024-01-15 10:30:00"
now.Format("01/02/2006")            // "01/15/2024"

// 解析时间字符串
t, err := time.Parse("2006-01-02", "2024-01-15")

// 时间运算
tomorrow := now.Add(24 * time.Hour)
yesterday := now.Add(-24 * time.Hour)
duration := tomorrow.Sub(now)       // time.Duration

// 时间比较
now.Before(tomorrow)   // true
now.After(yesterday)   // true
now.Equal(now)         // true

// 获取时间各部分
now.Year()    // 2024
now.Month()   // January（time.Month 类型）
now.Day()     // 15
now.Hour()    // 10
now.Minute()  // 30
now.Second()  // 0
now.Weekday() // Monday

// 时间戳
now.Unix()      // 秒级时间戳
now.UnixMilli() // 毫秒级
now.UnixNano()  // 纳秒级

// 定时器
time.Sleep(2 * time.Second)         // 休眠2秒
timer := time.NewTimer(5 * time.Second)
<-timer.C                           // 等待5秒后触发

ticker := time.NewTicker(1 * time.Second)
for t := range ticker.C {
    fmt.Println("tick:", t)
    // ticker.Stop() 停止
}
```

### 6. os — 操作系统接口

```go
import "os"

// 文件操作
f, err := os.Open("file.txt")          // 只读打开
f, err = os.Create("file.txt")         // 创建/截断
f, err = os.OpenFile("file.txt",
    os.O_RDWR|os.O_CREATE|os.O_APPEND, 0644)  // 自定义模式
defer f.Close()

f.Write([]byte("hello"))
f.WriteString("world")

data, err := os.ReadFile("file.txt")   // 一次性读取整个文件
err = os.WriteFile("out.txt", data, 0644)  // 一次性写入

err = os.Remove("file.txt")            // 删除文件
err = os.Rename("old.txt", "new.txt")  // 重命名/移动
err = os.MkdirAll("a/b/c", 0755)      // 递归创建目录

// 文件信息
info, err := os.Stat("file.txt")
info.Name()     // 文件名
info.Size()     // 文件大小（字节）
info.IsDir()    // 是否是目录
info.ModTime()  // 修改时间

// 环境变量
os.Getenv("HOME")                      // 获取环境变量
os.Setenv("MY_VAR", "value")           // 设置环境变量
os.Environ()                           // 所有环境变量

// 命令行参数
os.Args                                // []string，os.Args[0] 是程序名

// 退出程序
os.Exit(0)   // 正常退出
os.Exit(1)   // 异常退出
```

### 7. bufio — 缓冲 I/O

```go
import (
    "bufio"
    "os"
    "strings"
)

// 逐行读取文件
f, _ := os.Open("file.txt")
defer f.Close()
scanner := bufio.NewScanner(f)
for scanner.Scan() {
    line := scanner.Text()
    fmt.Println(line)
}

// 读取标准输入
scanner = bufio.NewScanner(os.Stdin)
scanner.Scan()
input := scanner.Text()

// 带缓冲的写入（减少系统调用，提高性能）
writer := bufio.NewWriter(f)
writer.WriteString("hello\n")
writer.Flush()  // 必须 Flush，否则缓冲区数据不会写入

// 从字符串读取
reader := bufio.NewReader(strings.NewReader("hello world"))
word, _ := reader.ReadString(' ')  // 读到空格为止
```

### 8. io — 基础 I/O 接口

`io` 包定义了最基础的 I/O 接口，几乎所有 I/O 操作都基于它。

```go
import (
    "io"
    "os"
    "strings"
    "bytes"
)

// ── 核心接口 ──
// io.Reader：只需实现 Read(p []byte) (n int, err error)
// io.Writer：只需实现 Write(p []byte) (n int, err error)
// io.Closer：只需实现 Close() error
// io.ReadWriter = Reader + Writer
// io.ReadWriteCloser = Reader + Writer + Closer

// ── 常用函数 ──

// io.ReadAll：读取全部内容（Go 1.16+，替代 ioutil.ReadAll）
f, _ := os.Open("file.txt")
defer f.Close()
data, err := io.ReadAll(f)
fmt.Println(string(data))

// io.Copy：从 Reader 复制到 Writer（流式，不占用大量内存）
src, _ := os.Open("src.txt")
dst, _ := os.Create("dst.txt")
n, err := io.Copy(dst, src)  // 返回复制的字节数
fmt.Println("复制了", n, "字节")

// io.CopyN：只复制 N 个字节
io.CopyN(dst, src, 1024)

// io.WriteString：向 Writer 写入字符串
io.WriteString(os.Stdout, "Hello, World!\n")

// io.Discard：丢弃所有写入（类似 /dev/null，常用于测试）
io.Copy(io.Discard, resp.Body)  // 读取并丢弃 HTTP 响应体

// io.LimitReader：限制读取字节数（防止读取过大内容）
limited := io.LimitReader(f, 1024)  // 最多读 1024 字节
data, _ = io.ReadAll(limited)

// io.MultiReader：将多个 Reader 串联成一个
r1 := strings.NewReader("Hello, ")
r2 := strings.NewReader("World!")
combined := io.MultiReader(r1, r2)
data, _ = io.ReadAll(combined)
fmt.Println(string(data))  // Hello, World!

// io.MultiWriter：同时写入多个 Writer
var buf bytes.Buffer
w := io.MultiWriter(os.Stdout, &buf)  // 同时写到标准输出和缓冲区
io.WriteString(w, "同时写入两处\n")

// io.TeeReader：读取时同时写入另一个 Writer（类似 tee 命令）
var logBuf bytes.Buffer
tee := io.TeeReader(f, &logBuf)  // 读 f 的同时，内容也写入 logBuf
io.ReadAll(tee)

// io.Pipe：创建同步的内存管道（一端写，另一端读）
pr, pw := io.Pipe()
go func() {
    pw.Write([]byte("通过管道传输"))
    pw.Close()
}()
data, _ = io.ReadAll(pr)
fmt.Println(string(data))

// ── 常用错误 ──
// io.EOF：读到文件末尾时返回，是正常结束信号，不是真正的错误
// io.ErrUnexpectedEOF：期望更多数据但遇到了 EOF
// io.ErrClosedPipe：向已关闭的管道写入
```

**io 包的设计哲学：**
- 基于接口（`Reader`/`Writer`），任何实现了这两个接口的类型都可以互相组合
- `os.File`、`bytes.Buffer`、`strings.Reader`、`net.Conn`、`http.Response.Body` 等都实现了这些接口
- 这使得代码可以不关心数据来源（文件/网络/内存），只需操作接口

### 9. encoding/json — JSON 处理

```go
import "encoding/json"

type Person struct {
    Name  string `json:"name"`
    Age   int    `json:"age"`
    Email string `json:"email,omitempty"`
}

// 序列化（struct → JSON）
p := Person{Name: "Alice", Age: 30}
data, err := json.Marshal(p)
fmt.Println(string(data))  // {"name":"Alice","age":30}

// 美化输出
data, err = json.MarshalIndent(p, "", "  ")

// 反序列化（JSON → struct）
jsonStr := `{"name":"Bob","age":25,"email":"bob@example.com"}`
var p2 Person
err = json.Unmarshal([]byte(jsonStr), &p2)
fmt.Println(p2.Name, p2.Age)

// 解析未知结构的 JSON
var result map[string]any
json.Unmarshal([]byte(jsonStr), &result)
fmt.Println(result["name"])  // Bob

// 流式编解码（处理大文件）
encoder := json.NewEncoder(os.Stdout)
encoder.Encode(p)

decoder := json.NewDecoder(strings.NewReader(jsonStr))
decoder.Decode(&p2)
```

### 9. net/http — HTTP 编程

```go
import "net/http"

// ── HTTP 客户端 ──
// GET 请求
resp, err := http.Get("https://api.example.com/users")
defer resp.Body.Close()
body, _ := io.ReadAll(resp.Body)
fmt.Println(string(body))

// POST 请求
data := strings.NewReader(`{"name":"Alice"}`)
resp, err = http.Post("https://api.example.com/users",
    "application/json", data)

// 自定义请求（设置 Header、超时等）
client := &http.Client{Timeout: 10 * time.Second}
req, _ := http.NewRequest("GET", "https://api.example.com", nil)
req.Header.Set("Authorization", "Bearer token123")
resp, err = client.Do(req)

// ── HTTP 服务器 ──
// 简单服务器
http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
    fmt.Fprintf(w, "Hello, %s!", r.URL.Path[1:])
})

http.HandleFunc("/api/users", func(w http.ResponseWriter, r *http.Request) {
    switch r.Method {
    case http.MethodGet:
        w.Header().Set("Content-Type", "application/json")
        json.NewEncoder(w).Encode(map[string]string{"name": "Alice"})
    case http.MethodPost:
        var body map[string]any
        json.NewDecoder(r.Body).Decode(&body)
        w.WriteHeader(http.StatusCreated)
    default:
        http.Error(w, "Method Not Allowed", http.StatusMethodNotAllowed)
    }
})

http.ListenAndServe(":8080", nil)
```

### 10. sort — 排序

```go
import "sort"

// 基本类型排序
nums := []int{5, 2, 8, 1, 9, 3}
sort.Ints(nums)                    // 升序
fmt.Println(nums)                  // [1 2 3 5 8 9]

strs := []string{"banana", "apple", "cherry"}
sort.Strings(strs)                 // 字典序
fmt.Println(strs)

floats := []float64{3.14, 1.41, 2.71}
sort.Float64s(floats)

// 自定义排序（实现 sort.Interface 或用 sort.Slice）
type Person struct{ Name string; Age int }
people := []Person{{"Alice", 30}, {"Bob", 25}, {"Charlie", 35}}

// sort.Slice：最简单的自定义排序
sort.Slice(people, func(i, j int) bool {
    return people[i].Age < people[j].Age  // 按年龄升序
})

// 稳定排序（相等元素保持原顺序）
sort.SliceStable(people, func(i, j int) bool {
    return people[i].Name < people[j].Name
})

// 二分查找（要求已排序）
idx := sort.SearchInts(nums, 5)    // 返回 5 应插入的位置
found := sort.Search(len(nums), func(i int) bool {
    return nums[i] >= 5
})
fmt.Println(idx, found)

// 检查是否已排序
sort.IntsAreSorted(nums)           // true
```

### 11. regexp — 正则表达式

```go
import "regexp"

// 编译正则（建议用 MustCompile，编译失败直接 panic）
re := regexp.MustCompile(`\d+`)

re.MatchString("abc123")           // true（是否匹配）
re.FindString("abc123def456")      // "123"（第一个匹配）
re.FindAllString("abc123def456", -1)  // ["123" "456"]（所有匹配）
re.ReplaceAllString("abc123", "NUM")  // "abcNUM"

// 捕获组
re2 := regexp.MustCompile(`(\w+)@(\w+)\.(\w+)`)
match := re2.FindStringSubmatch("user@example.com")
// match[0]="user@example.com", match[1]="user", match[2]="example", match[3]="com"

// 命名捕获组
re3 := regexp.MustCompile(`(?P<user>\w+)@(?P<domain>\w+)`)
names := re3.SubexpNames()  // ["", "user", "domain"]
```

### 12. log — 日志

```go
import "log"

log.Println("普通日志")           // 带时间戳
log.Printf("格式化: %d", 42)
log.Fatal("致命错误")             // 打印后调用 os.Exit(1)
log.Panic("严重错误")             // 打印后调用 panic

// 自定义 logger
logger := log.New(os.Stderr, "[ERROR] ", log.Ldate|log.Ltime|log.Lshortfile)
logger.Println("自定义日志")
```

### 13. path/filepath — 文件路径

```go
import "path/filepath"

filepath.Join("a", "b", "c.txt")   // "a/b/c.txt"（跨平台路径拼接）
filepath.Dir("/a/b/c.txt")          // "/a/b"
filepath.Base("/a/b/c.txt")         // "c.txt"
filepath.Ext("file.go")             // ".go"
filepath.Abs("./file.txt")          // 绝对路径

// 遍历目录
filepath.Walk(".", func(path string, info os.FileInfo, err error) error {
    if err != nil { return err }
    fmt.Println(path)
    return nil
})

// Go 1.16+ 推荐用 filepath.WalkDir（更高效）
filepath.WalkDir(".", func(path string, d os.DirEntry, err error) error {
    fmt.Println(path, d.IsDir())
    return nil
})
```

---

## 十二、泛型（Go 1.18+）

泛型允许编写适用于多种类型的通用代码，避免重复。

```go
package main

import "fmt"

// 泛型函数：[T comparable] 是类型参数，comparable 是约束（支持 == 比较）
func Contains[T comparable](slice []T, item T) bool {
    for _, v := range slice {
        if v == item {
            return true
        }
    }
    return false
}

// 自定义约束（使用 interface 定义）
type Number interface {
    int | int32 | int64 | float32 | float64
}

func Sum[T Number](nums []T) T {
    var total T
    for _, n := range nums {
        total += n
    }
    return total
}

// 泛型结构体
type Stack[T any] struct {
    items []T
}

func (s *Stack[T]) Push(item T) { s.items = append(s.items, item) }
func (s *Stack[T]) Pop() (T, bool) {
    var zero T
    if len(s.items) == 0 {
        return zero, false
    }
    item := s.items[len(s.items)-1]
    s.items = s.items[:len(s.items)-1]
    return item, true
}

func main() {
    fmt.Println(Contains([]int{1, 2, 3}, 2))          // true
    fmt.Println(Contains([]string{"a", "b"}, "c"))    // false

    fmt.Println(Sum([]int{1, 2, 3, 4, 5}))            // 15
    fmt.Println(Sum([]float64{1.1, 2.2, 3.3}))        // 6.6

    s := &Stack[int]{}
    s.Push(1)
    s.Push(2)
    v, _ := s.Pop()
    fmt.Println(v)  // 2
}
```

**常用内置约束（`golang.org/x/exp/constraints` 包）：**

| 约束 | 说明 |
|------|------|
| `any` | 等价于 `interface{}`，无限制 |
| `comparable` | 支持 `==` 和 `!=` 比较 |
| `constraints.Ordered` | 支持 `<`、`>`、`<=`、`>=` 比较（数字+字符串） |
| `constraints.Integer` | 所有整数类型 |
| `constraints.Float` | 所有浮点类型 |

---

## 附录：常用第三方库推荐

| 类别 | 库 | 说明 |
|------|-----|------|
| Web 框架 | `github.com/gin-gonic/gin` | 高性能 HTTP 框架，最流行 |
| Web 框架 | `github.com/gofiber/fiber` | 类 Express 风格，极速 |
| ORM | `gorm.io/gorm` | 功能完整的 ORM 框架 |
| 数据库驱动 | `github.com/go-sql-driver/mysql` | MySQL 驱动 |
| 配置管理 | `github.com/spf13/viper` | 支持多种格式的配置库 |
| CLI 工具 | `github.com/spf13/cobra` | 命令行应用框架 |
| 日志 | `go.uber.org/zap` | 高性能结构化日志 |
| 日志 | `github.com/sirupsen/logrus` | 功能丰富的日志库 |
| 测试 | `github.com/stretchr/testify` | 断言库，简化测试代码 |
| HTTP 客户端 | `github.com/go-resty/resty` | 链式调用的 HTTP 客户端 |
| UUID | `github.com/google/uuid` | UUID 生成 |
| 时间 | `github.com/jinzhu/now` | 时间处理增强 |

---

## 十三、配置文件解析（JSON / YAML）

### 1. JSON 配置文件读取

```go
// config.json
// {
//   "server": { "host": "localhost", "port": 8080 },
//   "database": { "dsn": "user:pass@tcp(127.0.0.1:3306)/db" },
//   "debug": true
// }

package main

import (
    "encoding/json"
    "fmt"
    "os"
)

type Config struct {
    Server struct {
        Host string `json:"host"`
        Port int    `json:"port"`
    } `json:"server"`
    Database struct {
        DSN string `json:"dsn"`
    } `json:"database"`
    Debug bool `json:"debug"`
}

func loadConfig(path string) (*Config, error) {
    data, err := os.ReadFile(path)
    if err != nil {
        return nil, fmt.Errorf("读取配置文件失败: %w", err)
    }
    var cfg Config
    if err := json.Unmarshal(data, &cfg); err != nil {
        return nil, fmt.Errorf("解析配置文件失败: %w", err)
    }
    return &cfg, nil
}

func main() {
    cfg, err := loadConfig("config.json")
    if err != nil {
        panic(err)
    }
    fmt.Printf("服务器: %s:%d\n", cfg.Server.Host, cfg.Server.Port)
    fmt.Printf("调试模式: %t\n", cfg.Debug)
}
```

### 2. YAML 配置文件读取

YAML 是比 JSON 更适合人类阅读的配置格式，需要第三方库 `gopkg.in/yaml.v3`。

```bash
go get gopkg.in/yaml.v3
```

```yaml
# config.yaml
server:
  host: localhost
  port: 8080
database:
  dsn: "user:pass@tcp(127.0.0.1:3306)/mydb?charset=utf8mb4&parseTime=True"
  max_open_conns: 10
  max_idle_conns: 5
redis:
  addr: "localhost:6379"
  password: ""
  db: 0
debug: true
```

```go
package main

import (
    "fmt"
    "os"
    "gopkg.in/yaml.v3"
)

type Config struct {
    Server struct {
        Host string `yaml:"host"`
        Port int    `yaml:"port"`
    } `yaml:"server"`
    Database struct {
        DSN          string `yaml:"dsn"`
        MaxOpenConns int    `yaml:"max_open_conns"`
        MaxIdleConns int    `yaml:"max_idle_conns"`
    } `yaml:"database"`
    Redis struct {
        Addr     string `yaml:"addr"`
        Password string `yaml:"password"`
        DB       int    `yaml:"db"`
    } `yaml:"redis"`
    Debug bool `yaml:"debug"`
}

func loadConfig(path string) (*Config, error) {
    data, err := os.ReadFile(path)
    if err != nil {
        return nil, err
    }
    var cfg Config
    if err := yaml.Unmarshal(data, &cfg); err != nil {
        return nil, err
    }
    return &cfg, nil
}

func main() {
    cfg, err := loadConfig("config.yaml")
    if err != nil {
        panic(err)
    }
    fmt.Printf("服务器: %s:%d\n", cfg.Server.Host, cfg.Server.Port)
    fmt.Printf("数据库: %s\n", cfg.Database.DSN)
    fmt.Printf("Redis: %s\n", cfg.Redis.Addr)
}
```

**YAML 标签说明：**
- `yaml:"field_name"`：指定 YAML 中的键名
- `yaml:"name,omitempty"`：零值时忽略（数字 `0`、字符串 `""`、布尔 `false`、nil 指针/切片/map 不会输出）
- `yaml:"-"`：序列化和反序列化时都忽略该字段
- `yaml:",inline"`：将嵌套结构体的字段展开到同一层级
- `yaml:",flow"`：使用行内流式格式输出（如 `[1, 2, 3]` 而非多行列表）

**JSON 与 YAML 标签对比：**

| 功能 | JSON 写法 | YAML 写法 |
|------|-----------|-----------|
| 指定键名 | `json:"name"` | `yaml:"name"` |
| 零值忽略 | `json:"name,omitempty"` | `yaml:"name,omitempty"` |
| 忽略字段 | `json:"-"` | `yaml:"-"` |
| 仅序列化时使用字段名 | `json:"Name"` | `yaml:"Name"` |

### 3. 使用 Viper 管理配置（推荐）

`github.com/spf13/viper` 支持 JSON/YAML/TOML/ENV 等多种格式，还支持热重载和环境变量覆盖。

```bash
go get github.com/spf13/viper
```

```go
package main

import (
    "fmt"
    "github.com/spf13/viper"
)

func initConfig() {
    viper.SetConfigName("config")   // 文件名（不含扩展名）
    viper.SetConfigType("yaml")     // 文件类型
    viper.AddConfigPath(".")        // 搜索路径
    viper.AddConfigPath("./config") // 可添加多个路径

    // 环境变量覆盖（如 APP_SERVER_PORT=9090 会覆盖 server.port）
    viper.SetEnvPrefix("APP")
    viper.AutomaticEnv()

    // 默认值
    viper.SetDefault("server.port", 8080)
    viper.SetDefault("debug", false)

    if err := viper.ReadInConfig(); err != nil {
        panic(fmt.Errorf("读取配置失败: %w", err))
    }
}

func main() {
    initConfig()

    host := viper.GetString("server.host")
    port := viper.GetInt("server.port")
    debug := viper.GetBool("debug")
    dsn := viper.GetString("database.dsn")

    fmt.Printf("服务器: %s:%d, 调试: %t\n", host, port, debug)
    fmt.Println("DSN:", dsn)

    // 监听配置文件变化（热重载）
    viper.WatchConfig()
}

---

## 十四、GORM 数据库操作

GORM 是 Go 最流行的 ORM 框架，支持 MySQL、PostgreSQL、SQLite、SQL Server。

```bash
go get gorm.io/gorm
go get gorm.io/driver/mysql   # MySQL 驱动
```

### 1. 连接数据库与模型定义

```go
package main

import (
    "gorm.io/driver/mysql"
    "gorm.io/gorm"
    "gorm.io/gorm/logger"
    "time"
)

// 模型定义：嵌入 gorm.Model 自动获得 ID、CreatedAt、UpdatedAt、DeletedAt 字段
type User struct {
    gorm.Model                        // 包含 ID(uint), CreatedAt, UpdatedAt, DeletedAt
    Name     string `gorm:"size:100;not null"`
    Email    string `gorm:"uniqueIndex;size:200"`
    Age      int    `gorm:"default:0"`
    Role     string `gorm:"type:varchar(20);default:'user'"`
}

// 自定义主键和表名
type Product struct {
    ID        uint      `gorm:"primaryKey;autoIncrement"`
    Name      string    `gorm:"column:product_name;size:200"`
    Price     float64
    Stock     int
    CreatedAt time.Time
}

// TableName 自定义表名（默认是结构体名的蛇形复数，如 users、products）
func (Product) TableName() string { return "t_product" }

var DB *gorm.DB

func initDB() {
    dsn := "user:password@tcp(127.0.0.1:3306)/mydb?charset=utf8mb4&parseTime=True&loc=Local"
    var err error
    DB, err = gorm.Open(mysql.Open(dsn), &gorm.Config{
        Logger: logger.Default.LogMode(logger.Info), // 打印 SQL 日志
    })
    if err != nil {
        panic("数据库连接失败: " + err.Error())
    }

    // 连接池配置
    sqlDB, _ := DB.DB()
    sqlDB.SetMaxOpenConns(100)  // 最大连接数
    sqlDB.SetMaxIdleConns(10)   // 最大空闲连接数

    // 自动迁移（根据结构体创建/更新表结构）
    DB.AutoMigrate(&User{}, &Product{})
}
```

### 2. CRUD 操作

```go
// ── Create（创建）──
user := User{Name: "Alice", Email: "alice@example.com", Age: 30}
result := DB.Create(&user)
fmt.Println(user.ID, result.Error)  // 创建后 ID 自动填充

// 批量创建
users := []User{
    {Name: "Bob", Email: "bob@example.com"},
    {Name: "Charlie", Email: "charlie@example.com"},
}
DB.Create(&users)

// ── Read（查询）──
// 按主键查询
var u User
DB.First(&u, 1)           // SELECT * FROM users WHERE id=1 LIMIT 1
DB.First(&u, "id = ?", 1) // 等价写法

// 条件查询
DB.Where("name = ?", "Alice").First(&u)
DB.Where("age > ? AND role = ?", 18, "user").Find(&users)

// 查询所有
var allUsers []User
DB.Find(&allUsers)

// 选择特定字段
DB.Select("name", "email").Find(&allUsers)

// 排序、分页
DB.Order("created_at DESC").Limit(10).Offset(20).Find(&allUsers)

// 统计
var count int64
DB.Model(&User{}).Where("age > ?", 18).Count(&count)

// ── Update（更新）──
// 更新单个字段
DB.Model(&u).Update("name", "Alice2")

// 更新多个字段（用 map 避免零值问题）
DB.Model(&u).Updates(map[string]any{
    "name": "Alice3",
    "age":  31,
})

// 更新结构体（零值字段会被忽略！）
DB.Model(&u).Updates(User{Name: "Alice4", Age: 32})

// 强制更新零值字段
DB.Model(&u).Select("age").Updates(User{Age: 0})

// ── Delete（删除）──
DB.Delete(&u)              // 软删除（设置 DeletedAt，数据仍在）
DB.Unscoped().Delete(&u)   // 硬删除（真正从数据库删除）
DB.Where("age < ?", 18).Delete(&User{})
```

### 3. 高级查询

```go
// 原生 SQL
var users []User
DB.Raw("SELECT * FROM users WHERE age > ?", 18).Scan(&users)
DB.Exec("UPDATE users SET role = ? WHERE id = ?", "admin", 1)

// 事务（Transaction）
err := DB.Transaction(func(tx *gorm.DB) error {
    if err := tx.Create(&User{Name: "Dave"}).Error; err != nil {
        return err  // 返回错误会自动回滚
    }
    if err := tx.Model(&Product{}).Where("id = ?", 1).
        Update("stock", gorm.Expr("stock - ?", 1)).Error; err != nil {
        return err
    }
    return nil  // 返回 nil 自动提交
})

// Preload（预加载关联，解决 N+1 问题）
type Order struct {
    gorm.Model
    UserID uint
    User   User    // 关联 User
    Items  []Item  // 一对多
}
type Item struct {
    gorm.Model
    OrderID uint
    Name    string
}

var orders []Order
DB.Preload("User").Preload("Items").Find(&orders)

// Joins（连表查询）
DB.Joins("JOIN orders ON orders.user_id = users.id").
    Where("orders.created_at > ?", time.Now().AddDate(0, -1, 0)).
    Find(&users)

// 错误处理
result := DB.First(&u, 999)
if result.Error != nil {
    if errors.Is(result.Error, gorm.ErrRecordNotFound) {
        fmt.Println("记录不存在")
    }
}
```

---

## 十五、常用中间件交互

### 1. Redis（go-redis）

```bash
go get github.com/redis/go-redis/v9
```

```go
package main

import (
    "context"
    "fmt"
    "time"
    "github.com/redis/go-redis/v9"
)

var rdb *redis.Client
var ctx = context.Background()

func initRedis() {
    rdb = redis.NewClient(&redis.Options{
        Addr:     "localhost:6379",
        Password: "",  // 无密码留空
        DB:       0,
    })
    // 测试连接
    if err := rdb.Ping(ctx).Err(); err != nil {
        panic("Redis 连接失败: " + err.Error())
    }
}

func main() {
    initRedis()

    // ── String（字符串）──
    rdb.Set(ctx, "name", "Alice", 10*time.Minute)  // 设置，10分钟过期
    val, _ := rdb.Get(ctx, "name").Result()
    fmt.Println(val)  // Alice

    rdb.Incr(ctx, "counter")   // 自增（原子操作，常用于计数器）
    rdb.IncrBy(ctx, "counter", 5)

    // ── Hash（哈希，适合存储对象）──
    rdb.HSet(ctx, "user:1", "name", "Alice", "age", 30)
    rdb.HSet(ctx, "user:1", map[string]any{"email": "alice@example.com"})
    name, _ := rdb.HGet(ctx, "user:1", "name").Result()
    all, _ := rdb.HGetAll(ctx, "user:1").Result()  // map[string]string
    fmt.Println(name, all)

    // ── List（列表，适合消息队列、最新N条）──
    rdb.LPush(ctx, "queue", "task1", "task2")  // 从左插入
    rdb.RPush(ctx, "queue", "task3")           // 从右插入
    task, _ := rdb.LPop(ctx, "queue").Result() // 从左弹出
    rdb.BLPop(ctx, 5*time.Second, "queue")     // 阻塞弹出（超时5秒）
    fmt.Println(task)

    // ── Set（集合，去重）──
    rdb.SAdd(ctx, "tags", "go", "backend", "go")  // 自动去重
    members, _ := rdb.SMembers(ctx, "tags").Result()
    rdb.SIsMember(ctx, "tags", "go")  // 是否存在

    // ── ZSet（有序集合，排行榜）──
    rdb.ZAdd(ctx, "leaderboard",
        redis.Z{Score: 100, Member: "Alice"},
        redis.Z{Score: 85, Member: "Bob"},
        redis.Z{Score: 95, Member: "Charlie"},
    )
    // 按分数降序获取前3名
    top3, _ := rdb.ZRevRangeWithScores(ctx, "leaderboard", 0, 2).Result()
    for _, z := range top3 {
        fmt.Printf("%s: %.0f\n", z.Member, z.Score)
    }

    // ── 过期与删除 ──
    rdb.Expire(ctx, "name", 1*time.Hour)  // 设置过期时间
    rdb.TTL(ctx, "name")                  // 查看剩余时间
    rdb.Del(ctx, "name", "counter")       // 删除

    // ── 分布式锁 ──
    ok, _ := rdb.SetNX(ctx, "lock:order", "1", 30*time.Second).Result()
    if ok {
        defer rdb.Del(ctx, "lock:order")
        // 执行需要加锁的操作...
    }

    // ── Pipeline（批量命令，减少网络往返）──
    pipe := rdb.Pipeline()
    pipe.Set(ctx, "k1", "v1", 0)
    pipe.Set(ctx, "k2", "v2", 0)
    pipe.Get(ctx, "k1")
    pipe.Exec(ctx)
}
```

### 2. 消息队列

#### RabbitMQ（amqp091-go）

```bash
go get github.com/rabbitmq/amqp091-go
```

```go
package main

import (
    "fmt"
    amqp "github.com/rabbitmq/amqp091-go"
)

func main() {
    conn, err := amqp.Dial("amqp://guest:guest@localhost:5672/")
    if err != nil { panic(err) }
    defer conn.Close()

    ch, _ := conn.Channel()
    defer ch.Close()

    // 声明队列（不存在则创建）
    q, _ := ch.QueueDeclare(
        "task_queue", // 队列名
        true,         // durable：持久化（重启后不丢失）
        false, false, false, nil,
    )

    // ── 生产者：发送消息 ──
    body := `{"action":"send_email","to":"alice@example.com"}`
    ch.PublishWithContext(ctx,
        "",     // exchange
        q.Name, // routing key
        false, false,
        amqp.Publishing{
            DeliveryMode: amqp.Persistent,  // 消息持久化
            ContentType:  "application/json",
            Body:         []byte(body),
        },
    )
    fmt.Println("消息已发送")

    // ── 消费者：接收消息 ──
    msgs, _ := ch.Consume(q.Name, "", false, false, false, false, nil)
    for msg := range msgs {
        fmt.Printf("收到消息: %s\n", msg.Body)
        // 处理完成后手动确认（防止消息丢失）
        msg.Ack(false)
        // msg.Nack(false, true)  // 处理失败，重新入队
    }
}
```

#### Kafka（kafka-go）

```bash
go get github.com/segmentio/kafka-go
```

```go
package main

import (
    "context"
    "fmt"
    "github.com/segmentio/kafka-go"
)

// 生产者
func produce() {
    w := &kafka.Writer{
        Addr:     kafka.TCP("localhost:9092"),
        Topic:    "order-events",
        Balancer: &kafka.LeastBytes{},
    }
    defer w.Close()

    err := w.WriteMessages(context.Background(),
        kafka.Message{Key: []byte("order-1"), Value: []byte(`{"id":1,"status":"created"}`)},
        kafka.Message{Key: []byte("order-2"), Value: []byte(`{"id":2,"status":"paid"}`)},
    )
    if err != nil { panic(err) }
}

// 消费者
func consume() {
    r := kafka.NewReader(kafka.ReaderConfig{
        Brokers:  []string{"localhost:9092"},
        Topic:    "order-events",
        GroupID:  "order-service",  // 消费者组（同组内消息只被消费一次）
        MinBytes: 1,
        MaxBytes: 10e6,
    })
    defer r.Close()

    for {
        msg, err := r.ReadMessage(context.Background())
        if err != nil { break }
        fmt.Printf("分区%d 偏移%d: key=%s value=%s\n",
            msg.Partition, msg.Offset, msg.Key, msg.Value)
        // kafka-go 的 ReadMessage 自动提交 offset
        // 若需手动提交，用 r.FetchMessage + r.CommitMessages
    }
}
```

### 3. HTTP 中间件（以 Gin 为例）

```bash
go get github.com/gin-gonic/gin
```

```go
package main

import (
    "net/http"
    "time"
    "github.com/gin-gonic/gin"
)

// 自定义中间件：日志记录
func LoggerMiddleware() gin.HandlerFunc {
    return func(c *gin.Context) {
        start := time.Now()
        c.Next()  // 执行后续处理器
        duration := time.Since(start)
        fmt.Printf("[%s] %s %s %d %v\n",
            time.Now().Format("2006-01-02 15:04:05"),
            c.Request.Method, c.Request.URL.Path,
            c.Writer.Status(), duration)
    }
}

// 自定义中间件：JWT 认证
func AuthMiddleware() gin.HandlerFunc {
    return func(c *gin.Context) {
        token := c.GetHeader("Authorization")
        if token == "" {
            c.JSON(http.StatusUnauthorized, gin.H{"error": "未授权"})
            c.Abort()  // 终止后续处理器
            return
        }
        // 验证 token，将用户信息存入上下文
        c.Set("userID", 123)
        c.Next()
    }
}

func main() {
    r := gin.New()

    // 全局中间件
    r.Use(gin.Recovery())  // 捕获 panic，返回 500
    r.Use(LoggerMiddleware())

    // 路由组 + 中间件
    api := r.Group("/api")
    api.Use(AuthMiddleware())
    {
        api.GET("/users", func(c *gin.Context) {
            userID, _ := c.Get("userID")
            c.JSON(http.StatusOK, gin.H{"userID": userID})
        })

        api.POST("/users", func(c *gin.Context) {
            var body struct {
                Name  string `json:"name" binding:"required"`
                Email string `json:"email" binding:"required,email"`
            }
            // binding 自动验证参数
            if err := c.ShouldBindJSON(&body); err != nil {
                c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
                return
            }
            c.JSON(http.StatusCreated, gin.H{"name": body.Name})
        })
    }

    // 路径参数和查询参数
    r.GET("/users/:id", func(c *gin.Context) {
        id := c.Param("id")           // 路径参数 /users/123
        page := c.Query("page")       // 查询参数 ?page=1
        size := c.DefaultQuery("size", "10")  // 带默认值
        c.JSON(http.StatusOK, gin.H{"id": id, "page": page, "size": size})
    })

    r.Run(":8080")
}
