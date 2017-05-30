#include "table.hpp"
#include "makeDependentData.hpp"
#include <vector>
#include <cmath>
using namespace std;

const vector<string> NAMES{"Alice", "Bob", "Cameron", "David"};

int main(){
  // 正規化対象のテーブルを作るため、関数従属性を定義する
  vector<dependency> inc;
  vector<int> head1 = {2, 3};
  int tail1 = 1;
  vector<int> head2 = {3};
  int tail2 = 0;
  inc.push_back(dependency({head1, tail1}));
  inc.push_back(dependency({head2, tail2}));
  table* testTable = makeDependentTable(inc, "Test Table", NAMES, NAMES.size() + 1);
  testTable -> view();
  vector<table*>* decomposedTestTables = testTable -> secondNormalized();
  delete decomposedTestTables;
}