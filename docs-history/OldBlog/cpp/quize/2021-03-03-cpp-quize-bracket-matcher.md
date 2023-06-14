---
title: "(C++ Quize) Medium : Bracket Matcher"
permalink: cpp/quize/brachet-matcher/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2021-03-03 00:00:00 -0000
last_modified_at: 2021-03-03 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
  - quize
category:
  - brachet-matcher
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

## Q

```
"((hello (world))"
false
```

```
"(hello (world))"
true
```

---

## A

```cpp
#include <iostream>
#include <string>
using namespace std;

int BracketMatcher(string str) {
  
  int len = int(str.length());
  int count = 0;
  for(int i = 0 ; i < len ; i++)
  {
    if(str[i] == '(') count++;
    else if (str[i] == ')') count--;
    
    if(count < 0) return 0;
    if(i == len - 1 && count != 0 )return 0;
  }
  return 1;

}

int main(void) { 
   
  // keep this function call here
  cout << BracketMatcher(gets(stdin));
  return 0;
    
}
```