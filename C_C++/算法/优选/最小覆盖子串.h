// 76. 最小覆盖子串
// 提示
// 给你一个字符串 s 、一个字符串 t 。返回 s 中涵盖 t 所有字符的最小子串。如果 s 中不存在涵盖 t 所有字符的子串，则返回空字符串 "" 。
// 注意：
// 对于 t 中重复字符，我们寻找的子字符串中该字符数量必须不少于 t 中该字符数量。
// 如果 s 中存在这样的子串，我们保证它是唯一的答案。
// 示例 1：
// 输入：s = "ADOBECODEBANC", t = "ABC"
// 输出："BANC"
// 解释：最小覆盖子串 "BANC" 包含来自字符串 t 的 'A'、'B' 和 'C'。
// 示例 2：
// 输入：s = "a", t = "a"
// 输出："a"
// 解释：整个字符串 s 是最小覆盖子串。
// 示例 3:
// 输入: s = "a", t = "aa"
// 输出: ""
// 解释: t 中两个字符 'a' 均应包含在 s 的子串中，
// 因此没有符合条件的子字符串，返回空字符串。
// 提示：
// m == s.length
// n == t.length
// 1 <= m, n <= 105
// s 和 t 由英文字母组成
// 进阶：你能设计一个在 o(m+n) 时间内解决此问题的算法吗？

// 暴力解法: 暴力枚举 + 哈希表(统计每个字符出现的次数)
/*
          |        符合要求        |
    ------.---.--------------------.---------
          ↑   ↑                    ↑
         left                    right
            →left                right
    情况: 
        1. 符合要求: right 不动
        2. 不符合要求: right 右移
*/
// 滑动窗口 + 哈希表:
// 1. left = 0, right = 0
// 2. 进窗口 hashs[in]++
// 3. 判断 hashs,hasht -- 更新结果(起始位置, 最短长度) -- 出窗口 hashs[out]--
// 优化: 使用变量 count 标记有效字符种类 -- 种类个数完全相同时(===)才 ++count
// 维护 count: 进之后 hashs[in] === hasht[in], count++
//             出之前 hashs[out] === hasht[out], count--
// 判断条件: count == hasht.size()

// class Solution {
// public:
//     string minWindow(string s, string t) {
//         int pos = 0, len = INT_MAX; // 返回的起始位置及长度
//         unordered_map<char, int> hasht; // t 的字符类型及个数
//         for(const auto& e: t) ++hasht[e];
//         int count = hasht.size(); // t 的字符个数
        
//         unordered_map<char, int> hashs; // s 的字符类型及个数
//         for(int left = 0, right = 0, cnt = 0; right < s.size(); ++right){ // 滑动窗口, cnt 是种类个数完全相同的字符个数
//             char ch = s[right]; // 记录当前字符
//             ++hashs[ch];// 进窗口
//             if(hasht.find(ch) != hasht.end() && hashs[ch] == hasht[ch]) ++cnt; // 维护 cnt

//             // 当当前窗口包含t的所有字符时, 尝试缩小窗口
//             while (cnt == count) {
//                 // 更新最小窗口
//                 if (right - left + 1 < len) {
//                     pos = left;
//                     len = right - left + 1;
//                 }
//                 char leftChar = s[left];
//                 --hashs[leftChar]; // 字符出窗口
//                 // 如果移出的字符在t中, 且移出后该字符的频率不再满足t中的要求, 则cnt减少
//                 if (hasht.find(leftChar) != hasht.end() && hashs[leftChar] < hasht[leftChar]) {
//                     --cnt;
//                 }
//                 ++left; // 左指针右移
//             }
//         }
//         if(len == INT_MAX) len = 0;
//         return s.substr(pos, len);
//     }
// };
// ==========================================================================================
// class Solution {
// public:
//     string minWindow(string s, string t) {
//         int pos = -1, len = INT_MAX; // 返回的起始位置 & 长度

//         int hasht[128] = {0}; // 统计字符串 t 的每一个字符出现频次
//         int kinds = 0; // 统计有效字符种类
//         for(const auto& e : t) if(hasht[e]++ == 0) ++kinds;

//         int hashs[128] = {0}; // 统计窗口内每一个字符出现的频次
//         for(int left = 0, right = 0, cnt = 0; right < s.size(); ++right){
//             char in = s[right];
//             // ++hashs[in]; // 进窗口
//             if(++hashs[in] == hasht[in]) ++cnt; // 进窗口 + 维护 cnt

//             // 判断 -- 固定窗口大小一般 if, 非固定大小窗口一般 while
//             while(cnt == kinds){
//                 if(right - left + 1 < len){ // 更新结果
//                     pos = left;
//                     len = right -left + 1;
//                 }
//                 char out = s[left++]; // 右移 left
//                 if(hashs[out]-- == hasht[out]) --cnt; // 维护 cnt + 出窗口
//                 // --hashs[out]; // 出窗口
//             }
//         }
//         if(len == INT_MAX) return "";
//         return s.substr(pos,len);
//     }
// };