<!-- 阅读前将文件名改为 CMakeLists.txt 体验更佳(更改后删除第 1 & 2 行) -->

# cmake 语句结尾无需分号
# cmake 一般创建一个 build 目录用于存放 cmake 产生的临时文件
# cmake 生成 makefile 文件, 会自动产生 make clean 命令
# 命令: 
# mkdir build && cd build
# cmake ..
# make
# make clean # 按需使用

# 声明所需的最低 cmake 版本
cmake_minimum_required(VERSION 3.0.0)

# 声明所需的 C++ 标准
set(CMAKE_CXX_STANDARD 23)

# 定义项目工程名称
project(test)

# 设置生成目标 -- 设置生成的可执行程序名称, 多个文件用空格或;进行间隔 -- 一般都在靠上的位置
add_executable(test test.cpp main.cpp)

# 普通变量定义 -- 多个变量内容用空格或;进行间隔, 没有内容给空""即可
# set(变量名称 变量内容)
set(target "test")
set(test1 "haha" "hehe";"heihei")
set(test2 "")

# 列表变量定义(类似数组)
# set(列表变量名称 列表变量内容)
# 添加元素使用 list(APPEND 列表变量名称 元素内容)
set(src_files "")
list(APPEND src_files main.cpp test.cpp)

# 自定义 DEBUG 宏
add_definitions(-DDEBUG)

# 预定义变量 / 宏
# CMAKE_CXX_STANDARD -- C++ 标准指定
# CMAKE_CXX_COMPILER -- C++ 编译器
# CMAKE_BUILD_TYPE -- 构建类型, 可选值有 Debug, Release, RelWithDebInfo, MinSizeRel
# CMAKE_CURRENT_SOURCE_DIR -- 当前源代码目录(当前 CMakeLists.txt 所在目录)
# CMAKE_CURRENT_BINARY_DIR -- 当前二进制目录(cmake 执行命令时所在的工作路径)
# CMAKE_INSTALL_PREFIX -- make install 安装目录, 默认是 /usr/local
# EXECUTABLE_OUTPUT_PATH -- 可执行程序输出路径

# 字符串内容替换
# striing(REPLACE ".old" ".new" dst src)
set(obj_files "")
string(REPLACE ".cpp" ".o" obj_files "${src_files}")
# 对 src_files 中的每个文件的 .cpp 后缀替换为 .o 后缀, 并将结果存储在 obj_files 中
# 取出变量中实际存储的内容: ${变量名称}

# 添加头文件路径 -- 设置 path 为头文件默认检索路径 -- 多个变量内容用空格或;进行间隔
# include_directories(path1 path2)
# 通常通过预定义变量进行相对路径的操作
set(current_path "/home/wzz/linux_test/StellarPost​​/project/server/example/compenents_example/cmake-test")
include_directories("${current_path}/log")

# 添加动态链接库  -- 多个变量内容用空格或;进行间隔 -- -l 可加可不加 -- 动态库名称可以是库名称也可以是全名
# target_link_libraries(target <PRIVATE|PUBLIC|INTERFACE> lib1 [<PRIVATE|PUBLIC|INTERFACE> lib2] ...)
# target：指定要加载动态库文件的名字: 源文件 / 动态库文件 / 可执行文件
# PRIVATE|PUBLIC|INTERFACE：动态库的访问权限，默认为 PUBLIC
# 如果各个动态库之间没有依赖关系，无需做任何设置，三者没有没有区别，一般无需指定，使用默认的 PUBLIC 即可
# 动态库的链接具有传递性，如果动态库 A 链接了动态库 B、 C；动态库 D 链接了动态库 A；此时动态库 D 相当于也链接了动态库 B、 C，并可以使用动态库 B、C 中定义的方法。
# PRIVATE|PUBLIC|INTERFACE 的区别：
# PUBLIC：在 public 后面的库会被 Link 到前面的 target 中，并且里面的符号也会被导出，提供给第三方使用
# PRIVATE：在 private 后面的库仅被 link 到前面的 target 中，并且终结掉，第三方不能感知链接了什么库
# INTERFACE：在 interface 后面引入的库不会被链接到前面的 target 中，只会导出符号
target_link_libraries(test -lspdlog fmt pthread)

# 添加静态链接库 -- 多个变量内容用空格或;进行间隔 -- -l 可加可不加 -- 静态库名称可以是库名称也可以是全名
# target_link_libraries(可执行程序名称 lib1 lib2)
target_link_libraries(test -lspdlog fmt pthread)

# 错误信息提示与打印
# message(STATUS "This is a status message")
# message(WARNING "This is a warning message")
# message(FATAL_ERROR "This is a fatal error message")

# 查找源码文件 -- 源码文件包括以 .cc .c .cpp .cxx ... 等结尾的文件
# aux_source_directory(<dir> <variable>)
# 在 dir 目录下找到所有源码文件, 并存储到 variable 变量中
aux_source_directory(${current_path} src_files)

# 查找头文件 -- 头文件包括以 .h .hpp .hh ... 等结尾的文件
# find_path(<variable> name1 path1 path2)

# 查找动态链接库 -- 动态链接库包括以 .so .dylib .dll ... 等结尾的文件
# find_library(<variable> name1 path1 path2)

# 查找静态链接库 -- 静态链接库包括以 .a .lib ... 等结尾的文件
# find_library(<variable> name1 path1 path2)

# file(GLOB/GLOB_RECURSE 变量名 要搜索的文件路径和文件类型)
# GLOB: 将指定目录下搜索到的满足条件的所有文件名生成一个列表，并将其存储到变量中

# 生成静态库
# add_library(库名称 STATIC 源文件 1 [源文件 2] ...)
# linux 中, 静态库名字分为三部分: lib+库名字+.a，库名称只需要指定出库的名字就可以了, 另外两部分在生成该文件的时候会自动填充

# 生成动态库
# add_library(库名称 SHARED 源文件 1 [源文件 2] ...)
# linux 中, 动态库名字分为三部分: lib+库名字+.so，库名称只需要指定出库的名字就可以了, 另外两部分在生成该文件的时候会自动填充

# 设置库输出路径 -- 默认是 ${PROJECT_BINARY_DIR}
# set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# 判断文件是否存在 -- 需要有结尾
# if(NOT EXISTS file)
# endif()
# NOT EXISTS 或者 EXISTS 都可以使用, file 是用于存放查找文件存在性的文件名集合的变量名

# 循环遍历 -- 需要有结尾
# foreach(val vals)
# endforeach()

# 执行外部指令 -- 如 shell 指令
# add_custom_command(
#     PRE_BUILD   # 表示在所有其他步骤之前执行的自定义命令
#     COMMAND   # 要执行的指令名称
#     ARGS      # 要执行的指令运行参数选项
#     OUTPUT    # 指定要生成的目标名称
#     DEPENDS   # 指定命令的依赖项
#     COMMENT   # 执行指令时要打印的内容
# )

# 添加嵌套子 cmake 目录
# add_subdirectory(dir)

# 设置安装路径
# INSTALL(TARGETS 可执行程序名称
#         RUNTIME DESTINATION bin # 可执行程序目标
#         LIBRARY DESTINATION lib # 动态库文件目标
#         ARCHIVE DESTINATION lib/static # 静态库文件目标
# )