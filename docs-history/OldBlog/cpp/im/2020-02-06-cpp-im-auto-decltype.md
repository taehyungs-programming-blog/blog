---
title: "(C++) auto / decltype 정리"
date: 2020-02-06 00:00:00 -0000
---

```cpp
#include <iostream>
#include <typeinfo>
#include <vector>
using namespace std;

int main()
{
  // 배열
  int x[3] = { 1, 2, 3 };   // x : int[3]
  
  auto a1 = x;        // int a1[3] = x; - error, 따라서 배열의 첫 번째 요소 주소를 넣어준다.
  auto& a2 = x;       // int (&a2)[3] = x; - ok
                      // a2 : int (&)[3]
  
  decltype(x) d;      // int [3]
  cout << typeid(a1).name() << endl;   // int*
  cout << typeid(a2).name() << endl;   // int(&)[3]
  cout << typeid(d).name() << endl;   // int [3]
  
  auto a3 = 1;    // int
  auto a4{1};     // int
  auto a5 = {1};  // initializer_list
  
  vector<int> v1(10, 0);
  auto a6 = v1[0];      // int
  
  vector<bool> v2(10, 0);
  auto a7 = v2[0];      // _Bit_reference?? -> temporary proxy를 알아야 됨. 일단은 받아들이자.
}
```