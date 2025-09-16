# Go 语言（Golang）

## 一、Go 语言基础

### 1. 环境搭建

- 下载并安装 Go 编译器（从官方网站https://golang.org/）
- 配置环境变量（GOPATH 和 GOROOT）
- 选择合适的开发工具（VS Code+Go 插件、GoLand 等）
- 验证安装：`go version`

### 2. 第一个 Go 程序

```go
package main

import "fmt"

func main() {
    fmt.Println("Hello, Go World!")
}
```

运行方法：

```bash
go run hello.go
```

### 3. 基本语法

#### 变量与常量

```go
package main

import "fmt"

// 常量定义
const PI = 3.14159
// 一次性定义多个常量
const (
    StatusOK = 200
    StatusNotFound = 404
)

func main() {
    // 变量声明方式1
    var age int
    age = 25

    // 变量声明方式2
    var name string = "Alice"

    // 变量声明方式3（类型推断）
    height := 1.75

    // 多变量声明
    var a, b int = 10, 20
    x, y := "Go", true

    fmt.Printf("Name: %s, Age: %d, Height: %.2f\n", name, age, height)
    fmt.Printf("a: %d, b: %d, x: %s, y: %t\n", a, b, x, y)
    fmt.Printf("PI: %.5f, StatusOK: %d\n", PI, StatusOK)
}
```

#### 基本数据类型

```go
package main

import "fmt"

func main() {
    // 整数类型
    var i int = 42
    var ui uint = 100
    var i8 int8 = -128
    var i64 int64 = 9223372036854775807

    // 浮点类型
    var f32 float32 = 3.14
    var f64 float64 = 2.718281828459045

    // 布尔类型
    var isReady bool = true

    // 字符串类型
    var str string = "Hello, Go"

    fmt.Printf("int: %d, uint: %d\n", i, ui)
    fmt.Printf("int8: %d, int64: %d\n", i8, i64)
    fmt.Printf("float32: %.2f, float64: %.6f\n", f32, f64)
    fmt.Printf("bool: %t\n", isReady)
    fmt.Printf("string: %s\n", str)
}
```

### 4. 控制流

#### 条件语句

```go
package main

import "fmt"

func main() {
    score := 85

    // 基本if语句
    if score >= 60 {
        fmt.Println("Passed")
    } else {
        fmt.Println("Failed")
    }

    // if-else if-else
    if score >= 90 {
        fmt.Println("Grade: A")
    } else if score >= 80 {
        fmt.Println("Grade: B")
    } else if score >= 70 {
        fmt.Println("Grade: C")
    } else if score >= 60 {
        fmt.Println("Grade: D")
    } else {
        fmt.Println("Grade: F")
    }

    // if语句中的初始化
    if num := 10; num%2 == 0 {
        fmt.Println(num, "is even")
    } else {
        fmt.Println(num, "is odd")
    }
}
```

#### 循环语句

```go
package main

import "fmt"

func main() {
    // 基本for循环
    fmt.Println("Counting from 1 to 5:")
    for i := 1; i <= 5; i++ {
        fmt.Printf("%d ", i)
    }
    fmt.Println()

    // 类似while的用法
    fmt.Println("Counting down from 5 to 1:")
    j := 5
    for j >= 1 {
        fmt.Printf("%d ", j)
        j--
    }
    fmt.Println()

    // 无限循环与break
    fmt.Println("Printing multiples of 3 up to 15:")
    k := 3
    for {
        fmt.Printf("%d ", k)
        k += 3
        if k > 15 {
            break
        }
    }
    fmt.Println()

    // continue语句
    fmt.Println("Even numbers between 1 and 10:")
    for n := 1; n <= 10; n++ {
        if n%2 != 0 {
            continue
        }
        fmt.Printf("%d ", n)
    }
    fmt.Println()
}
```

```go
package main

import "fmt"

func main() {
    // 定义一个int类型的数组（长度为5）
    nums := [5]int{10, 20, 30, 40, 50}
    
    // 1. 传统for循环（通过索引访问）
    fmt.Println("使用传统for循环打印：")
    for i := 0; i < len(nums); i++ {
        fmt.Printf("nums[%d] = %d  ", i, nums[i])
    }
    fmt.Println("\n") // 换行
    
    // 2. for range循环（直接获取元素，更简洁）
    fmt.Println("使用for range循环打印：")
    for index, value := range nums {
        fmt.Printf("索引：%d，值：%d  ", index, value)
    }
    fmt.Println()
}
```

#### 开关语句

```go
package main

import "fmt"

func main() {
    // 基本switch
    day := 3
    switch day {
    case 1:
        fmt.Println("Monday")
    case 2:
        fmt.Println("Tuesday")
    case 3:
        fmt.Println("Wednesday")
    case 4:
        fmt.Println("Thursday")
    case 5:
        fmt.Println("Friday")
    case 6, 7: // 多个值
        fmt.Println("Weekend")
    default:
        fmt.Println("Invalid day")
    }

    // 带初始化的switch
    switch num := 7; num % 2 {
    case 0:
        fmt.Println(num, "is even")
    case 1:
        fmt.Println(num, "is odd")
    }

    // 没有条件的switch（类似多个if-else）
    temperature := 25
    switch {
    case temperature < 0:
        fmt.Println("Freezing cold")
    case temperature < 10:
        fmt.Println("Cold")
    case temperature < 20:
        fmt.Println("Mild")
    case temperature < 30:
        fmt.Println("Warm")
    default:
        fmt.Println("Hot")
    }
}
```

### 5. 函数

```go
package main

import "fmt"

// 基本函数
func add(a, b int) int {
    return a + b
}

// 多返回值函数
func divide(dividend, divisor float64) (float64, error) {
    if divisor == 0 {
        return 0, fmt.Errorf("cannot divide by zero")
    }
    return dividend / divisor, nil
}

// 命名返回值
func calculate(a, b int) (sum, product int) {
    sum = a + b
    product = a * b
    return // 不需要显式指定返回值
}

// 可变参数函数
func sum(numbers ...int) int {
    total := 0
    for _, n := range numbers {
        total += n
    }
    return total
}

// 函数作为参数
func apply(op func(int, int) int, a, b int) int {
    return op(a, b)
}

func main() {
    // 调用基本函数
    fmt.Println("3 + 5 =", add(3, 5))

    // 调用多返回值函数
    result, err := divide(10, 2)
    if err != nil {
        fmt.Println("Error:", err)
    } else {
        fmt.Println("10 / 2 =", result)
    }

    // 调用命名返回值函数
    s, p := calculate(4, 5)
    fmt.Println("4 + 5 =", s, ", 4 * 5 =", p)

    // 调用可变参数函数
    fmt.Println("Sum of 1, 2, 3, 4 =", sum(1, 2, 3, 4))

    // 传递函数作为参数
    multiply := func(x, y int) int {
        return x * y
    }
    // 函数内定义的 “匿名函数” 更接近传统意义上的 Lambda 表达式，但 “函数内函数”（嵌套函数）≠ Lambda—— 两者的核心区别在于 “是否匿名”
    fmt.Println("3 * 7 =", apply(multiply, 3, 7))
    fmt.Println("10 - 4 =", apply(func(x, y int) int { return x - y }, 10, 4))
}
```

## 二、Go 语言核心特性

### 1. 数组与切片

```go
package main

import "fmt"

func main() {
    // 数组
    var numbers [5]int
    numbers[0] = 10
    numbers[1] = 20
    numbers[2] = 30

    fmt.Println("Array:", numbers)
    fmt.Println("Array length:", len(numbers))

    // 数组初始化
    fruits := [3]string{"apple", "banana", "cherry"}
    fmt.Println("Fruits array:", fruits)

    // 多维数组
    var matrix [2][3]int
    matrix[0] = [3]int{1, 2, 3}
    matrix[1] = [3]int{4, 5, 6}
    fmt.Println("Matrix:", matrix)

    // 切片
    slice1 := []int{1, 2, 3, 4, 5}
    fmt.Println("Slice1:", slice1)

    // 从数组创建切片
    slice2 := fruits[1:3] // 包含索引1，不包含索引3
    fmt.Println("Slice2 from array:", slice2)

    // 使用make创建切片
    slice3 := make([]int, 3, 5) // 长度3，容量5
    fmt.Printf("Slice3: %v, length: %d, capacity: %d\n", slice3, len(slice3), cap(slice3))

    // 切片追加元素
    slice3 = append(slice3, 10, 20)
    fmt.Printf("Slice3 after append: %v, length: %d, capacity: %d\n", slice3, len(slice3), cap(slice3))

    // 切片拷贝
    slice4 := make([]int, len(slice1))
    copy(slice4, slice1)
    fmt.Println("Slice4 (copy of slice1):", slice4)

    // 切片遍历
    fmt.Println("Elements in slice1:")
    for index, value := range slice1 {
        fmt.Printf("Index %d: %d\n", index, value)
    }

    // []内指明元素个数是数组, 否则是切片
    // [...]是数组, []是切片
    array := [...]int{1, 2, 3}
    slice := []int{1, 2, 3}
    fmt.Printf("array type: %T\n", array)
    fmt.Printf("slice type: %T\n", slice)
    // 数组不能更改元素个数, 切片可以
    // 数组可以更改已有元素内容
}
```

### 2. 映射（Map）

