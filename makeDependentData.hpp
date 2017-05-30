// 任意の依存関係を持つテーブルを作る関数

#ifndef MAKEDT
#define MAKEDT

#include<random>
#include<iostream>
#include<vector>
#include "dependency.hpp"

vector< vector <int> >* setValue(int, vector<dependency>&, vector< vector <int> >&, int, vector<bool>&);
void viewDependency(vector<dependency>&, const vector<string>&);

table* makeDependentTable(vector<dependency>& inc, string name, const vector<string>& cn, int rows){ // incで表される依存関係を持ったテーブルを作る
  cout << "Making a table, which has designated dependency:" << endl;
  viewDependency(inc, cn);
  cout << endl;
  vector< vector <int> > data(rows, vector<int>(cn.size(), 0));
  vector <bool> finished(cn.size(), false);
  for(int i = 0; i < inc.size(); i++){
    if(!finished[i]){ // 第iカラムの数値格納が終わっていない場合
      vector< vector <int> >* s = setValue(i, inc, data, rows, finished); // 数値格納を行う
      if(s == nullptr) return nullptr; // 異常終了
    }
  }
  table* ret = new table(data, name, cn);
  return ret;
}

void viewDependency(vector<dependency>& inc, const vector<string>& cn){
  for(auto itr = inc.begin(); itr != inc.end(); itr++){
    string headString;
    string tailString;
    if((*itr).head.size() > 0) headString += "{";
    for(int i = 0; i < (*itr).head.size(); i++){
      headString += cn[(*itr).head[i]] + ", ";
    }
    headString.erase(headString.end() - 2, headString.end());
    headString += "}";
    tailString += "{" + cn[(*itr).tail] + "}";
    cout << headString << " → " << tailString << endl;
  }
}

vector< vector <int> >* setValue(int n, vector<dependency>& inc, vector< vector <int> >& data, int rows, vector<bool>& finished){
  random_device rnd;
  mt19937 mt(rnd());
  uniform_int_distribution<> rand(0, pow(2, 10 + data.size() / 10) - 1); // 格納する値のバリエーション数はレコード数の指数オーダとする
                                                                          // 意図しないデータ重複の検出機能がなく、ドメインに十分数の要素が必要なため
  vector <int> dependentTo(0); // 依存先を格納するvector
  for(auto itr = inc.begin(); itr != inc.end(); itr++){ // 依存先の数値の格納が終わっているかチェック
    if(itr -> tail == n){
      if(dependentTo.size() > 0){
        cout << "Caution: Duplicated definition of dependency." << endl;
        return nullptr;
      }
      dependentTo = itr -> head;
      for(int i = 0; i < itr -> head.size(); i++){
        if(!finished[itr -> head[i]]){ // 終わっていなかった場合、
          setValue(itr -> head[i], inc, data, rows, finished); // 先に依存先の数値を決める必要があるので、そちらの格納を優先する
        }
      }
    }
  }
  if(dependentTo.size() == 0){ // 依存先がない場合、乱数を格納
    for(int i = 0; i < rows; i++){
      if(i == n + 1){ // 意図しない依存関係が存在しないよう、第n+1行において重複をつくる
        data[i][n] = data[0][n];
      }
      else{
        data[i][n] = rand(mt);
      }
    }
  }
  else{ // 依存先がある場合、それらの値の和を格納
    int b = 0; // 切片。任意の整数を入れてよいが、ここでは関数従属性のわかりやすさのため0としている
    for(int i = 0; i < rows; i++){
      if(i == n + 1){ // 意図しない依存関係が存在しないよう、第n+1行において重複をつくる
        data[i][n] = data[0][n];
      }
      else{
        for(auto itr = dependentTo.begin(); itr != dependentTo.end(); itr++){
          data[i][n] += data[i][*itr] + b;
        }
      }
    }
  }
  finished[n] = true;
  return &data;
}

#endif