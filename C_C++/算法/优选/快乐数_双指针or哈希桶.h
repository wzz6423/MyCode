//202. ������
//��дһ���㷨���ж�һ���� n �ǲ��ǿ�������
//���������� ����Ϊ��
//����һ����������ÿһ�ν������滻Ϊ��ÿ��λ���ϵ����ֵ�ƽ���͡�
//Ȼ���ظ��������ֱ���������Ϊ 1��Ҳ������ ����ѭ�� ��ʼ�ձ䲻�� 1��
//���������� ���Ϊ 1����ô��������ǿ�������
//��� n �� ������ �ͷ��� true �����ǣ��򷵻� false ��
//ʾ�� 1��
//���룺n = 19
//�����true
//���ͣ�
//12 + 92 = 82
//82 + 22 = 68
//62 + 82 = 100
//12 + 02 + 02 = 1
//ʾ�� 2��
//���룺n = 2
//�����false
//��ʾ��
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
//    int bitSum(int n) // ���� n �����ÿһλ�ϵ�ƽ����
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