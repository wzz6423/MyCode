#!/bin/bash
# 2. 替换弃用的`register`关键字（参考网页2[2](@ref)）
find ./ -name "*.cpp" -exec sed -i.bak 's/register //g' {} \;

# 3. 处理_ftelli64宏冲突（参考网页5[5](@ref)）
sed -i.bak 's/#define _ftelli64 ftello64//g' ./bundle.cpp
sed -i '/#define _ftelli64 ftello/!b;n;a\#undef _ftelli64' ./bundle.cpp

# 4. 修复std::ios_base::open_mode错误（参考网页3[3](@ref)）
find ./ -type f -name "*.cpp" -exec sed -i.bak \
    's/std::ios_base::open_mode/std::ios_base::openmode/g' {} \;

# 5. 处理const成员函数修改错误（参考网页7[7](@ref)）
sed -i.bak '/void clear_err() const/{n;s/error = false/mutable error = false/}' \
    ./bundle.cpp

# 8. 清理备份文件
find ./ -name "*.bak" -exec rm {} \;

echo "修复完成！若仍有错误，请检查步骤6的手动修改"
