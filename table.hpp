#ifndef INCLUDED_IO_CSV
#define INCLUDED_IO_CSV

#include<string>
#include<cmath>
#include<algorithm>
#include<vector>
#include<iostream>
#include<iomanip>
#include<stdlib.h>
#include<unordered_map>
#include "hashVI.hpp"
#include "dependency.hpp"

using namespace std;

class table{
  string tableName;
  vector<string> columnName;
  vector< unordered_map <string, int> > data;
public:
  table(string n, vector<string> cn) : tableName(n), columnName(cn), data(0){
    makeDistinguishedFirstRow(cn);
  }
  table(vector<int> d, string n, vector<string> cn) : tableName(n), columnName(cn), data(0){
    makeDistinguishedFirstRow(cn);
    insert(d);
  }
  table(vector< vector<int> > d, string n, vector<string> cn) : tableName(n), columnName(cn), data(0){
    makeDistinguishedFirstRow(cn);
    insert(d);
  }
  void makeDistinguishedFirstRow(vector<string> cn){ // カラムが存在するかの判定に用いる、dataの第１行をつくる関数
                                                      // 第１行の値は常にすべて1（存在しないkeyで検索した時の返り値0と区別できる値として必要）
                                                      // データの利用（関数従属性判定など）時に第１行を無視するよう、実装上注意すること
    unordered_map <string, int> tmp;
    data.push_back(tmp);
    for(auto itr = cn.begin(); itr != cn.end(); itr++){
      data[0][*itr] = 1;
    }
  }
  void insert(vector<int> d){ // タプルの挿入を行う関数
    if(d.size() != columnName.size()){
      cout << "Caution(" << tableName << "):Tupple with invalid cardinality was input." << endl;
      return;
    }
    unordered_map <string, int> tmp;
    data.push_back(tmp);
    for(int i = 0; i < columnName.size(); i++){
      data.back()[columnName[i]] = d[i];
    }
  }
  void insert(vector< vector <int> > d){ // タプルの集合の挿入を行う関数
    for(int i = 0; i < d.size(); i++){
      if(d[i].size() != columnName.size()){
        cout << "Caution(" << tableName << "):Tupple with various cardinality was input." << endl;
        return;
      }
    }
    for(int i = 0; i < d.size(); i++){
      unordered_map <string, int> tmp;
      data.push_back(tmp);
      for(int j = 0; j < columnName.size(); j++){
        data.back()[columnName[j]] = d[i][j];
      }
    }
  }
  void view(){
    cout << "--------" << tableName << "--------" << endl;
    if(data.size() == 1){
      cout << "Empty." << endl;
    }
    cout << "rows: " << data.size() - 1 << endl;
    cout << "columns: " << columnName.size() << endl << endl;

    for(int j = 0; j < columnName.size(); j++){
      cout << setw(max((int)columnName[j].size() + 1, 5)) << columnName[j];
    }
    cout << endl;
    for(int i = 1; i < data.size(); i++){
      for(int j = 0; j < columnName.size(); j++){
        cout << setw(max((int)columnName[j].size() + 1, 5)) << setprecision(3) << data[i][columnName[j]];
      }
      cout << endl;
    }
    cout << endl;
  }

