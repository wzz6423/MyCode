//611. 有效三角形的个数
//给定一个包含非负整数的数组 nums ，返回其中可以组成三角形三条边的三元组个数。
//示例 1:
//输入: nums = [2,2,3,4]
//输出: 3
//解释:有效的组合是: 
//2,3,4 (使用第一个 2)
//2,3,4 (使用第二个 2)
//2,2,3
//示例 2:
//输入: nums = [4,2,3,4]
//输出: 4
//提示:
//1 <= nums.length <= 1000
//0 <= nums[i] <= 1000
// 1.暴力解法 -- 三层 for 循环, 时间复杂度: O(3 * N^3)
// 2.排序 + 二分, 时间复杂度: O(N * logN + N^3)
// 2.排序 + 单调性 + 双指针, 时间复杂度: O(N * logN + N^2)
// 判断三角形, 先排序, 较小的两个之和大于第三边即可
// 先确定 c , 然后确定a、b, a + b 满足成立三角形条件, 则 ab 之间的数都满足, --b 即可
//class Solution {
//public:
//    int triangleNumber(vector<int>& nums) {
//        auto Check = [](int a, int b, int c) -> bool { return a + b > c; };
//        sort(nums.begin(), nums.end());
//        int ret = 0;
//        int n = nums.size();
//        for (int hypotenuse = n - 1; hypotenuse > 1; --hypotenuse) {
//            int left = 0;
//            int right = hypotenuse - 1;
//            while (left < right) {
//                if (nums[left] + nums[right] > nums[hypotenuse]) {
//                    ret += right - left;
//                    --right;
//                }
//                else {
//                    ++left;
//                }
//            }
//
//            // int right = hypotenuse - 1;
//            // for (int left = 0; left < right; ++left) {
//            //     if (Check(nums[left], nums[right], nums[hypotenuse])) {
//            //         ret += right - left;
//            //         --left; // 左边不能动
//            //         --right;
//            //     }
//            // }
//        }
//        return ret;
//    }
//};