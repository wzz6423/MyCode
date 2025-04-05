//求1+2+3+...+n
//描述求1+2+3+...+n，要求不能使用乘除法、for、while、if、else、switch、case等关键字及条件判断语句（A?B:C）。
//数据范围：0<n≤200
//进阶： 空间复杂度O(1) ，时间复杂度O(n)
//示例1
//输入：5
//返回值：15
//示例2
//输入：1
//返回值：1
//class Sum{
//public:
//    Sum(){
//        _ret += _i;
//        _i += 1;
//    }
//
//    static int Getret(){
//        return _ret;
//    }
//private:
//    static int _i;
//    static int _ret;
//};
//
//int Sum::_i = 1;
//int Sum::_ret = 0;
//
//class Solution {
//public:
//    int Sum_Solution(int n) {
//        Sum arr[n];
//
//        return Sum::Getret();
//    }
//};

//使用内部类优化
//class Solution {
//public:
//    class Sum{
//    public:
//        Sum() {
//            _ret += _i;
//            _i += 1;
//        }
//    };
//    int Sum_Solution(int n) {
//        Sum arr[n];
//
//        return _ret;
//    }
//private:
//    static int _i;
//    static int _ret;
//};
//
//int Solution::_i = 1;
//int Solution::_ret = 0;