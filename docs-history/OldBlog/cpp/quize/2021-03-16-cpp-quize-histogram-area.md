---
title: "(C++ Quize) Medium : Histogram Area"
permalink: cpp/quize/histogram-area/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2021-03-16 00:00:00 -0000
last_modified_at: 2021-03-16 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
  - quize
category:
  - histogram-area
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

## Q

```
Input: {6, 3, 1, 4, 12, 4}
Output: 12
```

```
Input: {5, 6, 7, 4, 1}
Output: 16
```

---

## A

```cpp
#include <iostream>
#include <string>
using namespace std;

int HistogramArea(int arr[], int arrLength) {

  int max_area = 0;
  for (int i = 0; i < arrLength; i++) {
    int height = arr[i];
    for (int j = i; j < arrLength; j++) {
      const int width = j - i + 1;
      height = std::min(height, arr[j]);
      max_area = std::max(max_area, height * width);
    }
  }

  return max_area;
  
}

int main(void) { 
   
  // keep this function call here
  /* Note: In C++ you first have to initialize an array and set 
     it equal to the stdin to test your code with arrays. */

  int A[] = coderbyteInternalStdinFunction(stdin);
  int arrLength = sizeof(A) / sizeof(*A);
  cout << HistogramArea(A, arrLength);
  return 0;
    
}
```