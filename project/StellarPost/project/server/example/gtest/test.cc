#include <gtest/gtest.h>

// TEST(测试名称, 测试用例名称){} // 宏

// ASSERT(条件) // 断言宏 -- 失败退出当前测试用例
// EXPECT(条件) // 期望宏 -- 失败继续执行

// ASSERT_EQ(期望值, 实际值) // 断言相等
// EXPECT_EQ(期望值, 实际值) // 期望相等
// ASSERT_NE(期望值, 实际值) // 断言不相等
// EXPECT_NE(期望值, 实际值) // 期望不相等
// ASSERT_TRUE(条件) // 断言为真
// EXPECT_TRUE(条件) // 期望为真
// ASSERT_FALSE(条件) // 断言为假
// EXPECT_FALSE(条件) // 期望为假
// ASSERT_STREQ(期望字符串, 实际字符串) // 断言字符串相等
// EXPECT_STREQ(期望字符串, 实际字符串) // 期望字符串相等
// ASSERT_STRNE(期望字符串, 实际字符串) // 断言字符串不相等
// EXPECT_STRNE(期望字符串, 实际字符串) // 期望字符串不相等
// ASSERT_FLOAT_EQ(期望值, 实际值) // 断言浮点数相等
// EXPECT_FLOAT_EQ(期望值, 实际值) // 期望浮点数相等
// ASSERT_DOUBLE_EQ(期望值, 实际值) // 断言双精度浮点数相等
// EXPECT_DOUBLE_EQ(期望值, 实际值) // 期望双精度浮点数相等
// ASSERT_THROW(表达式, 异常类型) // 断言抛出异常
// EXPECT_THROW(表达式, 异常类型) // 期望抛出异常
// ASSERT_NO_THROW(表达式) // 断言不抛出异常
// EXPECT_NO_THROW(表达式) // 期望不抛出异常
// ASSERT_ANY_THROW(表达式) // 断言抛出任意异常
// EXPECT_ANY_THROW(表达式) // 期望抛出任意异常
// ASSERT_EXIT(表达式, 退出码, 正则表达式) // 断言程序退出
// EXPECT_EXIT(表达式, 退出码, 正则表达式) // 期望程序退出
// ASSERT_GE(期望值, 实际值) // 断言大于等于
// EXPECT_GE(期望值, 实际值) // 期望大于等于
// ASSERT_GT(期望值, 实际值) // 断言大于
// EXPECT_GT(期望值, 实际值) // 期望大于
// ASSERT_LE(期望值, 实际值) // 断言小于等于
// EXPECT_LE(期望值, 实际值) // 期望小于等于
// ASSERT_LT(期望值, 实际值) // 断言小于
// EXPECT_LT(期望值, 实际值) // 期望小于
// ASSERT_PRED_FORMAT2(谓词, 期望值, 实际值) // 断言谓词格式化
// EXPECT_PRED_FORMAT2(谓词, 期望值, 实际值) // 期望谓词格式化
// ASSERT_PRED_FORMAT3(谓词, 期望值, 实际值, 其他参数) // 断言谓词格式化
// EXPECT_PRED_FORMAT4(谓词, 期望值, 实际值, 其他参数1, 其他参数2) // 期望谓词格式化
// ASSERT_PRED_FORMAT5(谓词, 期望值, 实际值, 其他参数1, 其他参数2, 其他参数3) // 断言谓词格式化
// EXPECT_PRED_FORMAT6(谓词, 期望值, 实际值, 其他参数1, 其他参数2, 其他参数3, 其他参数4) // 期望谓词格式化

auto Add(int a, int b) -> int
{
    return a + b;
}

TEST(test1, testAdd)
{
    // 断言相等
    ASSERT_EQ(Add(1, 2), 3);
    // 期望相等
    EXPECT_EQ(Add(2, 3), 5);
}

TEST(test2, testAdd)
{
    // 断言不相等
    ASSERT_NE(Add(1, 2), 4);
    // 期望不相等
    EXPECT_NE(Add(2, 3), 6);
}

auto main(int argc, char **argv) -> int
{
    // 单元测试框架初始化
    testing::InitGoogleTest(&argc, argv);
    // 进行单元测试
    // 运行所有测试用例
    // 如果有一个测试用例失败，则返回非零值
    // RUN_ALL_TESTS(); // 返回值是整型

    return RUN_ALL_TESTS();
}