---
title: "(C++) return type resolver"
date: 2020-02-02 00:00:00 -0000
---

```cpp
#include <iostream>
#include <cstdlib>
using namespace std;

// int* memAlloc(int sz)
template<typename T> T* memAlloc(int sz)
{
    return (T*)malloc(sz);
}

int main()
{
    // int* p = memAlloc(40);
    double * p = memAlloc<double>(40);
    // 여기 꼭 <double>을 입력해줘야하나??
}
```

> 해결책

```cpp
class memAlloc
{
    int size;
public:
    inline memAlloc(int sz) : size(sz) {}

    // operator double*()
    template<typename T> operator T*()
    {
        // return (double*)malloc(size);
        return (T*)malloc(size);
    }
};

int main()
{
    double* p1 = memAlloc(40);
    // 클래스이름() : 임시객체
    // 임시객체.operator double*()

    int* p2 = memAlloc(40);
}
```

