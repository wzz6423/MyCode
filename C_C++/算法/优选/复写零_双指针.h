//1089. ��д��
//��ʾ
//����һ�����ȹ̶����������� arr �����㽫�������г��ֵ�ÿ���㶼��дһ�飬���������Ԫ������ƽ�ơ�
//ע�⣺�벻Ҫ�ڳ��������鳤�ȵ�λ��д��Ԫ�ء������������� �͵� ���������޸ģ���Ҫ�Ӻ��������κζ�����
//ʾ�� 1��
//���룺arr = [1, 0, 2, 3, 0, 4, 5, 0]
//�����[1, 0, 0, 2, 3, 0, 0, 4]
//���ͣ����ú�������������齫���޸�Ϊ��[1, 0, 0, 2, 3, 0, 0, 4]
//ʾ�� 2��
//���룺arr = [1, 2, 3]
//�����[1, 2, 3]
//���ͣ����ú�������������齫���޸�Ϊ��[1, 2, 3]
//��ʾ��
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
//        // 1. ���ҵ����?����
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
//        // 2. ����?�±߽����
//        if (dest == n) {
//            arr[n - 1] = 0;
//            cur--;
//            dest -= 2;
//        }
//        // 3. �Ӻ���ǰ��ɸ�д����
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