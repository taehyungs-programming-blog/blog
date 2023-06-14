---
title: "(C++ Quize) Medium : Nearest Smaller Values"
permalink: cpp/quize/nearest-smaller-values/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2021-03-21 00:00:00 -0000
last_modified_at: 2021-03-21 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
  - quize
category:
  - nearest-smaller-values
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

## Q

```
Input: {5, 3, 1, 9, 7, 3, 4, 1}
Output: -1 -1 -1 1 1 1 3 1
```

```
Input: {2, 4, 5, 1, 7}
Output: -1 2 4 -1 1
```

## A

```cpp
#include <iostream>
#include <string>
using namespace std;

std::string NearestSmallerValues(int arr[], int arrLength) {
  std::string solution = "-1";

  for (int i = 1; i < arrLength; ++i) {
    std::string num = " -1";
    for (int j = i-1; j >=0 ; --j) {
      if (arr[j] <= arr[i]) {
        num = " " + std::to_string(arr[j]);
        break;
      }
    }
    solution += num;
  }
  return solution;

}

int main(void) { 
   
  // keep this function call here
  int A[] = coderbyteInternalStdinFunction(stdin);
  int arrLength = sizeof(A) / sizeof(*A);
  cout << NearestSmallerValues(A, arrLength);
  return 0;
    
}
```