---
title: "(C++ Quize) Medium : Food Distribution"
permalink: cpp/quize/food-distribution/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2021-03-15 00:00:00 -0000
last_modified_at: 2021-03-15 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
  - quize
category:
  - food-distribution
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

## Q

```
Input: {5, 2, 3, 4, 5}
Output: 1
```

```
Input: {3, 2, 1, 0, 4, 1, 0}
Output: 4
```

---

## A

```cpp
#include <iostream>
#include <cmath>

using namespace std;

unsigned int CalTotDiff (int arr[], int arrLength)
{
  unsigned int totDiff = 0;
  for(unsigned int i=1; i<arrLength-1; i++)
  {
    totDiff += (abs(arr[i]-arr[i+1]));
  }
  return totDiff;
}

void FindMinDiff (int arr[], int arrLength, int N, unsigned int& totDiff)
{
    // 처음은 샌드위지를 하나도 나눠주지 않고 토탈 distance를 계산
    unsigned int totDiff2 = CalTotDiff(arr,arrLength);
    if(totDiff2<totDiff)
    {
        if(!(totDiff=totDiff2)) // total Distance가 0이면 정답이니 리턴
            return;
    }
    if(N>0)
    {
        for(unsigned int i=1; i<arrLength; i++)
        {
            if(arr[i]>0)
            {
              arr[i] -= 1;
              FindMinDiff(arr, arrLength, N-1, totDiff);
              arr[i] += 1;
            }
        }
    }
    /*
    [5, 3, 1, 2, 1]
    샌드위치 5개
    [3, 1, 2, 1] -> (샌드위치 1개 나눠준다) [2, 1, 2, 1] -> (샌드위치 2개 나눠준다) [1, 1, 2, 1]
                                          [3, 0, 2, 1]                           ... 이렇게 모든 루프를 돌면서 최소 Distance를 찾게된다.
                                          [3, 1, 1, 1]
                                          [3, 1, 2, 0]
    */
}

int FoodDistribution (int arr[], int arrLength)
{
  unsigned int totDiff = -1;
  FindMinDiff(arr, arrLength, arr[0], totDiff);
  return totDiff;
}

int main(void) { 
   
  // keep this function call here
  int A[] = coderbyteInternalStdinFunction(stdin);
  int arrLength = sizeof(A) / sizeof(*A);
  cout << FoodDistribution(A, arrLength);
  return 0;
    
}
```