---
title: "STL bitset"
permalink: cpp/stl/bitset/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-01 00:00:00 -0000
last_modified_at: 2020-04-01 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

```cpp
#include <iostream>
#include <bitset>
#include <string>
using namespace std;

int main()
{
    bitset<8> b1 = 0xf0;            // 11110000
    bitset<8> b = 0b11110000;       // 11110000

    cout << b1 << endl;

    string s1 = b1.to_string();     
    unsigned long n = b1.to_ulong();
}
```

```cpp
b1.set();       // 모두 1으로
b1.reset();     // 모두 0으로
b1.set(1);      // 0000 0010
b1[2] = 1;      // 0000 0110
b1[0].flip();   // 0000 0111
```

```cpp
if(b1.test(1) == true)
if(b1[1] == true)
if(b1.none() == true)
```