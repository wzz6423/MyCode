//202. 快乐数
//编写一个算法来判断一个数 n 是不是快乐数。
//「快乐数」 定义为：
//对于一个正整数，每一次将该数替换为它每个位置上的数字的平方和。
//然后重复这个过程直到这个数变为 1，也可能是 无限循环 但始终变不到 1。
//如果这个过程 结果为 1，那么这个数就是快乐数。
//如果 n 是 快乐数 就返回 true ；不是，则返回 false 。
//示例 1：
//输入：n = 19
//输出：true
//解释：
//12 + 92 = 82
//82 + 22 = 68
//62 + 82 = 100
//12 + 02 + 02 = 1
//示例 2：
//输入：n = 2
//输出：false
//提示：
//1 <= n <= 231 - 1

//class Solution {
//public:
//    int Cal(int n) {
//        int ret = 0;
//        while (n != 0) {
//            int tmp = n % 10;
//            tmp = tmp * tmp;
//            n /= 10;
//            ret += tmp;
//        }
//        return ret;
//    }
//    bool isHappy(int n) {
//        int n1 = n;
//        int n2 = n;
//        while (true) {
//            n1 = Cal(n1);
//            n2 = Cal(n2);
//            n2 = Cal(n2);
//            if (n1 == n2 && n1 == 1) {
//                return true;
//            }
//            else if (n1 == n2 && n1 != 1) {
//                return false;
//            }
//        }
//    }
//};

//class Solution {
//public:
//    int bitSum(int n) // 返回 n 这个数每一位上的平方和
//    {
//        int sum = 0;
//        while (n) {
//            int t = n % 10;
//            sum += t * t;
//            n /= 10;
//        }
//        return sum;
//    }
//    bool isHappy(int n) {
//        int slow = n, fast = bitSum(n);
//        while (slow != fast) {
//            slow = bitSum(slow);
//            fast = bitSum(bitSum(fast));
//        }
//        return slow == 1;
//    }
//};

//class Solution {
//public:
//    int Cal(int n) {
//        int ret = 0;
//        while (n != 0) {
//            int tmp = n % 10;
//            tmp = tmp * tmp;
//            n /= 10;
//            ret += tmp;
//        }
//        return ret;
//    }
//    bool isHappy(int n) {
//        unordered_set<int> cal;
//        while (true) {
//            n = Cal(n);
//            if (!cal.count(n)) {
//                cal.insert(n);
//            }
//            else if (cal.count(n) && n == 1) {
//                return true;
//            }
//            else {
//                return false;
//            }
//        }
//    }
//};