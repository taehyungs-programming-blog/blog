---
title: "(C++ Quize) Medium : Formatted Number"
permalink: cpp/quize/formatted-number/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2021-03-13 00:00:00 -0000
last_modified_at: 2021-03-13 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
  - quize
category:
  - formatted-number
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

## Q

```
Input: {"0.232567"}
Output: true
```

```
Input: {"2,567.00.2"}
Output: false
```

---

## A

```cpp
#include <iostream>
#include <string>
using namespace std;

string FormattedNumber(string strArr[], int arrLength) {
  
  // code goes here  
  string str = strArr[0];
  size_t pos = str.find_last_of(".");
  if(pos!=string::npos)
  {
    for(size_t i=0;i<str.find_last_of(".");i++)
    {
      if(str[i]==',')
      {
        str.erase(i,1);
      }
      else if(isdigit(str[i])==0)
      {
        return "false";
      }
    }
  }
  pos = str.find_last_of(".");

  for(int i=pos-3;i>0;i-=3)
  {
    str.insert(i,",");
  }
 
  if(str!=strArr[0]) return "false";
  
  return "true";

}

int main(void) { 
   
  // keep this function call here
  string A[] = coderbyteInternalStdinFunction(stdin);
  int arrLength = sizeof(A) / sizeof(*A);
  cout << FormattedNumber(A, arrLength);
  return 0;
    
}
```