```go
package main

import "fmt"

func main() {
    // 创建map
    ages := make(map[string]int)

    // 添加键值对
    ages["Alice"] = 30
    ages["Bob"] = 25
    ages["Charlie"] = 35

    // 初始化map
    scores := map[string]int{
        "Math":    90,
        "English": 85,
        "Science": 95,
    }

    fmt.Println("Ages:", ages)
    fmt.Println("Scores:", scores)

    // 访问map元素
    fmt.Println("Alice's age:", ages["Alice"])

    // 检查键是否存在
    age, exists := ages["Dave"]
    if exists {
        fmt.Println("Dave's age:", age)
    } else {
        fmt.Println("Dave is not in the map")
    }

    // 修改map元素
    ages["Bob"] = 26
    fmt.Println("Bob's new age:", ages["Bob"])

    // 删除map元素
    delete(scores, "English")
    fmt.Println("Scores after deleting English:", scores)

    // 遍历map
    fmt.Println("All ages:")
    for name, age := range ages {
        fmt.Printf("%s: %d\n", name, age)
    }

    // map长度
    fmt.Println("Number of people:", len(ages))
}
```

### 3. 结构体与方法

```go
package main

import "fmt"

// 结构体定义
type Person struct {
    Name string
    Age  int
    Address
}

// 嵌套结构体
type Address struct {
    Street string
    City   string
    ZipCode string
}

// 结构体方法
func (p Person) Greet() string {
    return fmt.Sprintf("Hello, my name is %s and I'm %d years old", p.Name, p.Age)
}

// 指针接收者方法（可以修改结构体内容）
// 使用非指针（值类型）接收者的方法，操作的是结构体的副本，因此无法修改原结构体实例的值
func (p *Person) Birthday() {
    p.Age++
}
// 取地址也是 &

// 带参数的方法
func (p *Person) Move(newStreet, newCity, newZip string) {
    p.Street = newStreet
    p.City = newCity
    p.ZipCode = newZip
}

func main() {
    // 创建结构体实例
    person := Person{
        Name: "Alice",
        Age: 30,
        Address: Address{
            Street: "123 Main St",
            City: "New York",
            ZipCode: "10001",
        },
    }

    fmt.Println(person.Greet())
    fmt.Printf("Lives at: %s, %s, %s\n", person.Street, person.City, person.ZipCode)

    // 调用指针接收者方法
    person.Birthday()
    fmt.Println("After birthday:", person.Greet())

    // 调用修改地址的方法
    person.Move("456 Oak Ave", "Boston", "02108")
    fmt.Printf("New address: %s, %s, %s\n", person.Street, person.City, person.ZipCode)
}

// 类型别名
// type 别名 = 原类型
type MyInt = int
```

### 4. 接口

Go 语言中使用 `interface` 作为定义接口的关键字。而接口的核心作用，简单说就是**定义一组“方法签名”（方法的声明，不含实现）**，任何结构体（或其他类型）只要**实现了这组方法**，就会“隐式地”满足这个接口。这使得接口成为一种“行为契约”，能够将不同类型中具有相同行为的方法“抽象”出来，实现多态和代码解耦。

1. 接口的基本定义（`interface` 关键字）
   接口用 `interface{}` 定义，内部包含一组方法签名（只有方法名、参数和返回值，没有函数体）。例如：

```go
// 定义一个 Shape 接口，包含两个方法签名
type Shape interface {
    Area() float64   // 计算面积的方法
    Perimeter() float64  // 计算周长的方法
}
```

2. 接口的作用：“抽象行为契约”
   接口的核心价值不是单纯“收集”某个结构体的方法，而是：

- 定义一种“行为标准”（比如“能计算面积和周长”）；
- 任何类型（结构体、甚至基本类型）只要实现了接口的所有方法，就自动属于这个接口类型；
- 从而可以用接口类型统一接收所有实现了该行为的对象，实现“多态”（同一接口，不同实现）。

示例：接口如何工作

```go
package main

import "fmt"

// 1. 定义接口（行为契约）
type Shape interface {
    Area() float64
    Perimeter() float64
}

// 2. 结构体 Rectangle 实现 Shape 接口的所有方法
type Rectangle struct {
    Width  float64
    Height float64
}

// 实现 Area 方法
func (r Rectangle) Area() float64 {
    return r.Width * r.Height
}

// 实现 Perimeter 方法
func (r Rectangle) Perimeter() float64 {
    return 2 * (r.Width + r.Height)
}

// 3. 结构体 Circle 也实现 Shape 接口的所有方法
type Circle struct {
    Radius float64
}

// 实现 Area 方法
func (c Circle) Area() float64 {
    return 3.14 * c.Radius * c.Radius
}

// 实现 Perimeter 方法
func (c Circle) Perimeter() float64 {
    return 2 * 3.14 * c.Radius
}

// 4. 函数接收 Shape 接口类型（统一处理所有实现了 Shape 的对象）
func PrintShapeInfo(s Shape) {
    fmt.Printf("面积: %.2f, 周长: %.2f\n", s.Area(), s.Perimeter())
}

func main() {
    // 不同类型的实例
    rect := Rectangle{Width: 5, Height: 3}
    circle := Circle{Radius: 4}

    // 都可以传给接收 Shape 接口的函数（多态）
    fmt.Println("矩形:")
    PrintShapeInfo(rect)  // 矩形实现了 Shape，合法
    fmt.Println("圆形:")
    PrintShapeInfo(circle) // 圆形也实现了 Shape，合法
}
```

输出：

```
矩形:
面积: 15.00, 周长: 16.00
圆形:
面积: 50.24, 周长: 25.12
```

关键特点总结：

1. **隐式实现**：Go 的接口不需要显式声明“实现了某个接口”（比如 Java 的 `implements` 关键字），只要实现了接口的所有方法，就自动属于该接口类型。
2. **行为抽象**：接口不关心类型的“是什么”，只关心类型“能做什么”（即拥有哪些方法）。
3. **多态支持**：通过接口类型，可以统一操作所有实现了该接口的对象，调用相同的方法名，执行不同的实现逻辑。

接口的作用是**定义一套行为标准，让不同类型通过实现这套标准被统一处理**，这是 Go 中实现代码灵活性和可扩展性的核心机制之一。

Go 是隐式实现：结构体只要实现了接口的所有方法，就自动属于该接口类型，无需任何 “继承” 或 “实现” 关键字，是 “松耦合” 的行为绑定。

设计理念：
C++ 的继承是 “is-a” 关系（Rectangle 是一种 Shape），可能带来 “继承层次过深”“基类修改影响所有派生类” 等问题。
Go 的接口是 “can-do” 关系（Rectangle 能做 Shape 要求的事），不关心类型本身是什么，只关心它能提供什么行为，更灵活。

扩展性：
C++ 中如果基类新增一个虚函数，所有派生类都必须修改实现，否则会变成抽象类。
Go 中如果接口新增一个方法，已有的结构体不会受影响（只是不再满足新接口），不会破坏现有代码。

---

Go 中的**空接口（`interface{}`）** 是一种特殊的接口——它不包含任何方法签名。正因为如此，**所有类型（包括基础类型、结构体、函数等）都隐式实现了空接口**。这种“万能适配”的特性，让空接口成为 Go 中实现“通用型代码”的核心工具，其价值主要体现在以下几个方面：

1. 实现“通用类型”的参数/返回值
   当函数需要接收**任意类型的参数**（或返回任意类型的值）时，空接口是唯一选择。它打破了 Go 强类型的限制，让函数具备处理不确定类型的能力。

最典型的例子是标准库中的 `fmt.Println` 函数，其参数就是空接口切片：

```go
// fmt.Println 的函数签名（简化版）
func Println(a ...interface{}) (n int, err error) {
    // 内部通过反射处理不同类型的值
}

// 因此可以接收任何类型
fmt.Println(100)       // int
fmt.Println("hello")   // string
fmt.Println([]int{1,2}) // 切片
fmt.Println(map[string]int{"a":1}) // map
```

自己实现一个通用打印函数：

```go
// 接收任意类型的参数
func printAny(v interface{}) {
    fmt.Printf("值: %v, 类型: %T\n", v, v)
}

func main() {
    printAny(3.14)      // 值: 3.14, 类型: float64
    printAny([]string{"a", "b"}) // 值: [a b], 类型: []string
}
```

2. 定义“通用数据结构”
   当需要存储**不同类型的元素**时（如动态数组、字典等），空接口可以作为容器的元素类型，实现“异构数据”的存储。

例如：

```go
// 存储多种类型的切片
func main() {
    // 空接口切片可以存放任何类型
    data := []interface{}{
        100,          // int
        "hello",      // string
        3.14,         // float64
        []int{1, 2},  // 切片
        map[string]int{"a": 1}, // map
    }

    // 遍历并打印每个元素的类型和值
    for i, v := range data {
        fmt.Printf("索引 %d: 类型=%T, 值=%v\n", i, v, v)
    }
}
```

再比如 JSON 解析中常用的 `map[string]interface{}`，用于存储解析后的任意结构：

```go
import (
    "encoding/json"
    "fmt"
)

func main() {
    jsonStr := `{"name":"Alice", "age":30, "scores":[90, 85]}`

    // 用 map[string]interface{} 接收任意 JSON 结构
    var data map[string]interface{}
    json.Unmarshal([]byte(jsonStr), &data)

    fmt.Println(data["name"])   // Alice（string 类型）
    fmt.Println(data["age"])    // 30（float64 类型，JSON 数字默认解析为 float64）
    fmt.Println(data["scores"]) // [90 85]（[]interface{} 类型）
}
```

