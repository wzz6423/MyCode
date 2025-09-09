// 438. 找到字符串中所有字母异位词
// 给定两个字符串 s 和 p，找到 s 中所有 p 的 异位词 的子串，返回这些子串的起始索引。不考虑答案输出的顺序。
// 示例 1:
// 输入: s = "cbaebabacd", p = "abc"
// 输出: [0,6]
// 解释:
// 起始索引等于 0 的子串是 "cba", 它是 "abc" 的异位词。
// 起始索引等于 6 的子串是 "bac", 它是 "abc" 的异位词。
// 示例 2:
// 输入: s = "abab", p = "ab"
// 输出: [0,1,2]
// 解释:
// 起始索引等于 0 的子串是 "ab", 它是 "ab" 的异位词。
// 起始索引等于 1 的子串是 "ba", 它是 "ab" 的异位词。
// 起始索引等于 2 的子串是 "ab", 它是 "ab" 的异位词。
// 提示:
// 1 <= s.length, p.length <= 3 * 104
// s 和 p 仅包含小写字母

// 暴力解法:
// 把 p 的长度记为 len, 所有元素放入哈希表并计数, 从 s 中找到每个长度为 len 的子串, 也放入哈希表并计数, 比较两个哈希表是否相等
// 时间复杂度: O(n * m) n = s.length, m = p.length
// 空间复杂度: O(1) 哈希表大小固定为 26
/*
    s 串, 假设 len(p) = 3: 
    ----------------------------------
    ===              --> 第一次查找的子串
     ===             --> 第二次查找的子串
     ..              --> 两次查找的重复元素
    因此: 
    .  .
    ↑  ↑
    删 增
    可以理解为: 直接移动长度为 len 的窗口
*/
// 滑动窗口: 
// 1. left = 0, right = 0
// 2. 进窗口 -- map[s[right]]++
// 3. 判断(right - left + 1 == len) -- 出窗口 -- map[s[left]]--, left++
// 4. 更新结果(map1 == map2)

// class Solution {
// public:
//     vector<int> findAnagrams(string s, string p) {
//         vector<int> ret;
//         int len = p.size();
//         for (const auto& e : p) {
//             ++hashp[e - 'a'];
//         }
//         char hashs[26] = {0};
//         for (int left = 0, right = 0; right < s.size(); ++right) {
//             ++hashs[s[right] - 'a'];
//             if (right - left + 1 == len) {
//                 if(check(hashs)){
//                     ret.push_back(left);
//                 }
//                 --hashs[s[left++] - 'a'];
//             }
//         }
//         return ret;
//     }

// private:
//     char hashp[26] = {0};
//     bool check(const char hashs[26]) {
//         for(int i = 0; i < 26; ++i){
//             if(hashp[i] != hashs[i]){
//                 return false;
//             }
//         }
//         return true;
//     }
// };

// 5. 优化更新结果判断条件:
// 利用变量 count 统计窗口中"有效字符"个数
// 有效字符: 出现在 p 中的字符, 并且窗口中出现的次数 <= p 中出现的次数
// 进窗口: if (hashs[s[right] - 'a'] <= hashp[s[right] - 'a']) ++count;
// 出窗口: if (hashs[s[left] - 'a'] >= hashp[s[left] - 'a']) --count;
// 更新结果: if (count == len) ret.push_back(left);
// class Solution {
// public:
//     vector<int> findAnagrams(string s, string p) {
//         vector<int> ret;
//         int len = p.size();
//         int hashp[26] = {0}; // 统计字符串 p 的每个字符出现个数
//         for(const char& e : p) ++hashp[e - 'a'];
//         int hashs[26] = {0}; // 统计窗口里每个字符出现次数
//         for(int left = 0, right = 0, count = 0; right < s.size(); ++right){
//             char in = s[right] - 'a';
//             if(++hashs[in] <= hashp[in]) ++count; // 进窗口 + 判断维护 count
//             if(right - left + 1 > len){ // 判断
//                 char out = s[left++] - 'a';
//                 if(hashs[out]-- <= hashp[out]) --count; // 出窗口 + 判断维护 count
//             }
//             // 更新结果
//             if(count == len) ret.push_back(left);
//         }
//         return ret;
//     }
// };