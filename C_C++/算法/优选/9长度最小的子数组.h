//209. 长度最小的子数组
//给定一个含有 n 个正整数的数组和一个正整数 target 。
//找出该数组中满足其总和大于等于 target 的长度最小的 子数组[numsl, numsl + 1, ..., numsr - 1, numsr] ，并返回其长度。如果不存在符合条件的子数组，返回 0 。
//示例 1：
//输入：target = 7, nums = [2, 3, 1, 2, 4, 3]
//输出：2
//解释：子数组[4, 3] 是该条件下的长度最小的子数组。
//示例 2：
//输入：target = 4, nums = [1, 4, 4]
//输出：1
//示例 3：
//输入：target = 11, nums = [1, 1, 1, 1, 1, 1, 1, 1]
//输出：0
//提示：
//1 <= target <= 109
//1 <= nums.length <= 105
//1 <= nums[i] <= 104
//进阶：
//如果你已经实现 O(n) 时间复杂度的解法, 请尝试设计一个 O(n log(n)) 时间复杂度的解法。

// 暴力解法: 枚举所有子数组求和判断是否大于 target, 时间复杂度: O(N ^ 3) -- 枚举子数组(N ^ 2) + 求和(N)
// // 前提: 全为正整数 ---> 加得越多和越大
// 优化: 利用"单调性"使用【同向双指针】 --》 滑动窗口
// 先定义左区间, 定义一个变量 sum, 求以当前左区间开始的所有子区间的和, 第一次满足即可停止 -- 减少一次求和的遍历 -- O(N ^ 2)
// 同时当左指针移动时右指针不用变(上一次的 sum 减去原左指针的值即可)
// 操作: {初始化 left & right(left) & sum(和) + 【进窗口 + (移动 & 判断出窗口)】循环} + 更新结果(进时 or 出时)
// 正确性: 单调性
// 时间复杂度: O(N)

//class Solution {
//public:
//    int minSubArrayLen(int target, vector<int>& nums) {
//        int n = nums.size();
//        int sum = 0;
//        int len = INT_MAX;
//        for (int left = 0, right = 0; right < n; ++right) {
//            sum += nums[right]; // 进窗口
//            while (sum >= target) { // 判断
//                len = min(len, right - left + 1); // 更新结果
//                sum -= nums[left++]; // 出窗口
//            }
//        }
//        return len == INT_MAX ? 0 : len;
//    }
//};