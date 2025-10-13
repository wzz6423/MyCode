// 34. 在排序数组中查找元素的第一个和最后一个位置
// 给你一个按照非递减顺序排列的整数数组 nums，和一个目标值 target。请你找出给定目标值在数组中的开始位置和结束位置。
// 如果数组中不存在目标值 target，返回 [-1, -1]。
// 你必须设计并实现时间复杂度为 O(log n) 的算法解决此问题。
// 示例 1：
// 输入：nums = [5,7,7,8,8,10], target = 8
// 输出：[3,4]
// 示例 2：
// 输入：nums = [5,7,7,8,8,10], target = 6
// 输出：[-1,-1]
// 示例 3：
// 输入：nums = [], target = 0
// 输出：[-1,-1]
// 提示：
// 0 <= nums.length <= 105
// -109 <= nums[i] <= 109
// nums 是一个非递减数组
// -109 <= target <= 109
// 暴力解法：
// 遍历数组，找到第一个和最后一个目标值的位置
// 时间复杂度：O(n)
// 空间复杂度：O(1)
// 优化：
// 查找左端点：[小于target，大于等于target] -- 二段性
/*
    mid < target --> left = mid + 1   \
                                       =》[left,right]
    mid >= target --> right = mid     /
*/
// 循环条件：left < right
/*
    有结果：
    [      ][            ]
    -------.--------------
          ret
    没结果：
    全大于target：        全小于target：
    [            ]       [             ]
    .--------------      --------------.
    ret                               ret
    
    left == right --> 最终结果，不用判断
    有结果的时候如果判断了就是死循环
*/
// 求中点
/*
    // mid = left + (right - left) >> 1
    // mid = left + (right - left + 1) >> 1

    即：
    mid = left + (right - left) / 2
    mid = left + (right - left + 1) / 2 ==> 当只剩两个元素时，如果是 >= 的情况mid会与right重叠，陷入死循环
*/
// 查找右端点：[小于等于target，大于target] -- 二段性
/*
    mid <= target --> left = mid       \
                                        =》[left,right]
    mid > target --> right = mid - 1   /
*/
// 循环条件：left < right
// 求中点
// mid = left + (right - left + 1) / 2
// class Solution {
// public:
//     vector<int> searchRange(vector<int>& nums, int target) {
//         vector<int> ret{-1,-1};
//         if(nums.empty()) return ret;
//         int ret_left = 0, ret_right = 0;
//         int left = 0;
//         int right = nums.size() - 1;
//         while(left < right){
//             int mid = left + ((right - left) >> 1);
//             if(nums[mid] < target){
//                 left = mid + 1;
//             }
//             else{
//                 right = mid;
//             }
//         }
//         ret_left = left;
//         right = nums.size() - 1;
//         while(left < right){
//             int mid = left + ((right - left + 1) >> 1);
//             if(nums[mid] <= target){
//                 left = mid;
//             }
//             else{
//                 right = mid - 1;
//             }
//         }
//         ret_right = right;
//         if(ret_left == ret_right && nums[ret_left] == target){
//             ret[0] = ret[1] = ret_left;
//         }
//         else if(ret_left != ret_right){
//             ret[0] = ret_left;
//             ret[1] = ret_right;
//         }
//         return ret;
//     }
// };
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// class Solution {
// public:
//     vector<int> searchRange(vector<int>& nums, int target) {
//         if(nums.empty()) return {-1,-1};
//         int ret_left = 0;
//         int left = 0;
//         int right = nums.size() - 1;
//         while(left < right){
//             int mid = left + ((right - left) >> 1);
//             if(nums[mid] < target){
//                 left = mid + 1;
//             }
//             else{
//                 right = mid;
//             }
//         }
//         if(nums[left] != target) return {-1,-1};
//         else ret_left = left;
//         right = nums.size() - 1;
//         while(left < right){
//             int mid = left + ((right - left + 1) >> 1);
//             if(nums[mid] <= target){
//                 left = mid;
//             }
//             else{
//                 right = mid - 1;
//             }
//         }
//         return {ret_left, right};
//     }
// };
// 模板
// 左端点
// while(left < right){
//     int mid = left + ((right - left) >> 1);
//     if(cond...){
//         left = mid + 1;
//     }
//     else{
//         right = mid;
//     }
// }
// 右端点
// while(left < right){
//     int mid = left + ((right - left + 1) >> 1);
//     if(cond...){
//         left = mid;
//     }
//     else{
//         right = mid - 1;
//     }
// }