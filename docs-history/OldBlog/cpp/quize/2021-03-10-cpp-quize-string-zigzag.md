---
title: "(C++ Quize) Medium : String Zigzag"
permalink: cpp/quize/string-zigzag/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2021-03-10 00:00:00 -0000
last_modified_at: 2021-03-10 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
  - quize
category:
  - string-zigzag
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

## Q

```
Input: {"cat", "5"}
Output: cat
```

```
Input: {"kaamvjjfl", "4"}
Output: kjajfavlm
```

---

## A

```cpp
#include <iostream>
#include <string>
#include <vector>
using namespace std;

vector<string> Zigzag(string str,int num,string &ret)
{
  vector<string> split;
  vector<string> v;
  if(num==1)  // 1단만 남았다면 리턴
  {
    ret += str;
    return v;
  }
   
  if(num<1) return v;

  int interval = 2*(num-1);
  /*
  Input: {"kaamvjjfl", "4"}
  Output: kjajfavlm
  k      j
   a    j f
    a  v   l
     m
  하나의 v자를 interval이라 정의
  */
  size_t p = 0;
  while(p<str.length())
  {
    string sub = str.substr(p,interval);  
    // 첫 루프 :  6글자 "kaamvjj"을 넣는다
    // 두 번째 루프 : 남은 글자를 넣는다. "jfl"

    split.push_back(sub);
    if(p<str.length())
      p += interval;
  }
 

  for(size_t i=0;i<split.size();i++)
  {
    ret += split[i][0];   // 첫 번째 라인인 k j 가 들어간다.
  }

  for(size_t i=0;i<split.size();i++)
  {
    v.push_back(split[i].substr(1,split[i].length()-1));
    // 남은 글자를 리턴으로 넘긴다
    // aamvjj, fl 이 넘어갈 예정
  }

  return v;
}

string StringZigzag(string strArr[], int arrLength) {
  
  // code goes here  
  string str = strArr[0];
  vector<string> Input;
  Input.push_back(str);
  int num = stoi(strArr[1]);
  string ret;
  while(num>0)
  {
    vector<string> Output;
    for(size_t i=0;i<Input.size();i++)
    {
      vector<string> z = Zigzag(Input[i],num,ret);
      for(size_t j=0;j<z.size();j++)
      {
        Output.push_back(z[j]);
      }   
    }
    Input.clear();
    for(size_t j=0;j<Output.size();j++)
    {
      Input.push_back(Output[j]);
    }  
    num--;
  }
  
  return ret;

}

int main(void) { 
   
  // keep this function call here
  string A[] = coderbyteInternalStdinFunction(stdin);
  int arrLength = sizeof(A) / sizeof(*A);
  cout << StringZigzag(A, arrLength);
  return 0;
    
}
```