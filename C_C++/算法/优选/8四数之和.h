//18. 四数之和
//给你一个由 n 个整数组成的数组 nums ，和一个目标值 target 。请你找出并返回满足下述全部条件且不重复的四元组 [nums[a], nums[b], nums[c], nums[d]] （若两个四元组元素一一对应，则认为两个四元组重复）：
//0 <= a, b, c, d < n
//a、b、c 和 d 互不相同
//nums[a] + nums[b] + nums[c] + nums[d] == target
//你可以按任意顺序返回答案 。
//示例 1：
//输入：nums = [1,0,-1,0,-2,2], target = 0
//输出：[[-2,-1,1,2],[-2,0,0,2],[-1,0,0,1]]
//示例 2：
//输入：nums = [2,2,2,2,2], target = 8
//输出：[[2,2,2,2]]
//提示：
//1 <= nums.length <= 200
//-109 <= nums[i] <= 109
//-109 <= target <= 109

// 排序 + 暴力解法 + set 去重 -- 超时

// 排序 + 双指针
// 依次固定值 a, 利用三数之和办法使三数之和值为 target - a
// 依次固定值 b, 利用双指针找两个数使两数之和为 target - a - b
// 不重: 找到结果跳过相同数, b、a 各跳过相同的数
// 不漏: 找到结果不停
//class Solution {
//public:
//    vector<vector<int>> fourSum(vector<int>& nums, int target) {
//        vector<vector<int>> ret;
//        sort(nums.begin(), nums.end());
//        int n = nums.size();
//        for (int i = 0; i < n; ++i) {
//            for (int j = i + 1; j < n; ++j) {
//                int left = j + 1;
//                int right = n - 1;
//                long long aim = (long long)target - nums[i] - nums[j];
//                while (left < right) {
//                    int sum = nums[left] + nums[right];
//                    if (sum > aim) {
//                        while (left < right && nums[right] == nums[right - 1]) {
//                            --right;
//                        }
//                        --right;
//                    }
//                    else if (sum < aim) {
//                        while (left < right && nums[left] == nums[left + 1]) {
//                            ++left;
//                        }
//                        ++left;
//                    }
//                    else {
//                        ret.push_back(
//                            { nums[i], nums[j], nums[left], nums[right] });
//                        while (left < right && nums[right] == nums[right - 1]) {
//                            --right;
//                        }
//                        while (left < right && nums[left] == nums[left + 1]) {
//                            ++left;
//                        }
//                        ++left;
//                        --right;
//                    }
//                }
//                while (j < n - 1 && nums[j] == nums[j + 1]) {
//                    ++j;
//                }
//            }
//            while (i < n - 1 && nums[i] == nums[i + 1]) {
//                ++i;
//            }
//        }
//        return ret;
//    }
//};