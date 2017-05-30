#ifndef HASHVI
#define HASHVI

#include<vector>

using namespace std;

class HashVI {  // ハッシュ関数オブジェクト
public:
    int operator()(const vector<int> &x) const {
        const int C = 997;      // 素数
        int t = 0;
        for (int i = 0; i < x.size(); i++) {
            t = t * C + x[i];
        }
        return t;
    }
};


#endif