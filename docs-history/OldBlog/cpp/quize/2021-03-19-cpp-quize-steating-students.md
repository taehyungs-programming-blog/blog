---
title: "(C++ Quize) Medium : Seating Students"
permalink: cpp/quize/seating-students/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2021-03-19 00:00:00 -0000
last_modified_at: 2021-03-19 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
  - quize
category:
  - seating-students
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

## Q

```
Input: {6, 4}
Output: 4
```

```
Input: {8, 1, 8}
Output: 6
```

---

## A

```cpp
#include <iostream>
#include <string>
#include <set>

using namespace std;

int SeatingStudents(int arr[], int arrLength) {
  int num_seats = arr[0];
  auto seats = std::set<int>();
  for (int i = 1; i < arrLength; ++i) seats.insert(arr[i]);
  
  int solution = 0;
  for (int seat = 1; seat < num_seats; ++seat) {
    if (seats.find(seat) == seats.end()) {          // 빈 의자라면 검색
      solution += seat+2 <= num_seats && 
                  seats.find(seat+2) == seats.end();
      solution += seat+1 <= num_seats && 
                  seat % 2 > 0 && 
                  seats.find(seat+1) == seats.end();
    }
  }

    /*
    1  2
    3  4
    5  6
    7  8
    9  10
    11 12
    ...
    */

  return solution;
}

int main(void) { 
   
  // keep this function call here
  int A[] = coderbyteInternalStdinFunction(stdin);
  int arrLength = sizeof(A) / sizeof(*A);
  cout << SeatingStudents(A, arrLength);
  return 0;
    
}
```