  table* select(vector<string> keys, bool distinct = 0) { // カラム名でSELECT
                                                          // デフォルトはDISTINCTでない
    if(data.size() == 0){
      cout << "Caution: " << tableName << " is empty." << endl;
      return nullptr;
    }
    unordered_map <vector<int>, bool, HashVI> covered;
    for(auto itr = keys.begin(); itr != keys.end(); itr++){
      if(data[0][*itr] == 0){ // 第１行はmakeDistinguishedFirstRow関数によりすべて値が1なので（存在識別用）、
                              // デフォルト値の0が返った場合は存在しないkeyである
        cout << "Caution: " << *itr << " was not found." << endl;
        return nullptr;
      }
    }

    string name = tableName + "-";
    for(auto itr = keys.begin(); itr != keys.end(); itr++){
      name += (*itr)[0]; // "db4"の"Alice", "Bob"カラムが指定された場合、
                          // "db4-AB"が生成される
    }

    table* ret = new table(name, keys);
    vector< vector<int> > selectedData(0);
    if(data.size() > 1){
      for(int i = 1; i < data.size(); i++){ // 第１行は存在識別用の値が格納されているので無視
        vector<int> tmp(0);
        selectedData.push_back(tmp);
        for(int j = 0; j < keys.size(); j++){
          selectedData.back().push_back(data[i][keys[j]]);
        }
        if(distinct && covered[selectedData.back()] == 1){ // selectedData[i]が既出でないならば
          selectedData.pop_back(); // 挿入したデータを削除（重複しているので）
        }
        covered[selectedData.back()] = true;
      }
      (*ret).insert(selectedData);
      return ret;
    }
    return nullptr;
  }
  vector< vector<bool> >* detectCandidateKeys
    (vector<bool> flag = vector<bool>(0), int depth = 0, vector< vector<bool> >* ckey = nullptr){ // 二分木を使ってすべての候補キーを検出する
    if(depth == 0){
      ckey = new vector< vector<bool> >(0);
      if(flag.size() != 0){ // フラグを初期化
        vector<bool> tmp(0);
        flag = tmp;
      }
    }
    bool contain = false; // 既に検出された候補キーのいずれかをを含むか
    if((*ckey).size() > 0){ // flagによって表されるカラム集合は候補キーに含まれるか？
      for(auto itr = (*ckey).begin(); itr != (*ckey).end(); itr++){ //それぞれの候補キーについて
        for(int i = 0; i < (*itr).size(); i++){
        }
        contain = true;
        for(int i = 0; i < depth && contain; i++){
          if(!flag[i] && (*itr)[i]){ // flagが1で、候補キーに含まれない要素があるなら、その候補キーには含まれていない
            contain = false;
          }
        }
        for(int i = depth; i < columnName.size(); i++){
          if((*itr)[i]){ // flagが*で、候補キーに含まれる要素があるなら、その候補キーを含まない要素が子ノードに少なくとも１つ存在
            contain = false;
          }
        }
        if(contain){ // カラム集合を含む候補キーを検出したら、その時点でbreak
          break;
        }
      }
    }
    if(!contain){ // いかなる既出候補キーにも含まれないならば枝刈りせず進む
      if(depth < columnName.size()){ // 葉ノードでない
        flag.push_back(0);
        detectCandidateKeys(flag, depth + 1, ckey);
        flag.back() = 1;
        detectCandidateKeys(flag, depth + 1, ckey);
      }
      else{ // 葉ノード
        vector<string> keys;
        for(int i = 0; i < columnName.size(); i++){
          if(flag[i]) keys.push_back(columnName[i]);
        }
        table* selectedTable = select(keys, true); // keysについてDISTINCT指定のSELECT
        if(data.size() == selectedTable->data.size()){ // flagは候補キーを表す
          vector<bool> tmp(0);
          (*ckey).push_back(tmp);
          (*ckey).back() = flag;
        }
      }
    }
    if(depth == 0){
      cout << tableName << "'s candidate keys:" << endl;
      viewCandidateKeys(ckey, *this);
      cout << "." << endl << endl;
    }
    return ckey;
  }

