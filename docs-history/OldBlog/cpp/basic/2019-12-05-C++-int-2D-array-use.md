---
title: "(C++) int형 2차 배열 사용하기"
date: 2019-12-05 00:00:00 -0000
---

> 코딩테스트를 진행하다보면 2차원 배열을 사용해야할 경우가 있다.
>
> 가령 ...

```cpp
#define MATRIX_SIZE 4

int ** matrix = new int*[MATRIX_SIZE];

for(int i = 0; i < MATRIX_SIZE; i++) {
    matrix[i] = new int[MATRIX_SIZE];
}

for(int i = 0; i < MATRIX_SIZE; i++) {
    delete[] matrix[i];
}

delete[] matrix;
```