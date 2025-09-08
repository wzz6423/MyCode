//1004. 最大连续1的个数 III
//给定一个二进制数组 nums 和一个整数 k，假设最多可以翻转 k 个 0 ，则返回执行操作后 数组中连续 1 的最大个数 。
//示例 1：
//输入：nums = [1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0], K = 2
//输出：6
//解释：[1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1]
//粗体数字从 0 翻转到 1，最长的子数组长度为 6。
//示例 2：
//输入：nums = [0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1], K = 3
//输出：10
//解释：[0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1]
//粗体数字从 0 翻转到 1，最长的子数组长度为 10。
//提示：
//1 <= nums.length <= 105
//nums[i] 不是 0 就是 1
//0 <= k <= nums.length

// 转化: 找出最长的子数组, 该子数组中 0 的个数 <= k -- zero 计数器统计 0 出现次数
// 1. 暴力解法: 枚举所有子数组, 找出长度最长的
// 2. 滑动窗口: 维护一个窗口 [left, right], 统计窗口内 0 的个数, 当 0 的个数超过 k 时, 收缩左边界 -- O(N) O(1)
//    zero == k时, left 到 right - 1 的位置
//    1. left = 0, right = 0
//    2. 进窗口
//    3. 判断  --  出窗口?
//    4. 更新结果
// class Solution {
// public:
//     int longestOnes(vector<int>& nums, int k) {
//         int lenth = 0;
//         int left = 0;
//         int right = 0;
//         int zero = k;
//         while (right < nums.size()) {
//             if (nums[right]) {
//                 ++right;
//             } else if (!nums[right] && zero) {
//                 ++right;
//                 --zero;
//             } else {
//                 ++left;
//                 right = left;
//                 zero = k;
//             }
//             lenth = right - left > lenth ? right - left : lenth;
//         }
//         return lenth;
//     }
// };
// ======================================================================================================
// class Solution {
// public:
//     int longestOnes(vector<int>& nums, int k) {
//         int ret = 0;
//         for (int left = 0, right = 0, zero = 0; right < nums.size(); ++right) {
//             if (!nums[right]) ++zero; // 进窗口
//             while (zero > k) if (!nums[left++]) --zero; // 判断 + 出窗口
//             ret = right - left + 1 > ret ? right - left + 1 : ret; // 更新结果
//         }
//         return ret;
//     }
// };