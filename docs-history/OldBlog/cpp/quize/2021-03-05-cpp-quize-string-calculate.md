---
title: "(C++ Quize) Medium : String Calculate"
permalink: cpp/quize/string-calculate/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2021-03-04 00:00:00 -0000
last_modified_at: 2021-03-04 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
  - quize
category:
  - string-calculate
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

## Q

```
Input: "6*(4/2)+3*1"
Output: 15
```

```
Input: "100*2**4"
Output: 1600

```

---

## A

```cpp
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <cmath>
using namespace std;

double eval(string str)
{
  if(str.find('+')==string::npos&&str.find('-',1)==string::npos&&
     str.find('*')==string::npos&&str.find('/')==string::npos)
  {
      // 연산기호가 더 이상 없다면 숫자를 리턴
    return strtol(str.c_str(),NULL,10);
  }
  int parFerm;
  // 괄호를 모두 제거
  while((parFerm=str.find(')'))!=string::npos)
  {
    int parOuvr;
    for(parOuvr=parFerm-2;str[parOuvr]!='(';parOuvr--);
    stringstream ss;
    ss<<eval(str.substr(parOuvr+1,parFerm-parOuvr-1));
    str.replace(parOuvr,parFerm-parOuvr+1,ss.str());
  }
  for(int i=1;i<str.length();i++)
  {
    if(str[i]=='-'&&                    // 마이너스 기호 뒤에는 +를 넣는다
        '0'<=str[i-1]&&str[i-1]<='9')
    {
      str.insert(i++,"+");
    }
  }
  int plusPos;
  if((plusPos=str.find('+'))!=string::npos)
  {
    return eval(str.substr(0,plusPos))+eval(str.substr(plusPos+1));
  }
  int multPos;
  if((multPos=str.find('*'))!=string::npos&&str[multPos+1]!='*')
  {
    return eval(str.substr(0,multPos))*eval(str.substr(multPos+1));
  }
  int divPos;
  if((divPos=str.find('/'))!=string::npos)
  {
    for(divPos=str.length()-2;str[divPos]!='/';divPos--);
    return eval(str.substr(0,divPos))/eval(str.substr(divPos+1));
  }
  int powPos=str.find("**");
  return pow(eval(str.substr(0,powPos)),eval(str.substr(powPos+2)));
}

int StringCalculate(string str) { 

  // code goes here   
  for(int i=1;i<str.length();i++)
  {
    if(str[i]=='('      // '('로 시작하고
        &&('0'<=str[i-1]&&str[i-1]<='9' // 직전이 숫자이며
            ||str[i-1]==')'))   // 직전이 ')' 로 끝난다면
    {
      str.insert(i++,"*");      // *을 넣자
    }
  }
  return (int)eval(str); 
            
}

int main() { 
  
  // keep this function call here
  cout << StringCalculate(gets(stdin));
  return 0;
    
}
```