3. 实现“类型擦除”与“动态类型处理”
   空接口可以暂时“擦除”变量的具体类型，在需要时通过**类型断言**（`v.(Type)`）或**反射**（`reflect` 包）恢复类型，实现动态处理。

例如，根据不同类型执行不同逻辑：

```go
func process(v interface{}) {
    // 类型断言：检查 v 是否为 int 类型
    if num, ok := v.(int); ok {
        fmt.Printf("整数: %d, 平方: %d\n", num, num*num)
        return
    }

    // 检查是否为 string 类型
    if str, ok := v.(string); ok {
        fmt.Printf("字符串: %s, 长度: %d\n", str, len(str))
        return
    }

    // 其他类型
    fmt.Printf("未知类型: %T\n", v)
}

func main() {
    process(10)      // 整数: 10, 平方: 100
    process("hello") // 字符串: hello, 长度: 5
    process(3.14)    // 未知类型: float64
}
```

### 4. 适配第三方库或框架的扩展性

很多库或框架会用空接口定义“扩展点”，允许用户传入任意类型的数据，再通过内部逻辑处理。例如：

- 缓存库（如 `github.com/patrickmn/go-cache`）用空接口存储任意类型的缓存值；
- Web 框架（如 Gin）用 `context.Set(key, value interface{})` 存储任意类型的上下文数据。

### 注意：空接口的“代价”

空接口虽然灵活，但也有局限性：

- **类型安全**：编译期无法检查类型，需要在运行时通过类型断言或反射确保类型正确，否则可能 panic；
- **性能开销**：空接口本质是一个“指针 + 类型信息”的结构体，操作时会有轻微的性能损耗（尤其是反射）。

因此，应避免过度使用空接口——能明确类型时尽量用具体类型，仅在需要“通用处理”时使用空接口。

### 总结

空接口的核心价值是**提供“通用类型容器”**，让 Go 在保持强类型特性的同时，具备处理“不确定类型”的能力。它是实现通用函数、异构数据结构、动态类型处理的基础，也是 Go 生态中众多库和框架实现灵活性的关键机制。

---

反射: 使用`reflect`包获取变量的类型信息、值信息以及操作结构体字段：

```go
package main

import (
	"fmt"
	"reflect"
)

// 定义一个结构体用于演示
type Person struct {
	Name string `json:"name"`
	Age  int    `json:"age"`
	Addr string `json:"addr,omitempty"`
}
// 结构体字段的标签（struct tag）, 用于 encoding/json 包的 JSON 序列化 / 反序列化配置
// 1. json:"addr"：
// 指定当结构体被序列化为 JSON 时，Addr 字段在 JSON 中对应的键名是 "addr"（而不是结构体中定义的字段名 Addr）。
// 例如，当 Addr 字段值为 "Beijing" 时，序列化后的 JSON 会包含 "addr": "Beijing"。
// 2. omitempty：
// 这是一个可选参数，含义是：如果该字段的值是「零值」（如字符串的空值 ""、int 的 0、指针的 nil 等），则在 JSON 序列化时忽略这个字段，不包含在结果中。
// 举例：
// 若 Addr 字段值为 ""（空字符串，字符串类型的零值），则序列化后不会出现 "addr": "" 这一项；
// 若 Addr 有具体值（如 "Shanghai"），则正常保留 "addr": "Shanghai"。
// 这个标签是 encoding/json 包的特定语法，用于控制 JSON 序列化 / 反序列化的行为，类似的还有 xml:"..."（用于 XML 处理）、db:"..."（用于数据库 ORM 映射）等，不同包会解析各自对应的标签。

func main() {
	// 1. 对基本类型进行反射
	num := 100
	reflectBasicType(num)

	// 2. 对结构体进行反射
	p := Person{
		Name: "Alice",
		Age:  30,
	}
	reflectStruct(p)

	// 3. 对指针类型进行反射
	reflectPointer(&p)
}

// 反射基本类型
func reflectBasicType(v interface{}) {
	fmt.Println("\n--- 基本类型反射 ---")
	// 获取类型信息
	t := reflect.TypeOf(v)
	// 获取值信息
	val := reflect.ValueOf(v)

	fmt.Printf("类型: %s\n", t.Name())       // 类型名称
	fmt.Printf("种类: %s\n", t.Kind())       // 类型种类
	fmt.Printf("值: %v\n", val.Interface()) // 获取原始值
}

// 反射结构体
func reflectStruct(v interface{}) {
	fmt.Println("\n--- 结构体反射 ---")
	t := reflect.TypeOf(v)
	val := reflect.ValueOf(v)

	// 检查是否为结构体类型
	if t.Kind() != reflect.Struct {
		fmt.Println("不是结构体类型")
		return
	}

	// 获取结构体字段数量
	fmt.Printf("结构体字段数量: %d\n", t.NumField())

	// 遍历结构体字段
	for i := 0; i < t.NumField(); i++ {
		field := t.Field(i)       // 获取字段类型信息
		fieldVal := val.Field(i)  // 获取字段值信息

		fmt.Printf("字段名: %s, 类型: %s, 标签: %s, 值: %v\n",
			field.Name,          // 字段名称
			field.Type,          // 字段类型
			field.Tag.Get("json"), // 字段标签
			fieldVal.Interface())  // 字段值
	}
}

// 反射指针类型
func reflectPointer(v interface{}) {
	fmt.Println("\n--- 指针类型反射 ---")
	t := reflect.TypeOf(v)
	val := reflect.ValueOf(v)

	fmt.Printf("原始类型: %s\n", t.Kind()) // 指针类型的种类是ptr

	// 对指针进行解引用（获取指针指向的元素）
	if t.Kind() == reflect.Ptr {
		t = t.Elem()  // 获取指针指向的类型
		val = val.Elem() // 获取指针指向的值
		fmt.Printf("解引用后类型: %s\n", t.Kind())
		fmt.Printf("解引用后值: %v\n", val.Interface())
	}
}
// reflect.type：表示 “type类型” 这一种类, 实际怎么定义的 Struct/是什么类型的 Ptr 不重要
```

1. **基本类型反射**：通过`reflect.TypeOf`获取变量类型，`reflect.ValueOf`获取变量值，以及如何还原原始值。

2. **结构体反射**：

   - 检查是否为结构体类型
   - 获取结构体字段数量
   - 遍历字段并获取字段名、类型、标签和值

3. **指针类型反射**：
   - 处理指针类型的反射
   - 使用`Elem()`方法对指针进行解引用
   - 获取指针指向的实际类型和值

运行程序后，输出结果会展示各种类型在反射中的信息。反射在 Go 中常用于需要动态处理类型的场景，比如 JSON 序列化/反序列化、ORM 框架等，但应谨慎使用，因为它会降低性能并使代码更复杂。

---

```go
package main

import "fmt"

// 定义接口
type Shape interface {
    Area() float64
    Perimeter() float64
}

// 实现接口的结构体
type Rectangle struct {
    Width  float64
    Height float64
}

func (r Rectangle) Area() float64 {
    return r.Width * r.Height
}

func (r Rectangle) Perimeter() float64 {
    return 2 * (r.Width + r.Height)
}

// 另一个实现接口的结构体
type Circle struct {
    Radius float64
}

func (c Circle) Area() float64 {
    return 3.14159 * c.Radius * c.Radius
}

func (c Circle) Perimeter() float64 {
    return 2 * 3.14159 * c.Radius
}

// 使用接口的函数
func PrintShapeInfo(s Shape) {
    fmt.Printf("Area: %.2f\n", s.Area())
    fmt.Printf("Perimeter: %.2f\n", s.Perimeter())
}

// 空接口示例
func PrintValue(v interface{}) {
    fmt.Printf("Value: %v, Type: %T\n", v, v)
}

func main() {
    rect := Rectangle{Width: 5, Height: 3}
    circle := Circle{Radius: 4}

    fmt.Println("Rectangle info:")
    PrintShapeInfo(rect)

    fmt.Println("\nCircle info:")
    PrintShapeInfo(circle)

    // 类型断言
    var s Shape = Circle{Radius: 2.5}
    if circle, ok := s.(Circle); ok {
        fmt.Printf("\nIt's a circle with radius: %.2f\n", circle.Radius)
    } else if rect, ok := s.(Rectangle); ok {
        fmt.Printf("It's a rectangle with width: %.2f, height: %.2f\n", rect.Width, rect.Height)
    }

    // 空接口使用
    fmt.Println("\nEmpty interface examples:")
    PrintValue(42)
    PrintValue("Hello")
    PrintValue(3.14)
    PrintValue(true)
}
```

## 三、Go 语言高级特性

### 1. 并发编程（Goroutines 和 Channels）

goroutine（轻量级线程）和 channel（通道，用于 goroutine 间通信）

