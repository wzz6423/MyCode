#include <stdio.h>
#include <assert.h>
//void* my_memmove(void* dec, const void* src, size_t num) {
//    assert(dec && src);
//    if (0 == num || src == dec) {
//        return dec;
//    }
//    char* ret = (char*)dec;
//    char* tmp = (char*)src;
//    if (ret > tmp) {
//        while (num--) {
//            *(ret + num - 1) = *(tmp + num - 1);
//            ret--;
//            tmp--;
//        }
//    }
//    else {
//        while (num--) {
//            *ret = *tmp;
//            ret++;
//            tmp++;
//        }
//    }
//    return dec;
//}
//int main() {
//    int arr1[] = { 1,2,3,4,5,6,7,8,9,10 };
//    my_memmove(arr1 + 2, arr1, 5 * sizeof(int));
//    //1 2 1 2 3 4 5 8 9 10
//    for (int i = 0; i < 10; i++) {
//        printf("%d ", arr1[i]);
//    }
//    return 0;
//}
//�����������ret > tmpʱ��num--��ͬʱret��tmpҲ--������һ�����ƶ������ֽ�
// ͬʱnum�ڽ���whileѭ����ʱ���--��һ�Σ����Բ���Ҫnum-1
// �ʴ���
//�޸����£�
//1.
//void* my_memmove(void* dec, const void* src, size_t num) {
//    assert(dec && src);
//    if (0 == num || src == dec) {
//        return dec;
//    }
//    char* ret = (char*)dec;
//    char* tmp = (char*)src;
//    if (ret > tmp) {
//        while (num--) {
//            *(ret + num) = *(tmp + num);
//        }
//    }
//    else {
//        while (num--) {
//            *ret = *tmp;
//            ret++;
//            tmp++;
//        }
//    }
//    return dec;
//}
//int main() {
//    int arr1[] = { 1,2,3,4,5,6,7,8,9,10 };
//    my_memmove(arr1 + 2, arr1, 5 * sizeof(int));
//    //1 2 1 2 3 4 5 8 9 10
//    for (int i = 0; i < 10; i++) {
//        printf("%d ", arr1[i]);
//    }
//    return 0;
//}
//2.
//void* my_memmove(void* dec, const void* src, size_t num) {
//    assert(dec && src);
//    if (0 == num || src == dec) {
//        return dec;
//    }
//    char* ret = (char*)dec;
//    char* tmp = (char*)src;
//    if (ret > tmp) {
//        ret += num - 1;
//        tmp += num - 1;
//        while (num--) {
//            *ret = *tmp;
//            ret--;
//            tmp--;
//        }
//    }
//    else {
//        while (num--) {
//            *ret = *tmp;
//            ret++;
//            tmp++;
//        }
//    }
//    return dec;
//}
//int main() {
//    int arr1[] = { 1,2,3,4,5,6,7,8,9,10 };
//    my_memmove(arr1 + 2, arr1, 5 * sizeof(int));
//    //1 2 1 2 3 4 5 8 9 10
//    for (int i = 0; i < 10; i++) {
//        printf("%d ", arr1[i]);
//    }
//    return 0;
//}