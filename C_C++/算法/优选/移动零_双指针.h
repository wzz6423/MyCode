//283. �ƶ���
//��ʾ
//����һ������ nums����дһ������������ 0 �ƶ��������ĩβ��ͬʱ���ַ���Ԫ�ص����˳��
//��ע�� �������ڲ���������������ԭ�ض�������в�����
//ʾ�� 1:
//����: nums = [0,1,0,3,12]
//���: [1,3,12,0,0]
//ʾ�� 2:
//����: nums = [0]
//���: [0]
//��ʾ:
//1 <= nums.length <= 104
//-231 <= nums[i] <= 231 - 1
//���ף����ܾ���������ɵĲ���������

//class Solution {
//public:
//    void moveZeroes(vector<int>& nums) {
//        int dest = 0;
//        int cur = 0;
//        while (dest < nums.size() && cur < nums.size()) {
//            while (dest < nums.size() && nums[dest] != 0) {
//                ++dest;
//            }
//            while (cur < nums.size() && nums[cur] == 0) {
//                ++cur;
//            }
//            if (dest < cur && cur < nums.size() && nums[dest] == 0 && nums[cur] != 0) {
//                std::swap(nums[dest], nums[cur]);
//            }
//            else if (dest > cur && (dest + 1) < nums.size() && nums[dest] == 0 && nums[cur] != 0) {
//                cur = dest + 1;
//                continue;
//            }
//            else {
//                break;
//            }
//        }
//    }
//};

//class Solution {
//public:
//    void moveZeroes(vector<int>& nums) {
//        for (int dest = -1, cur = 0; cur < nums.size(); ++cur) {
//            if (nums[cur] != 0) {
//                swap(nums[++dest], nums[cur]);
//            }
//        }
//    }
//};