```go
package main

import (
    "fmt"
    "time"
)

// Goroutine示例 -- goroutine 的基本使用（并发执行函数）
func printNumbers() {
    // 打印数字 1-5
    for i := 1; i <= 5; i++ {
        time.Sleep(200 * time.Millisecond)// 休眠200ms，模拟耗时操作
        fmt.Printf("%d ", i)
    }
}

func printLetters() {
    // 打印字母 a-e
    for i := 'a'; i <= 'e'; i++ {
        time.Sleep(300 * time.Millisecond)// 休眠300ms，模拟耗时操作
        fmt.Printf("%c ", i)
    }
}

// Channel示例 -- channel 的基本用法（goroutine 间传递数据）
// Channel 是 goroutine 间的通信机制，用于传递数据，避免共享内存导致的竞态问题。
// 计算切片和，并通过channel发送结果
func sum(numbers []int, resultChan chan int) {
    sum := 0
    for _, n := range numbers {
        sum += n
    }
    resultChan <- sum  // 将结果发送到channel
}
// 声明一个名为 resultChan 的变量，其类型是 “可以传递 int 类型数据的 channel
// 需要注意的是：这种声明方式下，resultChan 是一个双向 channel—— 它既可以用于写入 int 类型的数据（通过 resultChan <- 100 语法），也可以用于读取 int 类型的数据（通过 num := <-resultChan 语法）。

// 带缓冲的channel
func bufferedChannelExample() {
    ch := make(chan string, 3)  // 创建缓冲大小为3的channel（可暂存3个值）
    
    ch <- "first"  // 发送到缓冲区，不阻塞（缓冲区未满）
    ch <- "second"
    ch <- "third"
    // ch <- "fourth"  // 若打开此行，会阻塞（缓冲区已满，需等待接收）
    
    fmt.Println(<-ch)  // 接收"first"，缓冲区剩余2个
    fmt.Println(<-ch)  // 接收"second"，缓冲区剩余1个
    fmt.Println(<-ch)  // 接收"third"，缓冲区为空
}
// 带缓冲的 channel 允许在阻塞前存储多个值（缓冲大小由 make(chan T, n) 的 n 指定）：
// 发送操作：缓冲区未满时不阻塞，满了才阻塞。
// 接收操作：缓冲区非空时不阻塞，空了才阻塞。
// 用途：适合处理生产速度和消费速度不匹配的场景（如消息队列）

// 关闭channel和range遍历
func channelRangeExample() {
    ch := make(chan int)
    
    // 启动goroutine发送数据到channel
    go func() {
        for i := 1; i <= 5; i++ {
            ch <- i  // 发送1-5
        }
        close(ch)  // 关闭channel（告诉接收方：不会再有数据发送了）
    }()
    
    // 用range遍历channel，直到channel被关闭
    for num := range ch {
        fmt.Printf("%d ", num)  // 输出：1 2 3 4 5
    }
}
// 关闭 channel：通过 close(ch) 关闭，关闭后不能再发送数据，但可以继续接收剩余数据。
// range 遍历 channel：会持续接收数据，直到 channel 被关闭，自动退出循环（避免了手动判断是否还有数据的麻烦）。
// 注意：若 channel 未关闭，range 遍历会一直阻塞等待新数据，可能导致死锁。

func main() {
    // Goroutine示例
    fmt.Println("Goroutine example:")
    // Goroutine 是 Go 的轻量级线程，通过 go 函数名() 启动，与主线程（main 函数）并发执行。
    go printNumbers()  // 启动goroutine执行printNumbers
    go printLetters()  // 启动goroutine执行printLetters
    time.Sleep(2 * time.Second)  // 主线程休眠2秒，等待两个goroutine执行完毕
    //  main 函数是主线程，若主线程提前结束，所有子 goroutine 会被强制终止。这里休眠是为了让子 goroutine 有时间执行完
    fmt.Println("\n")
    // 执行效果：数字和字母会交替打印（因两个 goroutine 并发执行，且休眠时间不同），例如：1 a 2 3 b 4 c 5 d e（顺序可能略有差异，取决于调度）

    // Channel示例
    fmt.Println("Channel example:")
    numbers := []int{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}

    // 创建两个channel
    ch1 := make(chan int) // 创建无缓冲channel（只能存1个值，发送和接收需同步）
    ch2 := make(chan int)

    // 启动两个goroutine
    go sum(numbers[:5], ch1)  // 计算前5个数字的和，结果发送到ch1
    go sum(numbers[5:], ch2)  // 计算后5个数字的和，结果发送到ch2

    // 从channel接收结果
    sum1, sum2 := <-ch1, <-ch2  // 从channel接收结果（若channel无数据，会阻塞等待）
    fmt.Printf("Sum of first 5: %d\n", sum1)  // 15（1+2+3+4+5）
    fmt.Printf("Sum of last 5: %d\n", sum2)   // 40（6+7+8+9+10）
    fmt.Printf("Total: %d\n", sum1+sum2)      // 55
    // channel 发送（ch <- val）和接收（val := <-ch）操作默认是阻塞的：发送方会等待接收方准备好，接收方会等待发送方发送数据，实现了 goroutine 间的同步。
    // 这里通过两个 channel 分别接收两个 goroutine 的计算结果，避免了共享变量的竞争。

    // 带缓冲的channel
    fmt.Println("Buffered channel example:")
    bufferedChannelExample()
    fmt.Println()

    // Channel range遍历
    fmt.Println("Channel range example:")
    channelRangeExample()
}
```

* 补充：channel 的 “方向”
Go 中可以通过语法限制 channel 的方向（只读或只写），例如：

writeChan chan<- int：只能写入 int 的 channel（“只写 channel”），不能从中读取。
readChan <-chan int：只能读取 int 的 channel（“只读 channel”），不能向其写入。

而上面的 resultChan chan int 没有限制方向，是双向的，这也是最常用的声明方式（除非需要明确限制 channel 的使用场景，比如函数参数中限制只能写入或只能读取）。

在示例代码的 sum 函数中：
```go
func sum(numbers []int, resultChan chan int) {
    // ... 计算逻辑 ...
    resultChan <- sum  // 向 channel 写入 int 类型的结果
}
```
这里 resultChan 被用作 “输出通道”（写入结果），但这是函数的使用方式，而非 channel 本身的类型限制 —— 从类型上看，它仍然可以被读取（就像 main 函数中通过 <-ch1 读取结果一样）

--- 

`for-range` 是一种简洁的循环语法，主要用于**遍历数组、切片、字符串、映射（map）和通道（channel）**。它会自动迭代集合中的元素，无需手动控制索引，语法更简洁。

### 基本语法
```go
for 索引/键, 值 := range 集合 {
    // 循环体（使用索引/键和值）
}
```
- 对于不同类型的“集合”，`for-range` 返回的“索引/键”和“值”含义不同（见下文分类型示例）。  
- 若不需要“索引/键”或“值”，可使用空白标识符 `_` 忽略（例如 `for _, v := range 集合`）。  

### 分类型用法示例

#### 1. 遍历数组/切片（最常用）
数组和切片的 `for-range` 返回 **`索引`** 和 **`对应元素的值`**。

```go
package main

import "fmt"

func main() {
    // 切片（数组用法相同）
    fruits := []string{"apple", "banana", "cherry"}
    
    // 完整形式：获取索引和值
    for i, fruit := range fruits {
        fmt.Printf("索引：%d，值：%s\n", i, fruit)
    }
    
    // 简化：只需要值（忽略索引）
    fmt.Println("\n只打印值：")
    for _, fruit := range fruits {
        fmt.Println(fruit)
    }
}
```
**输出**：
```
索引：0，值：apple
索引：1，值：banana
索引：2，值：cherry

只打印值：
apple
banana
cherry
```


#### 2. 遍历字符串
字符串的 `for-range` 返回 **`字符的起始索引`** 和 **`字符的 Unicode 码点（rune 类型）`**，会自动处理多字节字符（如中文、 emoji）。

```go
package main

import "fmt"

func main() {
    str := "Go语言😊"
    
    // 遍历字符串（处理多字节字符）
    for i, c := range str {
        fmt.Printf("索引：%d，字符：%c（Unicode值：%U）\n", i, c, c)
    }
}
```
**输出**：
```
索引：0，字符：G（Unicode值：U+0047）
索引：1，字符：o（Unicode值：U+006F）
索引：2，字符：语（Unicode值：U+8BED）
索引：5，字符：言（Unicode值：U+8A00）
索引：8，字符：😊（Unicode值：U+1F60A）
```
- 注意：中文、emoji 等是多字节字符，索引可能不连续（如“语”占 3 个字节，索引从 2 开始）。


#### 3. 遍历映射（map）
map 的 `for-range` 返回 **`键（key）`** 和 **`值（value）`**，且遍历顺序是**随机的**（每次运行可能不同）。

```go
package main

import "fmt"

func main() {
    scores := map[string]int{
        "Alice": 90,
        "Bob":   85,
        "Charlie": 95,
    }
    
    // 遍历map的键和值
    for name, score := range scores {
        fmt.Printf("%s 的分数：%d\n", name, score)
    }
    
    // 只遍历键（忽略值）
    fmt.Println("\n所有名字：")
    for name := range scores {
        fmt.Println(name)
    }
}
```
**可能的输出**（顺序随机）：
```
Alice 的分数：90
Bob 的分数：85
Charlie 的分数：95

所有名字：
Alice
Charlie
Bob
```


#### 4. 遍历通道（channel）
channel 的 `for-range` 会**持续接收通道中的数据**，直到通道被关闭，此时循环自动退出。

```go
package main

import "fmt"

func main() {
    ch := make(chan int, 3)
    ch <- 10
    ch <- 20
    ch <- 30
    close(ch) // 关闭通道（必须关闭，否则for-range会一直阻塞）
    
    // 遍历通道中的数据
    for num := range ch {
        fmt.Println("收到：", num)
    }
}
```
**输出**：
```
收到： 10
收到： 20
收到： 30
```


