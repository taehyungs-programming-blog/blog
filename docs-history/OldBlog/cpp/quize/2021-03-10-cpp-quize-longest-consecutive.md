---
title: "(C++ Quize) Medium : Longest Consecutive"
permalink: cpp/quize/longest-consecutive/                # link 직접 지정
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
  - longest-consecutive
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

## Q

```
Input: {6, 7, 3, 1, 100, 102, 6, 12}
Output: 2
```

```
Input: {5, 6, 1, 2, 8, 9, 7}
Output: 5
```

---

## A

```cpp
#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

int LongestConsecutive(int arr[], unsigned int len) {
  
  // code goes here  
  int max = 1, lcsSize = 1;
  sort(arr, arr+len);
  for(unsigned int i = 1; i < len; i++) {
    if(arr[i-1] + 1 == arr[i])
      lcsSize++;
    else if(arr[i-1] != arr[i]) {
      if(lcsSize > max)
        max = lcsSize;
      lcsSize = 1;
    }
  }
  if(lcsSize > max)
    max = lcsSize;
  return max;

}

int main(void) { 
   
  // keep this function call here
  /* Note: In C++ you first have to initialize an array and set 
     it equal to the stdin to test your code with arrays. */

  int A[] = gets(stdin);
  cout << LongestConsecutive(A, sizeof(A)/sizeof(*A));
  return 0;
    
}
```