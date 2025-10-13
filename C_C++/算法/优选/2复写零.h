//1089. 复写零
//提示
//给你一个长度固定的整数数组 arr ，请你将该数组中出现的每个零都复写一遍，并将其余的元素向右平移。
//注意：请不要在超过该数组长度的位置写入元素。请对输入的数组 就地 进行上述修改，不要从函数返回任何东西。
//示例 1：
//输入：arr = [1, 0, 2, 3, 0, 4, 5, 0]
//输出：[1, 0, 0, 2, 3, 0, 0, 4]
//解释：调用函数后，输入的数组将被修改为：[1, 0, 0, 2, 3, 0, 0, 4]
//示例 2：
//输入：arr = [1, 2, 3]
//输出：[1, 2, 3]
//解释：调用函数后，输入的数组将被修改为：[1, 2, 3]
//提示：
//1 <= arr.length <= 104
//0 <= arr[i] <= 9

//class Solution {
//public:
//    void duplicateZeros(vector<int>& arr) {
//        int dest = -1;
//        int cur = 0;
//        while ((int)dest < (int)(arr.size() - 1)) {
//            if (arr[cur] != 0) {
//                ++cur;
//                ++dest;
//            }
//            else {
//                ++cur;
//                dest += 2;
//            }
//            cout << dest << " " << cur << endl;
//        }
//
//
//        --cur;
//        while (cur >= 0 && dest >= 0) {
//            if (arr[cur] == 0) {
//                if (dest == arr.size()) {
//                    --dest;
//                    --cur;
//                    arr[dest--] = 0;
//                    continue;
//                }
//                arr[dest--] = 0;
//                arr[dest--] = 0;
//                --cur;
//            }
//            else {
//                arr[dest--] = arr[cur--];
//            }
//        }
//    }
//};

//class Solution {
//public:
//    void duplicateZeros(vector<int>& arr) {
//        // 1. 先找到最后?个数
//        int cur = 0, dest = -1, n = arr.size();
//        while (cur < n) {
//            if (arr[cur])
//                dest++;
//            else
//                dest += 2;
//            if (dest >= n - 1)
//                break;
//            cur++;
//        }
//        // 2. 处理?下边界情况
//        if (dest == n) {
//            arr[n - 1] = 0;
//            cur--;
//            dest -= 2;
//        }
//        // 3. 从后向前完成复写操作
//        while (cur >= 0) {
//            if (arr[cur])
//                arr[dest--] = arr[cur--];
//            else {
//                arr[dest--] = 0;
//                arr[dest--] = 0;
//                cur--;
//            }
//        }
//    }
//};