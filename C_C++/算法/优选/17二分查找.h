// 704. 二分查找
// 给定一个 n 个元素有序的（升序）整型数组 nums 和一个目标值 target  ，写一个函数搜索 nums 中的 target，如果 target 存在返回下标，否则返回 -1。
// 你必须编写一个具有 O(log n) 时间复杂度的算法。
// 示例 1:
// 输入: nums = [-1,0,3,5,9,12], target = 9
// 输出: 4
// 解释: 9 出现在 nums 中并且下标为 4
// 示例 2:
// 输入: nums = [-1,0,3,5,9,12], target = 2
// 输出: -1
// 解释: 2 不存在 nums 中因此返回 -1
// 提示：
// 你可以假设 nums 中的所有元素是不重复的。
// n 将在 [1, 10000]之间。
// nums 的每个元素都将在 [-9999, 9999]之间。

// 暴力解法：暴力查找，返回结果 -- 一个一个挨个比较，每次排除一个数 -- O(n)
// 优化：利用单调升序的特性，找到一个数a，如果a大于target，说明target在a的左边，否则在右边，一次排除一批数
// 二分：数组中根据某个规律能够划分区域 --> 具有“二段性”，可以舍弃其中一段 -- O(logn)
/*
   mid < target --> left = mid + 1    \
                                   =》[left,right]
   mid > target --> right = mid - 1   /
   mid == target --> return mid
*/
// 循环结束条件：left > right
// class Solution {
// public:
//     int search(vector<int>& nums, int target) {
//         int size = nums.size();
//         int left = 0;
//         int right = size -1;
//         int ret = -1;
//         while(left <= right){
//             // int mid = (right - left) / 2 + left;
//             int mid = ((right - left) >> 1) + left;
//             if(nums[mid] < target){
//                 left = mid + 1;
//             }
//             else if(nums[mid] > target){
//                 right = mid - 1;
//             }
//             else if(nums[mid] == target){
//                 ret = mid;
//                 break;
//             }
//         }
//         return ret;
//     }
// };
// 模板
// while(left <= right){
//    int mid = ((right - left) >> 1) + left;
//    if(cond...){
//       left = mid + 1;
//    }
//    else if(cond...){
//       right = mid - 1;
//    }
//    else{
//       return ...;
//    }
// }