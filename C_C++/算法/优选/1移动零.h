//283. 移动零
//提示
//给定一个数组 nums，编写一个函数将所有 0 移动到数组的末尾，同时保持非零元素的相对顺序。
//请注意 ，必须在不复制数组的情况下原地对数组进行操作。
//示例 1:
//输入: nums = [0,1,0,3,12]
//输出: [1,3,12,0,0]
//示例 2:
//输入: nums = [0]
//输出: [0]
//提示:
//1 <= nums.length <= 104
//-231 <= nums[i] <= 231 - 1
//进阶：你能尽量减少完成的操作次数吗？

//class Solution {
//public:
//    void moveZeroes(vector<int>& nums) {
//        int dest = 0;
//        int cur = 0;
//        while (dest < nums.size() && cur < nums.size()) {
//            while (dest < nums.size() && nums[dest] != 0) {
//                ++dest;
//            }
//            while (cur < nums.size() && nums[cur] == 0) {
//                ++cur;
//            }
//            if (dest < cur && cur < nums.size() && nums[dest] == 0 && nums[cur] != 0) {
//                std::swap(nums[dest], nums[cur]);
//            }
//            else if (dest > cur && (dest + 1) < nums.size() && nums[dest] == 0 && nums[cur] != 0) {
//                cur = dest + 1;
//                continue;
//            }
//            else {
//                break;
//            }
//        }
//    }
//};

//class Solution {
//public:
//    void moveZeroes(vector<int>& nums) {
//        for (int dest = -1, cur = 0; cur < nums.size(); ++cur) {
//            if (nums[cur] != 0) {
//                swap(nums[++dest], nums[cur]);
//            }
//        }
//    }
//};