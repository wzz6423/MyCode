// 1658. 将 x 减到 0 的最小操作数
// 给你一个整数数组 nums 和一个整数 x 。每一次操作时，你应当移除数组 nums 最左边或最右边的元素，然后从 x 中减去该元素的值。请注意，需要 修改 数组以供接下来的操作使用。
// 如果可以将 x 恰好 减到 0 ，返回 最小操作数 ；否则，返回 -1 。
// 示例 1：
// 输入：nums = [1,1,4,2,3], x = 5
// 输出：2
// 解释：最佳解决方案是移除后两个元素，将 x 减到 0 。
// 示例 2：
// 输入：nums = [5,6,7,8,9], x = 4
// 输出：-1
// 示例 3：
// 输入：nums = [3,2,20,1,1,3], x = 10
// 输出：5
// 解释：最佳解决方案是移除后三个元素和前两个元素（总共 5 次操作），将 x 减到 0 。
// 提示：
// 1 <= nums.length <= 105
// 1 <= nums[i] <= 104
// 1 <= x <= 109

// O(N) O(1)
// ⭐正难则反
// 找出最长子数组长度(len)使其值为 sum(nums) - x, 结果是 nums.size() - len
// 循环找子数组和 >= sum(nums) - x --> 滑动窗口(right 不回来了, ++left即可)
// 1. left = 0, right = 0
// 2. 进窗口 -- sum += nums[right]
// 3. 判断(子数组之和 > sum(nums) - x, 此处不能 >=, 因为要找 =) -- 出窗口(sum -= nums[left], ++left)
// 4. 判断 -- 更新结果?(子数组之和 == sum(nums) - x)
// class Solution {
// public:
//     int minOperations(vector<int>& nums, int x) {
//         int target = accumulate(nums.begin(), nums.end(), 0) - x;
//         if(target < 0) return -1;
//         int n = nums.size();
//         int ret = -1;
//         for(int left = 0, right = 0, sum = 0; right < n; ++right){
//             sum += nums[right]; // 进窗口
//             while(sum > target) sum -= nums[left++]; // 判断+出窗口
//             if(sum == target) ret = max(ret, right - left + 1);
//         }
//         if(ret == -1) return ret;
//         else return n - ret;
//     }
// };