  vector<table*>* secondNormalized(){ // 第２正規化されたテーブルの集合を返す関数
    vector< vector <bool> >* ckeys = detectCandidateKeys();
    // 任意の候補キーの、任意の真部分集合について、関数従属する非キー属性がないか調べる
    vector<dependency>* partialFunctionalDependency;
    for(auto itr = (*ckeys).begin(); itr != (*ckeys).end(); itr++){
      partialFunctionalDependency = detectPartialFunctionalDependency(*itr);
      if(partialFunctionalDependency->size() > 0){ // 1つ以上の部分関数従属が見つかったら
        break; // 一旦break
      }
    }
    vector<string> body = columnName; // 正規化後のテーブル
    vector< vector<string> > sattelite(0); // 関数従属している部分をSELECTしたテーブル
    int erased = 0;
    if(partialFunctionalDependency->size() == 0){ // 関数従属性が検出されなかったら
      vector<table*>* ret = new vector<table*>(1, this); // 自身のみを格納した返り値を用意し、返す
      return ret;
    }
    for(auto itr = partialFunctionalDependency->begin(); itr != partialFunctionalDependency->end(); itr++){
      vector<string> tmp(0);
      sattelite.push_back(tmp);
      for(int i = 0; i < (*itr).head.size(); i++){
        sattelite.back().push_back(columnName[(*itr).head[i]]);
      }
      body.erase(body.begin() + (*itr).tail - erased++);
      sattelite.back().push_back(columnName[(*itr).tail]);
    }
    vector<table*>* ret; // 正規化されたテーブル群を格納する返り値
    ret = select(body, true)->secondNormalized(); // まずbodyを正規化したものを格納;
    for(int i = 0; i < partialFunctionalDependency->size(); i++){
      ret->push_back(select(sattelite[i], true));
    }

    cout << "Normarized " << tableName << " (2NF)." << endl;
    for(auto itr = ret->begin(); itr != ret->end(); itr++){
      (*itr) -> view();
    }
    return ret;
  }