### 注意事项
1. **值是副本**：  
   遍历数组/切片/字符串时，`for-range` 中的“值”是元素的**副本**，修改它不会影响原集合：
   ```go
   nums := []int{1, 2, 3}
   for _, v := range nums {
       v = 100 // 原切片nums不会被修改
   }
   fmt.Println(nums) // 输出：[1 2 3]
   ```
   若需修改原元素，需通过索引操作（如 `nums[i] = 100`）。

2. **map 遍历的随机性**：  
   Go 故意设计为 map 遍历顺序不固定，避免开发者依赖顺序逻辑（如需顺序，需手动排序键）。

3. **channel 必须关闭**：  
   若遍历未关闭的 channel，且 channel 中无数据，`for-range` 会一直阻塞，导致死锁。

4. **避免修改正在遍历的集合**：  
   遍历中修改集合（如给 map 新增/删除元素）可能导致部分元素被跳过或重复遍历（不推荐）。


`for-range` 是 Go 中最常用的遍历方式，尤其适合不需要手动控制索引的场景，代码更简洁易读。根据不同的集合类型，注意其返回值的含义和特殊特性即可。

--- 

### 2. 错误处理

```go
package main

import (
    "errors"
    "fmt"
    "os"
)

// 自定义错误：除法运算，除数为0时返回错误
func divide(a, b float64) (float64, error) {
    if b == 0 {
        return 0, errors.New("cannot divide by zero") // 创建简单错误
    }
    return a / b, nil // 无错误时返回nil
}
// Go 错误处理的基本模式：函数通过返回值返回运算结果和 error 类型（error 是一个接口，需实现 Error() string 方法）。
// 当操作成功时，error 返回 nil；当操作失败时，返回具体的错误信息（通过 errors.New 创建简单错误）

// 更详细的自定义错误
// Go 允许定义自定义错误类型（需实现 error 接口的 Error() 方法），用于携带更详细的错误信息（如错误字段、具体原因等）。
// 自定义错误类型：包含错误字段和详细信息
type ValidationError struct {
    Field   string // 错误关联的字段
    Message string // 错误描述
}

// 实现error接口的Error()方法，使ValidationError成为error类型
func (e *ValidationError) Error() string {
    return fmt.Sprintf("Validation error for field %s: %s", e.Field, e.Message)
}

// 验证用户信息的函数，返回自定义错误
func validateUser(name string, age int) error {
    if name == "" {
        // 返回自定义错误实例，包含具体字段和信息
        return &ValidationError{Field: "name", Message: "cannot be empty"}
    }
    if age < 0 || age > 150 {
        return &ValidationError{Field: "age", Message: "must be between 0 and 150"}
        // 返回指针 --> 满足 error 接口的实现要求
        // 接口（如 error）的实现遵循 “接收者类型匹配” 原则
        // Error() 方法的接收者是指针 *ValidationError
    }
    return nil // 验证通过，返回nil
}
// 自定义错误的优势：相比简单字符串错误，能携带更多上下文信息（如这里的 Field 字段），方便调用方更精准地处理错误。
// 如果将 ValidationError 的 Error() 方法接收者从指针类型改为结构体值类型，那么 ValidationError（值类型）会直接实现 error 接口，此时函数既可以返回 ValidationError（结构体值），也可以返回 &ValidationError（结构体指针）—— 并非 “只能返回结构体”，但返回值类型的选择会更灵活（本质是两种类型都实现了 error 接口）
// 关键原理：方法接收者与接口实现的关系
// 方法接收者的类型会直接决定 “哪些类型实现了接口”：
// 若方法接收者是 值类型（如 func (e ValidationError) Error() string）：
// 则 ValidationError（值类型） 和 &ValidationError（指针类型） 都会实现该接口。
// 原因：指针类型可以 “自动解引用” 调用值接收者的方法（比如 (&e).Error() 会自动转为 e.Error()），因此两种类型都符合接口要求。
// 若方法接收者是 指针类型（如 func (e *ValidationError) Error() string）：
// 则只有 &ValidationError（指针类型） 实现该接口。
// 原因：值类型无法 “自动取地址” 调用指针接收者的方法（比如 e.Error() 不能自动转为 (&e).Error()），因此值类型不符合接口要求

// 错误包装与检查
// 在复杂场景中，错误可能经过多层传递（如函数 A 调用 B，B 调用 C，C 产生错误），此时需要包装错误以保留完整的错误链，方便追溯根源。
// 错误包装示例：读取文件并包装可能的错误
func readFile(filename string) (string, error) {
    data, err := os.ReadFile(filename) // 调用标准库函数，可能返回错误（如文件不存在）
    if err != nil {
        // 使用fmt.Errorf和%w包装原始错误，保留错误链
        return "", fmt.Errorf("failed to read file: %w", err)
    }
    return string(data), nil
}
// 错误包装：通过 fmt.Errorf 配合 %w 格式动词，将原始错误（如 os.ReadFile 返回的错误）包装到新错误中，形成错误链（类似异常的 “堆栈信息”）。
// 当使用 fmt.Errorf 配合 %w 包装错误时，保存的是结构体实例本身（而非仅保存 Error() 方法的返回字符串）
// %w 的核心作用是构建错误链，它要求被包装的对象必须实现 error 接口（即有 Error() 方法）。包装时会保留原始错误的完整类型信息和值，而不是仅提取其字符串描述

func main() {
    // 基本错误处理
    result, err := divide(10, 0)
    if err != nil { // 检查错误是否存在（非nil）
        fmt.Println("Error:", err) // 处理错误：打印错误信息
        // 这里除数为 0，会输出 Error: cannot divide by zero。
    } else {
        fmt.Println("Result:", result) // 无错误时处理结果
    }

    // 自定义错误处理 -- （通过类型断言判断错误类型）
    err = validateUser("", 25) // 验证失败（name为空）
    if err != nil {
        // 类型断言：判断err是否为*ValidationError类型
        if valErr, ok := err.(*ValidationError); ok {
            fmt.Println("Validation Error:", valErr) // 打印错误（自动调用Error()方法）
            fmt.Println("Field:", valErr.Field) // 访问自定义错误的字段
        } else {
            fmt.Println("Unexpected error:", err) // 处理其他类型错误
        }
    }
    // 这里通过 err.(*ValidationError) 将通用 error 转换为具体的自定义错误类型，从而获取更详细的 Field 信息

    // 错误包装和检查
    _, err = readFile("nonexistent.txt") // 读取不存在的文件，产生错误
    if err != nil {
        fmt.Println("Error reading file:", err) // 打印包装后的错误信息
        // 检查错误链中是否包含特定错误（os.ErrNotExist：文件不存在）
        if errors.Is(err, os.ErrNotExist) {
            fmt.Println("File does not exist") // 针对性处理
        }
    }
}
```
Go 错误处理核心特点总结
* 非异常机制：Go 不使用 try/catch 异常机制，而是通过返回 error 类型值显式处理错误，更强调 “错误是流程的一部分”。
* 自定义错误：通过实现 error 接口（Error() string 方法），可以定义携带丰富信息的错误类型，方便精细处理。
* 错误包装：使用 %w 包装错误，保留错误链，便于调试和追溯根源；通过 errors.Is 检查错误链中的特定错误。
* 类型断言：对于自定义错误类型，使用类型断言（err.(具体类型)）可以转换为具体错误，获取额外信息。

```go
// 有一个实现了 error 接口的自定义结构体
type MyError struct {
    Code    int
    Message string
}

func (e *MyError) Error() string {
    return fmt.Sprintf("code=%d: %s", e.Code, e.Message)
}

//用 %w 包装这个结构体时
func main() {
    // 创建自定义错误结构体实例
    originalErr := &MyError{Code: 500, Message: "internal error"}
    
    // 用%w包装错误
    wrappedErr := fmt.Errorf("wrapper: %w", originalErr)
    
    // 检查包装的错误类型（证明保存的是原始结构体）
    var err *MyError
    if errors.As(wrappedErr, &err) {
        fmt.Println("提取原始错误结构体：")
        fmt.Printf("Code: %d\n", err.Code)      // 可以访问原始结构体的字段
        fmt.Printf("Message: %s\n", err.Message)
    }
}

// 输出
// 提取原始错误结构体：
// Code: 500
// Message: internal error
```
保存的是原始结构体实例：%w 会将原始错误（即使是结构体类型）完整保存到错误链中，而非仅保存 Error() 方法返回的字符串。
支持类型提取：通过 errors.As 可以从包装后的错误中提取原始结构体，访问其字段（如示例中的 Code 和 Message），这依赖于原始结构体被完整保存。
与 %s 的区别：如果用 %s 格式化错误（如 fmt.Errorf("wrapper: %s", originalErr)），则只会保存 originalErr.Error() 的字符串结果，丢失原始结构体的类型和字段信息，后续无法通过 errors.As 提取原始结构体。
因此，%w 的设计目的是保留错误的完整类型信息和上下文，以便后续追溯错误根源或提取详细信息，这也是它在错误链处理中不可替代的作用

### 3. 包与模块

```go
// 文件名: mathutil/add.go
package mathutil

// Add 函数将两个整数相加并返回结果
// 首字母大写表示导出函数（公共函数）
func Add(a, b int) int {
    return a + b
}

// multiply 函数将两个整数相乘并返回结果
// 首字母小写表示未导出函数（私有函数）
func multiply(a, b int) int {
    return a * b
}

// 文件名: mathutil/calc.go
package mathutil

// Calculate 演示如何在同一个包中调用其他函数
func Calculate(a, b int) (int, int) {
    sum := Add(a, b)
    product := multiply(a, b) // 可以调用同一个包中的私有函数
    return sum, product
}
```

