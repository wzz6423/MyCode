//11. 盛最多水的容器
//提示
//给定一个长度为 n 的整数数组 height 。有 n 条垂线，第 i 条线的两个端点是(i, 0) 和(i, height[i]) 。
//找出其中的两条线，使得它们与 x 轴共同构成的容器可以容纳最多的水。
//返回容器可以储存的最大水量。
//说明：你不能倾斜容器。
//示例 1：
//输入：[1, 8, 6, 2, 5, 4, 8, 3, 7]
//输出：49
//解释：图中垂直线代表输入数组[1, 8, 6, 2, 5, 4, 8, 3, 7]。在此情况下，容器能够容纳水（表示为蓝色部分）的最大值为 49。
//示例 2：
//输入：height = [1, 1]
//输出：1
//提示：
//n == height.length
//2 <= n <= 105
//0 <= height[i] <= 104
// 1.暴力解法 -- 两层 for 循环 -- 超时
// 2.双指针
// 左右指针开始移动, 无论哪一个向内侧移动最终体积都会减小 -- 单调性 -- 长不变, 高减小 / 长减小, 高减小
// 因此直接淘汰左右指针中小的那个数即可, 最后选出最大值
// 时间复杂度: O(N), 空间复杂度: O(1)
//class Solution {
//public:
//    int maxArea(vector<int>& height) {
//        int left = 0;
//        int right = height.size() - 1;
//        int V = 0;
//        while (right > 0 && right > left) {
//            int high = height[left] >= height[right] ? height[right] : height[left];
//            int len = right - left;
//            if (high * len > V) {
//                V = high * len;
//            }
//
//            if (height[left] >= height[right]) {
//                --right;
//            }
//            else {
//                ++left;
//            }
//        }
//        return V;
//    }
//};
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~写法2~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//class Solution {
//public:
//    int maxArea(vector<int>& height) {
//        int left = 0;
//        int right = height.size() - 1;
//        int V = 0;
//        while (right > left) {
//            V = max(V, (min(height[left], height[right]) * (right - left)));
//
//            if (height[left] < height[right]) ++left;
//            else --right;
//        }
//        return V;
//    }
//};