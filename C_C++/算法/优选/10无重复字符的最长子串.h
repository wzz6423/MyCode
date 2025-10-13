//3. 无重复字符的最长子串
//给定一个字符串 s ，请你找出其中不含有重复字符的 最长 子串 的长度。
//示例 1:
//输入: s = "abcabcbb"
//输出: 3 
//解释: 因为无重复字符的最长子串是 "abc"，所以其长度为 3。
//示例 2:
//输入: s = "bbbbb"
//输出: 1
//解释: 因为无重复字符的最长子串是 "b"，所以其长度为 1。
//示例 3:
//输入: s = "pwwkew"
//输出: 3
//解释: 因为无重复字符的最长子串是 "wke"，所以其长度为 3。
//     请注意，你的答案必须是 子串 的长度，"pwke" 是一个子序列，不是子串。
//提示：
//0 <= s.length <= 5 * 104
//s 由英文字母、数字、符号和空格组成

// 1. 暴力枚举 + 哈希(判断字符是否重复) -- O(N ^ 2)
// 2. 滑动窗口 -- 字符进入哈希表 -- O(N)
//	0.left=0, right=0
//	1.进窗口
//	2.判断  --  出窗口？(窗口出现重复字符, 从哈希表中删除该字符)
//  3.更新结果
//class Solution {
//public:
//    int lengthOfLongestSubstring(string s) {
//        int lenth = 0;
//        unordered_set<char> chars;
//        int left = 0;
//        int right = 0;
//        while (right < s.size()) {
//            if (chars.find(s[right]) == chars.end()) {
//                chars.insert(s[right++]);
//                int newLenth = chars.size();
//                lenth = newLenth > lenth ? newLenth : lenth;
//                continue;
//            }
//            chars.erase(s[left++]);
//            if (s.size() - left < lenth) {
//                break;
//            }
//        }
//        return lenth;
//    }
//};
// 优化空间复杂度 -- O(1)
//class Solution {
//public:
//    int lengthOfLongestSubstring(string s) {
//        int lenth = 0;
//        bitset<128> chars(0);
//        int left = 0;
//        int right = 0;
//        while (right < s.size()) {
//            if (!chars[s[right]]) {
//                chars[s[right++]] = 1;
//                int newLenth = chars.count();
//                lenth = newLenth > lenth ? newLenth : lenth;
//                continue;
//            }
//            chars[s[left++]] = 0;
//            if (s.size() - left < lenth) {
//                break;
//            }
//        }
//        return lenth;
//    }
//};