使用包的示例：

```go
// 文件名: main.go
package main

import (
    "fmt"
    "your-module-path/mathutil" // 替换为实际的模块路径
)

func main() {
    sum := mathutil.Add(3, 5)
    fmt.Println("3 + 5 =", sum)

    s, p := mathutil.Calculate(4, 6)
    fmt.Println("4 + 6 =", s)
    fmt.Println("4 * 6 =", p)
}
```

模块操作命令：

```bash
# 初始化模块
go mod init your-module-path
# 作用：初始化一个新的 Go 模块，生成 go.mod 文件（模块的核心配置文件）。
# your-module-path 是你的模块唯一标识，通常是代码仓库的 URL（如 github.com/yourname/yourproject），用于其他项目引用该模块时定位它。
# 执行后会在当前目录生成 go.mod 文件，内容大致如下（记录模块路径和 Go 版本）：
# module github.com/yourname/yourproject  // 模块路径
# go 1.21  // 项目使用的 Go 版本
# 场景：新建项目时第一步执行，用于将项目声明为一个可管理的模块

# 添加依赖
go get example.com/some/package
# 作用：添加、更新或指定依赖包的版本，并更新 go.mod 和 go.sum 文件。
# example.com/some/package 是依赖包的路径（通常是代码仓库地址）。
# 可以通过 @版本号 指定具体版本，例如：
# go get example.com/some/package@v1.2.3  # 使用 v1.2.3 版本
# go get example.com/some/package@latest  # 使用最新版本
# 执行后，go.mod 会新增该依赖的记录（含版本），go.sum 会添加该依赖的校验和（确保依赖未被篡改）。
# 场景：需要引入新的第三方库，或升级 / 降级已有依赖的版本时使用。

# 下载依赖
go mod download
# 作用：下载 go.mod 中声明的所有依赖到本地缓存（默认路径：$GOPATH/pkg/mod），但不会修改 go.mod 或 go.sum。
# 依赖被缓存后，其他项目可以复用，避免重复下载。
# 通常用于预先下载依赖（例如在构建环境中，确保依赖已存在，加快后续构建速度）。
# 场景：克隆一个已有模块的代码后，执行该命令下载项目所需的所有依赖（替代旧版本的 go get ./...）

# 整理依赖
go mod tidy
# 作用：自动整理依赖，确保 go.mod 中记录的依赖与代码中实际使用的依赖一致。
# 添加：如果代码中使用了 go.mod 中未声明的依赖，会自动添加（默认取最新版本）。
# 移除：如果 go.mod 中声明了代码中不再使用的依赖，会自动删除。
# 同时更新 go.sum 以匹配整理后的依赖。
# 场景：开发中频繁使用（例如新增 / 删除依赖后），保持 go.mod 简洁，避免冗余依赖。
```

## 四、Go 语言标准库

### 1. 文件操作

```go
package main

import (
    "bufio"    // 提供缓冲I/O功能，用于逐行读取
    "fmt"      // 提供输入输出格式化功能
    "io/ioutil"// 提供文件I/O的工具函数（如读取整个文件）
    "os"       // 提供与操作系统交互的功能（如文件操作）
)

func main() {
    // 创建文件并初始化关闭操作
    file, err := os.Create("example.txt")  // 创建文件（若存在则截断为空）
    if err != nil {  // 处理可能的错误（如权限不足）
        fmt.Println("Error creating file:", err)
        return
    }
    defer file.Close()  // 延迟关闭文件（函数结束时自动执行，确保资源释放）

    // 写入内容到文件
    _, err = file.WriteString("Hello, Go File Handling!\n")  // 写入第一行内容
    if err != nil {  // 处理写入错误
        fmt.Println("Error writing to file:", err)
        return
    }
    // WriteString返回两个值：写入的字节数（此处用_忽略）和错误信息
    // 字符串末尾的\n表示换行

    // 追加内容
    _, err = file.WriteString("This is another line.\n") // 继续写入第二行
    if err != nil {
        fmt.Println("Error appending to file:", err)
        return
    }
    // 由于文件未关闭（defer在函数结束时才执行），再次调用WriteString会直接追加内容到文件末尾

    // 读取整个文件
    data, err := ioutil.ReadFile("example.txt")  // 一次性读取整个文件到字节切片
    if err != nil {
        fmt.Println("Error reading file:", err)
        return
    }
    fmt.Println("File content:")
    fmt.Println(string(data))  // 将字节切片转换为字符串并打印
    // ioutil.ReadFile会自动打开和关闭文件，无需手动处理文件句柄
    // 适合读取小型文件，大型文件可能占用过多内存

    // 逐行读取文件内容
    file, err = os.Open("example.txt")  // 重新打开文件（以只读方式）
    if err != nil {
        fmt.Println("Error opening file:", err)
        return
    }
    defer file.Close()  // 再次延迟关闭（覆盖之前的file变量，确保本次打开的文件被关闭）

    scanner := bufio.NewScanner(file)  // 创建扫描器，用于逐行读取
    lineNumber := 1
    fmt.Println("\nReading line by line:")
    for scanner.Scan() {  // 循环扫描每行内容
        fmt.Printf("Line %d: %s\n", lineNumber, scanner.Text())  // 打印行号和内容
        lineNumber++
    }

    if err := scanner.Err(); err != nil {  // 检查扫描过程中的错误（如读取失败）
        fmt.Println("Error scanning file:", err)
    }

    // 删除文件
    err = os.Remove("example.txt")  // 删除文件
    if err != nil {
        fmt.Println("Error deleting file:", err)
        return
    }
    fmt.Println("\nFile deleted successfully")
    // 若文件不存在或无权限，会返回错误
}
```

os.Open 函数的源码实现本质上是对 os.OpenFile 的封装，它固定使用了只读标志：

```go
// 源码简化版
func Open(name string) (*File, error) {
    return OpenFile(name, O_RDONLY, 0) // 固定使用 O_RDONLY 标志
}
```

其中 O_RDONLY 是 os 包定义的常量，表示只读模式（Read-Only）。
这样设计的原因是：
os.Open 的定位是 “打开一个已存在的文件用于读取”，是最常用的文件打开场景之一。通过固定为只读模式，可以避免误操作导致文件内容被修改，是一种安全设计

在 Go 语言中，`os.Open` 之所以默认是**只读模式**，是由其底层实现决定的。要理解这一点，以及如何区分不同的文件打开模式，需要从 Go 对文件操作的设计逻辑说起：

Go 中通过 `os.OpenFile` 函数的第二个参数（`flag`）来指定文件打开模式，常见模式如下：

| 模式标志          | 含义                                  | 典型场景                     |
|-------------------|---------------------------------------|------------------------------|
| `os.O_RDONLY`     | 只读模式（默认，不可与写模式同时使用） | 读取配置文件                 |
| `os.O_WRONLY`     | 只写模式（不可与读模式同时使用）      | 日志写入（只追加不读取）     |
| `os.O_RDWR`       | 读写模式                              | 需要同时读写的场景（如编辑） |
| `os.O_CREATE`     | 若文件不存在则创建                    | 新建文件（如日志文件）       |
| `os.O_TRUNC`      | 打开文件时清空原有内容                | 覆盖写入（如重新生成文件）   |
| `os.O_APPEND`     | 写入时追加到文件末尾                  | 日志追加、累加数据           |


模式标志可以通过**位或操作符 `|`** 组合，实现更复杂的行为。例如：
```go
// 示例1：创建文件（若不存在），以读写模式打开，且清空原有内容
file, err := os.OpenFile("test.txt", os.O_CREATE|os.O_RDWR|os.O_TRUNC, 0644)

// 示例2：打开已存在的文件，以只写模式追加内容
file, err := os.OpenFile("log.txt", os.O_WRONLY|os.O_APPEND, 0644)
```
注意：`os.O_RDONLY`（只读）、`os.O_WRONLY`（只写）、`os.O_RDWR`（读写）是**互斥的**，只能选其一；其他标志（如 `O_CREATE`、`O_APPEND`）可以与之组合。

除了 `os.Open`，Go 还提供了一些简化函数，它们本质上是 `os.OpenFile` 加固定模式的封装：
- `os.Create(name string)`：等价于 `os.OpenFile(name, os.O_CREATE|os.O_WRONLY|os.O_TRUNC, 0666)`，即“创建文件（若不存在），只写模式，清空原有内容”。
- `os.Open(name string)`：等价于 `os.OpenFile(name, os.O_RDONLY, 0)`，即“只读模式打开已有文件”。

--- 

在 Go 语言中，`defer` 是一个用于**延迟执行函数调用**的关键字，它的核心作用是确保某些操作（通常是资源清理、释放）在函数退出前**一定会执行**，无论函数是正常返回、还是因错误提前退出。


### 一、基本用法
`defer` 后面必须跟一个**函数调用**（可以是普通函数、方法，甚至匿名函数），这个调用会被推迟到包含它的函数（以下简称“主函数”）即将返回时执行。

```go
package main

import "fmt"

func main() {
    fmt.Println("start")
    defer fmt.Println("deferred") // 延迟执行
    fmt.Println("end")
}
```

输出结果：
```
start
end
deferred  // 主函数即将返回时执行
```


### 二、核心特性
#### 1. 执行时机：主函数退出前
`defer` 语句定义的函数调用，会在主函数的**所有代码执行完毕后、真正返回前**执行。无论主函数是正常返回、还是因 `return` 提前退出、甚至是发生 panic，`defer` 都会执行。

