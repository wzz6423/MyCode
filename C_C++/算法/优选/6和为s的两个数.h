//LCR 179. 查找总价格为目标值的两个商品
//购物车内的商品价格按照升序记录于数组 price。请在购物车中找到两个商品的价格总和刚好是 target。若存在多种情况，返回任一结果即可。
//示例 1：
//输入：price = [3, 9, 12, 15], target = 18
//输出：[3, 15] 或者[15, 3]
//示例 2：
//输入：price = [8, 21, 27, 34, 52, 66], target = 61
//输出：[27, 34] 或者[34, 27]
//提示：
//1 <= price.length <= 10 ^ 5
//1 <= price[i] <= 10 ^ 6
//1 <= target <= 2 * 10 ^ 6
// 1.暴力解法 -- 列举所有两数之和, 两个 for 循环 -- 时间复杂度: O(N^2) -- 超时
// 2.利用数组有序特性 -- 二分 -- 不是最优解
// 利用数组有序特性 -- 单调性 -- 双指针 -- 时间复杂度: O(N)
//class Solution {
//public:
//    vector<int> twoSum(vector<int>& price, int target) {
//        int left = 0;
//        int right = price.size() - 1;
//        vector<int> ret;
//        while (left < right) {
//            if (price[left] + price[right] > target) {
//                --right;
//            }
//            else if (price[left] + price[right] < target) {
//                ++left;
//            }
//            else {
//                ret.push_back(price[left]);
//                ret.push_back(price[right]);
//                break;
//            }
//        }
//        return ret;
//    }
//};