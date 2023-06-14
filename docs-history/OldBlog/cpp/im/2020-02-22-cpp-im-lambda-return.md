---
title: "(C++) lambda & suffix return type"
date: 2020-02-22 00:00:00 -0000
---

```cpp
int main()
{
    auto f1 = [](int a, int b) { return a + b; }        // 리턴타입을 표기하지 않았지만 컴파일러가 알아서 판단

    auto f2 = [](int a, int b) -> int { return a + b; }
    // 직접 표기도 가능하다.

    auto f3 = [](int a, double b) { 
        if (a == 1) 
            return a + b;
        else
            return b;
    };      // compile error -> 컴파일러가 리턴타입을 모른다.

    auto f4 = [](int a, double b) -> double { 
        if (a == 1) 
            return a + b;
        else
            return b;
    };      
}
```