```go
func test() {
    defer fmt.Println("清理资源") // 一定会执行
    
    if true {
        return // 提前返回，但defer仍会执行
    }
    
    fmt.Println("这里不会执行")
}

func main() {
    test()
}
```

输出：
```
清理资源
```


#### 2. 参数求值时机：定义时求值
`defer` 语句中函数的**参数**，会在 `defer` 语句定义时就被计算，而不是在执行时。

```go
func main() {
    i := 10
    defer fmt.Println("defer:", i) // 参数i在此时求值（值为10）
    
    i = 20
    fmt.Println("main:", i)
}
```

输出：
```
main: 20
defer: 10  // 用的是defer定义时的i值
```


#### 3. 多个defer的执行顺序：后进先出（LIFO）
如果一个函数中有多个 `defer` 语句，它们会按照**“定义顺序的反序”**执行（类似栈的“后进先出”）。

```go
func main() {
    defer fmt.Println("1")
    defer fmt.Println("2")
    defer fmt.Println("3")
}
```

输出：
```
3  // 最后定义的最先执行
2
1
```


#### 4. 与返回值的交互：可修改命名返回值
如果主函数有**命名返回值**，`defer` 函数可以修改这个返回值（因为 `return` 的执行过程分两步：先赋值给返回变量，再执行 `defer`，最后返回）。

```go
// 命名返回值：result
func calculate() (result int) {
    defer func() {
        result += 10 // 修改返回值
    }()
    
    return 5 // 实际执行：先将5赋值给result，再执行defer（result变为15），最后返回15
}

func main() {
    fmt.Println(calculate()) // 输出15
}
```

执行步骤拆解：
1. 执行 `return 5` → 将 5 赋值给命名返回值 `result`（此时 `result=5`）；
2. 执行 `defer` 函数 → `result += 10`（此时 `result=15`）；
3. 主函数返回 `result` → 最终返回 15。


### 三、典型应用场景
`defer` 最核心的价值是**“确保资源释放”**，避免因代码路径复杂（如多个 `return`）导致的资源泄漏。

#### 1. 关闭文件
```go
func readFile() {
    file, err := os.Open("test.txt")
    if err != nil {
        return
    }
    defer file.Close() // 确保文件最终会关闭，无论后续是否出错
    
    // 读取文件操作...
}
```


#### 2. 释放锁
```go
import "sync"

var mu sync.Mutex

func safeAccess() {
    mu.Lock()
    defer mu.Unlock() // 确保锁最终会释放，避免死锁
    
    // 临界区操作...
}
```


#### 3. 释放网络连接
```go
func fetchData() {
    conn, err := net.Dial("tcp", "example.com:80")
    if err != nil {
        return
    }
    defer conn.Close() // 确保连接最终会关闭
    
    // 网络操作...
}
```


#### 4. 捕获panic（结合recover）
`defer` 可以配合 `recover()` 捕获 `panic`，避免程序崩溃（类似其他语言的 try-catch）。

```go
func riskyOperation() {
    defer func() {
        if err := recover(); err != nil { // 捕获panic
            fmt.Println("捕获到错误:", err)
        }
    }()
    
    panic("发生严重错误！") // 触发panic
}

func main() {
    riskyOperation()
    fmt.Println("程序继续执行") // 不会崩溃，会执行到这里
}
```

输出：
```
捕获到错误: 发生严重错误！
程序继续执行
```


### 四、注意事项
1. **循环中使用defer需谨慎**  
   循环中的 `defer` 会积累到函数结束才执行，可能导致资源长时间不释放（如文件句柄、连接）。解决方法：将循环体封装到匿名函数中，让 `defer` 在每次迭代后执行。

   ```go
   // 错误示例：1000个文件句柄会等到函数结束才关闭
   for i := 0; i < 1000; i++ {
       file, _ := os.Open(fmt.Sprintf("file%d.txt", i))
       defer file.Close() // 危险：积累1000个未关闭的文件
   }
   
   // 正确示例：每次迭代后关闭
   for i := 0; i < 1000; i++ {
       func() {
           file, _ := os.Open(fmt.Sprintf("file%d.txt", i))
           defer file.Close() // 每次迭代结束后关闭
           // 使用文件...
       }()
   }
   ```


2. **避免defer执行耗时操作**  
   `defer` 会延迟到函数退出时执行，如果操作耗时（如大量计算、网络请求），会阻塞函数返回，影响性能。


3. **匿名函数的变量捕获**  
   如果 `defer` 后面是匿名函数，且引用了主函数的变量，需注意变量是“引用传递”（执行时取值），而非参数的“值传递”（定义时取值）。

   ```go
   func main() {
       i := 10
       defer func() {
           fmt.Println("defer:", i) // 引用主函数的i，执行时取值
       }()
       
       i = 20
   }
   ```

   输出：
   ```
   defer: 20  // 匿名函数引用的是变量i本身，执行时i已变为20
   ```


### 总结
`defer` 是 Go 语言中用于**延迟执行**的核心机制，其设计初衷是简化资源管理：  
- 确保清理操作（关闭文件、释放锁等）**一定会执行**，无论函数如何退出；  
- 遵循“定义时求值参数、主函数退出前执行、多个defer后进先出”的规则；  
- 配合命名返回值可修改返回结果，配合 `recover()` 可捕获 `panic`，是 Go 代码健壮性的重要保障。

--- 

Go 语言的异常处理机制与 Java、Python 等语言的 `try-catch` 机制有显著不同，它没有专门的 `try`、`catch` 关键字，而是通过 **`panic`（触发异常）**、**`recover`（捕获异常）** 和 **`defer`（延迟执行）** 三者配合，实现对程序运行中“非预期错误”的处理。


### 一、错误（Error）与异常（Panic）的区别
在 Go 中，“错误”和“异常”是两个不同的概念，需要先明确区分：

| 类型       | 含义                                  | 处理方式                  | 典型场景                     |
|------------|---------------------------------------|---------------------------|------------------------------|
| 错误（Error） | 可预期的、轻微的问题（如文件不存在）  | 通过 `error` 类型返回处理  | 读取配置文件失败、网络超时等 |
| 异常（Panic） | 不可预期的、严重的问题（如数组越界）  | 通过 `panic` 触发，`recover` 捕获 | 空指针访问、除以零、断言失败等 |

Go 的设计哲学是：**“错误是值，异常是流程中断”**。对于可预期的问题，应使用 `error` 类型返回并显式处理；对于不可恢复的严重错误，才使用 `panic` 中断程序流程。


### 二、`panic`：触发异常
`panic` 是 Go 的内置函数，用于触发“异常状态”，中断当前函数的正常执行流程。


#### 1. `panic` 的触发方式
- **主动调用 `panic()`**：开发者在代码中显式调用 `panic` 函数，传入一个任意类型的参数（通常是错误信息）。
  ```go
  func checkAge(age int) {
      if age < 0 {
          panic("年龄不能为负数") // 主动触发异常
      }
  }
  ```

- **运行时自动触发**：当程序出现严重错误（如语法无法处理的情况），Go 运行时会自动触发 `panic`。
  ```go
  func main() {
      arr := []int{1, 2, 3}
      fmt.Println(arr[10]) // 数组越界，运行时自动触发panic
  }
  ```


#### 2. `panic` 的执行流程
当 `panic` 被触发后，程序会按以下步骤执行：
1. **终止当前函数的后续代码**：`panic` 所在的函数会立即停止执行后续语句。
2. **执行当前函数的所有 `defer` 语句**：确保资源清理等操作被执行。
3. **向上传播异常**：`panic` 会沿着调用栈向上传递，终止上层函数的执行，并执行它们的 `defer` 语句。
4. **程序崩溃**：如果 `panic` 一直传播到程序入口（`main` 函数）且未被捕获，程序会打印错误信息和堆栈跟踪，然后退出。

示例：
```go
func a() {
    fmt.Println("a start")
    defer fmt.Println("a defer") // 步骤2：执行当前函数的defer
    b()
    fmt.Println("a end") // 不会执行（被panic中断）
}

func b() {
    fmt.Println("b start")
    defer fmt.Println("b defer") // 步骤2：执行当前函数的defer
    panic("发生异常") // 触发panic
    fmt.Println("b end") // 不会执行
}

func main() {
    fmt.Println("main start")
    defer fmt.Println("main defer") // 步骤2：执行当前函数的defer
    a()
    fmt.Println("main end") // 不会执行
}
```

输出：
```
main start
a start
b start
b defer  // b的defer执行
a defer  // a的defer执行
main defer  // main的defer执行
panic: 发生异常  // 最终崩溃，打印错误
```


### 三、`recover`：捕获异常
`recover` 是 Go 的内置函数，用于**捕获 `panic` 触发的异常**，阻止其继续向上传播，使程序恢复正常执行。


#### 1. `recover` 的使用条件
`recover` 必须满足以下条件才能生效：
- **必须在 `defer` 语句中调用**：`recover` 只有在延迟执行的函数（`defer` 修饰的函数）中调用时，才能捕获到 `panic`。
- **必须在 `panic` 传播路径上**：`recover` 只能捕获当前 Goroutine 中传播的 `panic`。


#### 2. `recover` 的基本用法
```go
func safeFunc() {
    defer func() {
        // 在defer函数中调用recover
        if err := recover(); err != nil {
            // 捕获到panic，err为panic的参数
            fmt.Printf("捕获到异常: %v\n", err)
        }
    }()
    
    // 可能触发panic的代码
    panic("这是一个异常")
}

func main() {
    safeFunc()
    fmt.Println("程序继续执行") // 不会崩溃，正常执行
}
```

