#pragma once

#include <list>
#include <unordered_map>

//146. LRU ����
//�е�
//��ر�ǩ
//�����ҵ
//������Ʋ�ʵ��һ������  LRU(�������ʹ��) ���� Լ�������ݽṹ��
//ʵ�� LRUCache �ࣺ
//LRUCache(int capacity) �� ������ ��Ϊ���� capacity ��ʼ�� LRU ����
//int get(int key) ����ؼ��� key �����ڻ����У��򷵻عؼ��ֵ�ֵ�����򷵻� - 1 ��
//void put(int key, int value) ����ؼ��� key �Ѿ����ڣ�����������ֵ value ����������ڣ����򻺴��в������ key - value ���������������¹ؼ����������� capacity ����Ӧ�� ��� ���δʹ�õĹؼ��֡�
//���� get �� put ������ O(1) ��ƽ��ʱ�临�Ӷ����С�
//ʾ����
//����
//["LRUCache", "put", "put", "get", "put", "get", "put", "get", "get", "get"]
//[[2], [1, 1], [2, 2], [1], [3, 3], [2], [4, 4], [1], [3], [4]]
//���
//[null, null, null, 1, null, -1, null, -1, 3, 4]
//����
//LRUCache lRUCache = new LRUCache(2);
//lRUCache.put(1, 1); // ������ {1=1}
//lRUCache.put(2, 2); // ������ {1=1, 2=2}
//lRUCache.get(1);    // ���� 1
//lRUCache.put(3, 3); // �ò�����ʹ�ùؼ��� 2 ���ϣ������� {1=1, 3=3}
//lRUCache.get(2);    // ���� -1 (δ�ҵ�)
//lRUCache.put(4, 4); // �ò�����ʹ�ùؼ��� 1 ���ϣ������� {4=4, 3=3}
//lRUCache.get(1);    // ���� -1 (δ�ҵ�)
//lRUCache.get(3);    // ���� 3
//lRUCache.get(4);    // ���� 4
//��ʾ��
//1 <= capacity <= 3000
//0 <= key <= 10000
//0 <= value <= 105
//������ 2 * 105 �� get �� put

class LRUCache {
private:
    using LtIter = std::list<std::pair<int, int>>::iterator;
public:
    LRUCache(int capacity)
        :_capacity(capacity)
    {
    }

    int get(int key) {
        auto ret = _hashMap.find(key);
        if (ret == _hashMap.end()) {
            return -1;
        }
        else {
            // ����key��Ӧֵ��list�е�λ��
            LtIter it = ret->second;
            // 1.erase + push_front : ������ʧЧ
            // 2.splice ת�ƽڵ�
            _LRUList.splice(_LRUList.begin(), _LRUList, it);

            // return ret->second->second;
            return it->second;
        }
    }

    void put(int key, int value) {
        auto ret = _hashMap.find(key);
        if (ret == _hashMap.end()) {
            if (_capacity == _hashMap.size()) {
                std::pair<int, int> back = _LRUList.back();
                _hashMap.erase(back.first);
                _LRUList.pop_back();
            }

            _LRUList.push_front(std::make_pair(key, value));
            _hashMap[key] = _LRUList.begin();
        }
        else {
            LtIter it = ret->second;
            it->second = value;

            _LRUList.splice(_LRUList.begin(), _LRUList, it);
        }
    }

private:
    std::unordered_map<int, LtIter> _hashMap; // ���Ҹ���O(1)
    std::list<std::pair<int, int>> _LRUList; // β������������
    size_t _capacity;
};

/**
 * Your LRUCache object will be instantiated and called as such:
 * LRUCache* obj = new LRUCache(capacity);
 * int param_1 = obj->get(key);
 * obj->put(key,value);
 */