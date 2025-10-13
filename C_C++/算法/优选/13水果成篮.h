// 904. 水果成篮
// 你正在探访一家农场，农场从左到右种植了一排果树。这些树用一个整数数组 fruits 表示，其中 fruits[i] 是第 i 棵树上的水果 种类 。
// 你想要尽可能多地收集水果。然而，农场的主人设定了一些严格的规矩，你必须按照要求采摘水果：
// 你只有 两个 篮子，并且每个篮子只能装 单一类型 的水果。每个篮子能够装的水果总量没有限制。
// 你可以选择任意一棵树开始采摘，你必须从 每棵 树（包括开始采摘的树）上 恰好摘一个水果 。采摘的水果应当符合篮子中的水果类型。每采摘一次，你将会向右移动到下一棵树，并继续采摘。
// 一旦你走到某棵树前，但水果不符合篮子的水果类型，那么就必须停止采摘。
// 给你一个整数数组 fruits ，返回你可以收集的水果的 最大 数目。
// 示例 1：
// 输入：fruits = [1,2,1]
// 输出：3
// 解释：可以采摘全部 3 棵树。
// 示例 2：
// 输入：fruits = [0,1,2,2]
// 输出：3
// 解释：可以采摘 [1,2,2] 这三棵树。
// 如果从第一棵树开始采摘，则只能采摘 [0,1] 这两棵树。
// 示例 3：
// 输入：fruits = [1,2,3,2,2]
// 输出：4
// 解释：可以采摘 [2,3,2,2] 这四棵树。
// 如果从第一棵树开始采摘，则只能采摘 [1,2] 这两棵树。
// 示例 4：
// 输入：fruits = [3,3,3,1,2,1,1,2,3,3,4]
// 输出：5
// 解释：可以采摘 [1,2,1,1,2] 这五棵树。
// 提示：
// 1 <= fruits.length <= 105
// 0 <= fruits[i] < fruits.length

// 转化: 找出最长子数组, 其中不同元素个数不超过 2
// 暴力解法: 罗列所有子数组, 找出最长的 -- 记录元素类型: 哈希表
/*
    --------.----------------.--------.--------
            ↑      kinds     ↑        ↑
            left            right    next
    next: 下一个不同的元素 --> left 右移
    此时不同元素个数(kinds): 不变/变小
                             ↓    ↓
    right:                  不变 右移
*/
// 滑动窗口: 右指针不断右移, 直到窗口内不同元素个数超过 2, 左指针右移直到不同元素个数不超过 2 -- O(N) O(N)
// 1. left = 0, right = 0
// 2. 进窗口 -- map[fruits[right]]++
// 3. 判断(map.size() > 2) -- 出窗口 -- map[fruits[left]]--, left++
// 4. 更新结果
// class Solution {
// public:
//     int totalFruit(vector<int>& fruits) {
//         unordered_map<int, int> tmp;
//         int ret = 0;
//         for (int left = 0, right = 0; right < fruits.size(); ++right) {
//             tmp[fruits[right]]++;
//             if (tmp.size() > 2) {
//                 while (left < right &&
//                        none_of(tmp.begin(), tmp.end(), [](const auto& pair) {
//                            return pair.second == 0;
//                        })) {
//                     --tmp[fruits[left++]];
//                 }
//                 tmp.erase(find_if(tmp.begin(), tmp.end(), [](const auto& pair) {
//                     return pair.second == 0;
//                 }));
//             }
//             int sum = accumulate(
//                 tmp.begin(), tmp.end(), 0,
//                 [](int acc, const auto& pair) { return acc + pair.second; });
//             ret = max(ret, sum);
//         }
//         return ret;
//     }
// };
// ==============================================================================================================
// class Solution {
// public:
//     int totalFruit(vector<int>& fruits) {
//         int ret = 0;
//         unordered_map<int, int> hash; // 统计窗口内出现了多少种水果
//         for(int left = 0, right = 0; right < fruits.size(); ++right){
//             hash[fruits[right]]++;
//             while(hash.size() > 2) // 判断
//             {
//                 --hash[fruits[left]];
//                 if(hash[fruits[left]] == 0){
//                     hash.erase(fruits[left]);
//                 }
//                 ++left;
//             }
//             ret = max(ret, right - left + 1);
//         }
//         return ret;
//     }
// };
// ==============================================================================================================
// class Solution {
// public:
//     int totalFruit(vector<int>& fruits) {
//         int ret = 0;
//         int hash[100001] = {0}; // 统计窗口内出现了多少种水果
//         for(int left = 0, right = 0, kinds = 0; right < fruits.size(); ++right){
//             if(hash[fruits[right]] == 0) ++kinds; // 维护水果种类
//             hash[fruits[right]]++;
//             while(kinds > 2) // 判断
//             {
//                 --hash[fruits[left]];
//                 if(hash[fruits[left]] == 0) --kinds;
//                 ++left;
//             }
//             ret = max(ret, right - left + 1);
//         }
//         return ret;
//     }
// };