输出：
```
捕获到异常: 这是一个异常
程序继续执行
```


#### 3. `recover` 的返回值
- 如果捕获到 `panic`，`recover()` 返回 `panic` 的参数（任意类型）。
- 如果没有 `panic` 发生，`recover()` 返回 `nil`。


#### 4. 多层调用中的 `recover`
`recover` 只能捕获其所在函数及下层函数触发的 `panic`。如果 `panic` 已经被下层的 `recover` 捕获，上层的 `recover` 将无法再捕获。

示例：
```go
func c() {
    panic("c的异常")
}

func b() {
    defer func() {
        if err := recover(); err != nil {
            fmt.Printf("b中捕获: %v\n", err) // 捕获c的panic
        }
    }()
    c()
}

func a() {
    defer func() {
        if err := recover(); err != nil {
            fmt.Printf("a中捕获: %v\n", err) // 不会执行（已被b捕获）
        }
    }()
    b()
}

func main() {
    a()
}
```

输出：
```
b中捕获: c的异常  // 异常在b中被捕获，不再向上传播
```


### 四、最佳实践
Go 官方推荐的异常处理原则是：**“少用 `panic`，慎用 `recover`”**，具体建议如下：


#### 1. 何时使用 `panic`？
- **不可恢复的严重错误**：如配置文件缺失导致程序无法启动、数据库连接失败且无法重试等。
- **开发阶段的断言**：如对函数参数的合法性检查（如必须非空的参数为空时）。
- **库的内部错误**：库函数遇到无法处理的错误时，可通过 `panic` 告知调用者（但调用者应能通过 `recover` 处理）。

避免用 `panic` 处理可预期的错误（如“文件不存在”应返回 `error`，而非 `panic`）。


#### 2. 何时使用 `recover`？
- **顶层函数捕获**：在程序的顶层函数（如 `main` 或 HTTP 服务器的请求处理函数）中使用 `recover`，防止单个请求的异常导致整个程序崩溃。
  ```go
  // HTTP服务器示例：单个请求的panic不影响整体服务
  func handler(w http.ResponseWriter, r *http.Request) {
      defer func() {
          if err := recover(); err != nil {
              w.WriteHeader(http.StatusInternalServerError)
              fmt.Fprintf(w, "服务器内部错误: %v", err)
          }
      }()
      // 处理请求的逻辑（可能触发panic）
      panic("处理请求时出错")
  }

  func main() {
      http.HandleFunc("/", handler)
      http.ListenAndServe(":8080", nil) // 即使某个请求panic，服务仍能继续运行
  }
  ```

- **资源清理保证**：在捕获 `panic` 后，可进行必要的资源清理（如关闭文件、释放锁），再决定是否重新抛出 `panic`。
  ```go
  func riskyOperation() {
      file, _ := os.Open("data.txt")
      defer func() {
          file.Close() // 确保文件关闭
          if err := recover(); err != nil {
              fmt.Printf("操作失败: %v\n", err)
              // 可选：重新抛出panic，让上层处理
              // panic(err)
          }
      }()
      // 可能触发panic的操作
  }
  ```


#### 3. 避免滥用 `recover`
- 不要在每个函数中都用 `recover`，这会隐藏真正的错误，增加调试难度。
- 捕获 `panic` 后，应记录详细日志（包括堆栈信息），便于排查问题。可通过 `debug.Stack()` 获取堆栈信息：
  ```go
  import "runtime/debug"

  defer func() {
      if err := recover(); err != nil {
          fmt.Printf("捕获异常: %v\n堆栈信息: %s\n", err, debug.Stack())
      }
  }()
  ```


### 五、总结
Go 的异常机制核心是：
- **`panic`**：用于触发不可预期的严重错误，中断程序流程并向上传播。
- **`recover`**：必须在 `defer` 中调用，用于捕获 `panic`，阻止程序崩溃，使流程恢复正常。
- **`defer`**：保证 `recover` 在 `panic` 传播时被执行，是异常捕获的前提。

其设计思想是“简洁、显式”：通过区分“可预期错误（`error`）”和“不可预期异常（`panic`）”，让开发者更清晰地处理程序中的问题，同时避免了 `try-catch` 机制可能导致的代码嵌套过深问题。

--- 

### 2. 网络编程

```go
package main

import (
    "bufio"
    "fmt"
    "net"
    "os"
    "strings"
)

// 简单的TCP服务器 -- TCP 服务器实现，负责监听端口、接受连接、处理客户端消息
func startServer() {
    // net.Listen("tcp", ":8080")：创建 TCP 监听器，监听本地所有 IP 的 8080 端口
    // 错误处理：如果监听失败（如端口被占用），打印错误并退出
    // defer listener.Close()：确保服务器退出时关闭监听器，释放资源
    listener, err := net.Listen("tcp", ":8080")
    if err != nil {
        fmt.Println("Error starting server:", err)
        return
    }
    defer listener.Close()
    fmt.Println("Server started on :8080")

    // 接受客户端连接
    for {
        conn, err := listener.Accept()  // 阻塞等待客户端连接
        if err != nil { ... }  // 处理连接错误
        
        go handleConnection(conn)  // 启动goroutine处理当前连接
    }
    // 无限循环：服务器持续接受新的客户端连接
    // listener.Accept()：阻塞方法，直到有客户端连接到来，返回net.Conn对象（代表连接）
    // go handleConnection(conn)：用 goroutine 并发处理每个连接，使服务器可同时服务多个客户端（非阻塞）
}

// 处理连接（handleConnection(conn net.Conn)）
func handleConnection(conn net.Conn) {
    defer conn.Close()  // 确保连接最终关闭
    clientAddr := conn.RemoteAddr().String()  // 获取客户端地址（IP:端口）
    fmt.Println("New connection from", clientAddr)

    reader := bufio.NewReader(conn)  // 创建带缓冲的读取器
    for {
        // 读取客户端消息（以换行符'\n'为结束标志）
        message, err := reader.ReadString('\n')
        if err != nil { ... }  // 处理读取错误（如客户端断开连接）
        
        message = strings.TrimSpace(message)  // 去除首尾空白（包括换行符）
        fmt.Printf("Received from %s: %s\n", clientAddr, message)

        // 向客户端发送响应
        response := fmt.Sprintf("Server received: %s\n", message)
        _, err = conn.Write([]byte(response))
        if err != nil { ... }  // 处理写入错误

        // 如果客户端发送"exit"，断开连接
        if message == "exit" {
            break
        }
    }
    fmt.Println("Connection from", clientAddr, "closed")
}
// 核心逻辑：循环读取客户端消息→处理消息→返回响应→判断是否退出
// 并发保障：每个连接在独立 goroutine 中处理，互不阻塞
// 通信协议：消息以\n为分隔符（客户端需确保消息末尾有换行，服务器用ReadString('\n')读取完整消息）
// 退出条件：客户端发送 "exit" 时，跳出循环并关闭连接

// TCP客户端 -- TCP 客户端实现，负责连接服务器、发送消息、接收响应
func startClient() {
    // 连接服务器（localhost:8080）
    conn, err := net.Dial("tcp", "localhost:8080")
    if err != nil { ... }  // 处理连接失败（如服务器未启动）
    defer conn.Close()  // 确保退出时关闭连接

    scanner := bufio.NewScanner(os.Stdin)  // 创建标准输入扫描器（读取用户输入）
    for {
        fmt.Print("Enter message (type 'exit' to quit): ")
        scanner.Scan()  // 读取用户输入（直到回车）
        message := scanner.Text() + "\n"  // 拼接换行符（满足服务器的消息分隔要求）

        // 发送消息到服务器
        _, err := conn.Write([]byte(message))
        if err != nil { ... }  // 处理发送错误

        // 如果输入"exit"，退出循环
        if message == "exit\n" {
            break
        }

        // 读取服务器响应（以换行符为结束标志）
        response, err := bufio.NewReader(conn).ReadString('\n')
        if err != nil { ... }  // 处理读取响应错误
        fmt.Print("Server response: ", response)
    }
}
// 连接服务器：net.Dial("tcp", "localhost:8080") 与服务器建立 TCP 连接
// 消息输入：通过bufio.Scanner读取用户从终端的输入
// 消息发送：用户输入的内容需添加\n（因为服务器用ReadString('\n')读取）
// 响应处理：发送消息后，读取服务器的响应并打印
// 退出条件：用户输入 "exit" 时，断开连接并退出

// 程序入口，根据命令行参数决定启动服务器还是客户端
func main() {
    if len(os.Args) < 2 {
        fmt.Println("Please specify 'server' or 'client'")
        return
    }

    if os.Args[1] == "server" {
        startServer()
    } else if os.Args[1] == "client" {
        startClient()
    } else {
        fmt.Println("Unknown command. Please specify 'server' or 'client'")
    }
}
// 通过命令行参数（os.Args）区分启动模式：
// 运行go run main.go server：启动 TCP 服务器
// 运行go run main.go client：启动 TCP 客户端
// 参数校验：如果未指定参数或参数错误，提示用户正确用法
```
启动服务器：
```bash
go run main.go server
# 输出：Server started on :8080
```
启动客户端（新终端）：
```bash
go run main.go client
# 输出：Enter message (type 'exit' to quit): 
```