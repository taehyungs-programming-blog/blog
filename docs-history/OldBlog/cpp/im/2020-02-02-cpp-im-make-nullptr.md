---
title: "(C++) make nullptr"
date: 2020-02-02 00:00:00 -0000
---

```cpp
int main()
{
    int n1 = 10;        // ok
    void* p1 = 10;      // error

    int n2 = 0;         // ok
    void* p2 = 0;       // ok, 0은 정수이지만 포인터로 암시적 형변환된다.
}
```

```cpp
#include <iostream>
using namespace std;

void foo(int n) { cout << "int" << endl; }
void foo(void* p) { cout << "void*" << endl; }

int main()
{
    foo(0);             // void foo(int n)로 가게 된다.
    foo((void*)0);      // void foo(void* p)로 간다.
}
```

> 하지만 C++에서는 NULL을 다음과 같이 정의한다.

```cpp
#ifdef __cplusplus
    #define NULL 0
#else
    #define NULL (void*)0
#endif
```

```cpp
foo(NULL);          // void foo(int n)로 가게 된다.
// ??? -> 명확한 nullptr선언이 필요해진다.
```

```cpp
struct xnullptr_t
{
    // operator void*() { return 0; }
    template<typename T>
    operator T*() { return 0; }
};
xnullptr_t xnullptr;

int main()
{
    foo(0);
    foo(xnullptr);      // xnullptr_t가 void*로 암시적 변환이 된다면?
}
```

> 위 struct가 `nullptr`로 직접구현되어 있다.
