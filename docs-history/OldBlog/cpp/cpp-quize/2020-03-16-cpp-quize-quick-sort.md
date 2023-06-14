---
title: "(C++) quick sort 구현"
permalink: cpp/quize/quick-sort/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-15 00:00:00 -0000
last_modified_at: 2020-03-15 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

quick sort에 대한 이론적 내용을 모른다면 참고사이트를 보자.

```cpp
#include <stdio.h> 

int data[10] = {4, 1, 2, 3, 9, 7, 8, 6, 10, 5}; 

void quick_sort(int *data, int start, int end){ 
    if(start >= end){ // 원소가 1개인 경우 
        return; } 
    int pivot = start; 
    int i = pivot + 1; // 왼쪽 출발 지점 
    int j = end; // 오른쪽 출발 지점 
    int temp; 
    
    while(i <= j){ // 포인터가 엇갈릴때까지 반복 
        while(i <= end && data[i] <= data[pivot]){ 
            i++; 
        } 
        while(j > start && data[j] >= data[pivot]){ 
            j--; 
        } 
        
        if(i > j){ // 엇갈림 
            temp = data[j]; 
            data[j] = data[pivot]; 
            data[pivot] = temp; 
        } else { // i번째와 j번째를 스왑 
            temp = data[i]; 
            data[i] = data[j]; 
            data[j] = temp; 
        } 
    } // 분할 계산 
    
    quick_sort(data, start, j - 1); 
    quick_sort(data, j + 1, end); 
    
} 

int main(void){ 
    quick_sort(data, 0, 9); // 결과 확인 
    
    for(int i=0; i<10; i++){ 
        printf("%d ", data[i]); 
    } 
    
    return 0; 
}
```

> * [참고사이트](https://hongku.tistory.com/149)