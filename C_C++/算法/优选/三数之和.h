//15. 三数之和
//提示
//给你一个整数数组 nums ，判断是否存在三元组[nums[i], nums[j], nums[k]] 满足 i != j、i != k 且 j != k ，同时还满足 nums[i] + nums[j] + nums[k] == 0 。请你返回所有和为 0 且不重复的三元组。
//注意：答案中不可以包含重复的三元组。
//示例 1：
//输入：nums = [-1, 0, 1, 2, -1, -4]
//输出： [[-1, -1, 2], [-1, 0, 1]]
//解释：
//nums[0] + nums[1] + nums[2] = (-1) + 0 + 1 = 0 。
//nums[1] + nums[2] + nums[4] = 0 + 1 + (-1) = 0 。
//nums[0] + nums[3] + nums[4] = (-1) + 2 + (-1) = 0 。
//不同的三元组是[-1, 0, 1] 和[-1, -1, 2] 。
//注意，输出的顺序和三元组的顺序并不重要。
//示例 2：
//输入：nums = [0, 1, 1]
//输出：[]
//解释：唯一可能的三元组和不为 0 。
//示例 3：
//输入：nums = [0, 0, 0]
//输出： [[0, 0, 0]]
//解释：唯一可能的三元组和为 0 。
//提示：
//3 <= nums.length <= 3000
//- 105 <= nums[i] <= 105

// 暴力解法: 暴力枚举 -- 枚举出所有三元组, 进行去重(排序) + 选择 -- 三层循环
// 1. 先排序
// 2. 结合 STL -- set
// O(logN) + O(n^3)

// 有序数组 --> 二分(低优先级) / 双指针(高优先级, 时间复杂度降级)
// 定一移二 -- 两层循环(一层普通循环 + 一层(局部)双指针循环) -- 两数之和
// 去重: set -(优化)-> 已经有序, 因此考虑过的值无需重复考虑(跳过 -- 双指针跳过, 外层循环也要跳过)
// 不漏: 检验完所有值
// 小心越界
//class Solution {
//public:
//	vector<vector<int>> threeSum(vector<int>& nums) {
//		vector<vector<int>> ret;
//		// 排序
//		sort(nums.begin(), nums.end());
//
//		// 固定第一个数
//		for (int i = 0; i < nums.size() && nums[i] <= 0; ++i) {
//			// 双指针 + 去重
//			int j = i + 1;
//			int k = nums.size() - 1;
//			int target = -nums[i];
//			while (j < k) {
//				int sum = nums[j] + nums[k];
//				if (sum > target) {
//					while (j < k && nums[k] == nums[k - 1]) {
//						--k;
//					}
//					--k;
//				}
//				else if (sum < target) {
//					while (j < k && nums[j] == nums[j + 1]) {
//						++j;
//					}
//					++j;
//				}
//				else {
//					ret.push_back({ nums[i], nums[j], nums[k] });
//					while (j < k && nums[j] == nums[j + 1]) {
//						++j;
//					}
//					while (j < k && nums[k] == nums[k - 1]) {
//						--k;
//					}
//					++j;
//					--k;
//				}
//			}
//			while (i < nums.size() - 1 && nums[i] == nums[i + 1]) {
//				++i;
//			}
//		}
//		return ret;
//	}
//};
