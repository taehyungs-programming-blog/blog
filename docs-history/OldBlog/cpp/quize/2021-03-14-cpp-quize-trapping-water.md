---
title: "(C++ Quize) Medium : Trapping Water"
permalink: cpp/quize/trapping-water/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2021-03-14 00:00:00 -0000
last_modified_at: 2021-03-14 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
  - quize
category:
  - trapping-water
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

## Q

```
Input: {1, 2, 1, 2}
Output: 1
```

```
Input: {0, 2, 4, 0, 2, 1, 2, 6}
Output: 11
```

---

## A

```cpp
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

int TrappingWater(int arr[], int arrLength) {
  vector<int> left(arrLength, 0);
  vector<int> right(arrLength, 0);
  int temp = arr[0];
  for(int i=1; i < arrLength; i++) {
    left[i] = temp; 
    temp = max(temp, arr[i]);
  }
  temp = arr[arrLength-1];
  for (int i = arrLength - 2; i>=0; i--) {
    right[i] = temp; 
    temp = max(temp, arr[i]);
  }
  int water = 0; 
  for(int i = 1; i < arrLength - 1; i++) {
    int area = min(right[i], left[i]-arr[i]);
    if(area > 0) water += area;
  }
  // code goes here  
  return water;

}

int main(void) { 
   
  // keep this function call here
  int A[] = coderbyteInternalStdinFunction(stdin);
  int arrLength = sizeof(A) / sizeof(*A);
  cout << TrappingWater(A, arrLength);
  return 0;
    
}
```