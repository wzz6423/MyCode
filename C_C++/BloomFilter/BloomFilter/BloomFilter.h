#pragma once
//#include "HashFunc.h"
#include <bitset>
#include <string>
using namespace std;

namespace wzz {
    struct BKDRHash{
        size_t operator()(const string& str){
            size_t hash = 0;
            for (auto ch : str){
                hash = hash * 131 + ch;
            }
            return hash;
        }
    };

    struct APHash{
        size_t operator()(const string& str){
            size_t hash = 0;
            for (size_t i = 0; i < str.size(); i++){
                size_t ch = str[i];
                if ((i & 1) == 0){
                    hash ^= ((hash << 7) ^ ch ^ (hash >> 3));
                }
                else{
                    hash ^= (~((hash << 11) ^ ch ^ (hash >> 5)));
                }
            }
            return hash;
        }
    };

    struct DJBHash{
        size_t operator()(const string& str){
            size_t hash = 5381;
            for (auto ch : str){
                hash += (hash << 5) + ch;
            }
            return hash;
        }
    };

	template <size_t N,
		typename K = string>
	class BloomFilter {
	public:
		void set(const K& key) {
			size_t hashi1 = BKDRHash()(key) % N;
			_bs.set(hashi1);

			size_t hashi2 = APHash()(key) % N;
			_bs.set(hashi2);

			size_t hashi3 = DJBHash()(key) % N;
			_bs.set(hashi3);
		}

		bool test(const K& key) {
			size_t hashi1 = BKDRHash()(key) % N;
			if (_bs.test(hashi1) == false) {
				return false;
			}

			size_t hashi2 = APHash()(key) % N;
			if (_bs.test(hashi2) == false) {
				return false;
			}

			size_t hashi3 = DJBHash()(key) % N;
			if (_bs.test(hashi3) == false) {
				return false;
			}
			return true;
		}
	private:
		bitset<N> _bs;
	};
}