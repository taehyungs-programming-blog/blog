---
title: "(C++ Quize) Medium : Plus Minus"
permalink: cpp/quize/plus-minus/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2021-03-07 00:00:00 -0000
last_modified_at: 2021-03-07 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
  - quize
category:
  - plus-minus
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

## Q

```
Input: 199
Output: not possible
```

```
Input: 26712
Output: -+--
```

---

## A

```cpp
#include <iostream>
#include <string>
using namespace std;

void helper(int sum, string &source, string &res, string temp, int minus, int &max) {
  if(temp.size()==source.size()-1) {        // 모든 기호가 다 들어감.
    if(sum == 0 &&          // 모든 합이 0이고
        minus > max) {      // 최대 마이너스 개수를 체크(두 번째 조건임)
      max = minus; 
      res = temp;
    }
    return;
  }

  // 최대 -가 우선되기에 -를 먼저 검사
  temp.push_back('-');         
  int sum1 = sum - (source[temp.size()] - '0');
  helper(sum1, source, res, temp, minus+1, max);
  temp.pop_back();

  temp.push_back('+');
  int sum2 = sum + (source[temp.size()] - '0');
  helper(sum2, source, res, temp, minus+1, max);
  return; 
}

/*
145라 가정 분기가 된다 생각하면 좀 쉽다

''  -> '-'  -> '--'  --> 이걸 검사 후 조건에 맞으면 max, res에 값을 갱신한다
            -> '-+'
    -> '+'  -> '+-'
            -> '++'
*/

string PlusMinus(int num) {
  string no = "not possible";
  string source = to_string(num);
  if(source.size() == 1) return no; 
  if(source.empty()) return no;
  string res = "";
  string temp = "";
  int sum = source[0] - '0';
  int max = 0; 
  helper(sum, source, res, temp, 0, max);
  if (res == "") return no;
  else return res; 
}

int main(void) { 
   
  // keep this function call here
  cout << PlusMinus(coderbyteInternalStdinFunction(stdin));
  return 0;
    
}
```