  vector< dependency >* detectPartialFunctionalDependency(vector<bool>& ckey, vector<bool> flag = vector<bool>(0), int depth = 0, vector<dependency>* partialFunctionalDependency = nullptr){ // 二分木を使ってすべての候補キーを検出する
    // 候補キーckeyについて、二分木を用いて、部分関数従属性を検出する
    if(depth == 0){
      partialFunctionalDependency = new vector< dependency >(0);
      if(flag.size() != 0){ // フラグを初期化
        vector<bool> tmp(0);
        flag = tmp;
      }
    }
    vector<int> ckeyColumnNumber(0); // ckey == 01101001 のとき、ckeyColumnNumber == {1, 2, 4, 7}となる
    unordered_map<int, bool> isInCKey; // ckey == 01101001 のとき、
                                          // isInCKey[0 or 3 or 5 or 6] == false,
                                          // isInCKey[1 or 2 or 4 or 7] == true.
    for(int i = 0; i < ckey.size(); i++){
      if(ckey[i]){
        ckeyColumnNumber.push_back(i);
        isInCKey[i] = true;
      }
    }
    bool contain = false; // 既に検出された候補キーのいずれかをを含むか
    if((*partialFunctionalDependency).size() > 0){ // flagによって表されるカラム集合は候補キーに含まれるか？
      for(auto itr = (*partialFunctionalDependency).begin(); itr != (*partialFunctionalDependency).end(); itr++){ //それぞれの関数従属性の独立要素について
        unordered_map<int, bool> isInPFD;
        string nodeName = "";
        string headOfPFD = "";
        nodeName += "{";
        headOfPFD += "{";
        for(int i = 0; i < flag.size(); i++){
          if(flag[i]) nodeName += columnName[ckeyColumnNumber[i]] + ", ";
        }
        for(int i = 0; i < (*itr).head.size(); i++){
          headOfPFD += columnName[(*itr).head[i]] + ", ";
          isInPFD[(*itr).head[i]] = true;
        }
        if(nodeName.size() > 2){
          nodeName.erase(nodeName.end() - 2, nodeName.end());
        }
        if(headOfPFD.size() > 2){
          headOfPFD.erase(headOfPFD.end() - 2, headOfPFD.end());
        }
        nodeName += "}";
        headOfPFD += "}";
        contain = true;
        for(int i = 0; i < depth && contain; i++){
          if(!flag[i] && isInPFD[ckeyColumnNumber[i]]){
            contain = false;
          }
        }
        for(int i = depth; i < ckeyColumnNumber.size(); i++){
          if(isInPFD[ckeyColumnNumber[i]]){ // flagが*で、関数従属性の独立要素に含まれる要素があるなら、その関数従属性の独立要素を含まない要素が子ノードに少なくとも１つ存在
            contain = false;
          }
        }
        if(contain){ // カラム集合が含んでいる関数従属性の独立要素を検出したら、その時点でbreak
          break;
        }
      }
    }
    if(!contain){ // いかなる既出関数従属性の独立要素にも含まれないならば枝刈りせず進む
      if(depth < ckeyColumnNumber.size()){ // 葉ノードでない
        flag.push_back(0);
        detectPartialFunctionalDependency(ckey, flag, depth + 1, partialFunctionalDependency);
        flag.back() = 1;
        detectPartialFunctionalDependency(ckey, flag, depth + 1, partialFunctionalDependency);
      }
      else{ // 葉ノード
        // flagに対応するカラム集合及び、任意の１つの非キー属性についてSELECTし、
        // カラム集合の値に非キー属性の値が従属しているかを確認する
        vector<int> subsetOfCKey(0);// この葉ノードで見る、候補キーckeyの真部分集合
        string subsetOfCKeyString = ""; // subsetOfCKeyのカラム名
        subsetOfCKeyString += "{";
        for(int i = 0; i < ckeyColumnNumber.size(); i++){
          if(flag[i]){
            subsetOfCKey.push_back(ckeyColumnNumber[i]);
            subsetOfCKeyString += columnName[ckeyColumnNumber[i]] + ", ";
          }
        }
        if(subsetOfCKeyString.size() > 2){
          subsetOfCKeyString.erase(subsetOfCKeyString.end() - 2, subsetOfCKeyString.end());
        }
        subsetOfCKeyString += "}";
        if(subsetOfCKey.size() == 0 || subsetOfCKey.size() == ckeyColumnNumber.size()){ // trivialな部分集合なら
          return partialFunctionalDependency; // 確認は不要
        }
        for(int j = 0; j < columnName.size(); j++){ // 候補キーに含まれないカラムが、subsetOfCKeyに関数従属するか
          if(isInCKey[j]) break; // 候補キーに含まれるカラムは無視
          unordered_map<vector<int>, int, HashVI> function; // ノードで表されるカラム集合が、非キー属性を決める「関数」
          for(int i = 0; i < data.size(); i++){
            vector<int> selectedDataWithSubsetOfCKey(0);
            for(auto itr = subsetOfCKey.begin(); itr != subsetOfCKey.end(); itr++){
              selectedDataWithSubsetOfCKey.push_back(data[i][columnName[*itr]]);
            }
            if(function[selectedDataWithSubsetOfCKey] == 0){ // まだデータが既出で無いなら
              function[selectedDataWithSubsetOfCKey] = data[i][columnName[j]]; // データを格納
            }
            else if(function[selectedDataWithSubsetOfCKey] != data[i][columnName[j]]){ // 過去のデータと一致しないなら
              break; // 関数従属していないので、break
            }
            if(i == data.size() - 1){ // 最後のタプルまできたら関数従属している
              dependency tmp = {subsetOfCKey, j}; // カラムjはカラム集合subsetOfCkeyに関数従属
              partialFunctionalDependency->push_back(tmp);
            }
          }
        }
      }
    }
    return partialFunctionalDependency;
  }

  friend void viewCandidateKeys(vector< vector <bool> >*, table&);
};

void view(vector<table*>& dbs){
  for(auto itr = dbs.begin(); itr != dbs.end(); itr++){
    (*itr)->view();
  }
  return;
}

void viewCandidateKeys(vector< vector <bool> >* result, table& t){
  //cout << (*result).size() << " candidate keys have been detected." << endl;
  string ckey;
  for(int i = 0; i < (*result).size(); i++){
    ckey += "{";
    for(int j = 0; j < (*result)[i].size(); j++){
      if((*result)[i][j]){
        ckey += t.columnName[j] + ", ";
      }
    }
    ckey.erase(ckey.end() - 2, ckey.end());
    ckey += "}";
  }
  cout << ckey;
}

#endif