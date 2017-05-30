#ifndef INCIDENCY
#define INCIDENCY

#include<vector>

struct dependency{ // headで表されるカラム集合が決まればtailで表されるカラムが決まるという依存関係を表す構造体
  vector<int> head;
  int tail;
};

#endif