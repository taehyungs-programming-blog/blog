---
title: "(C++) lambda MISC"
date: 2020-02-22 00:00:00 -0000
---

```cpp
#include <iostream>
using namespace std;

int main()
{
    // generic lambda : c++14
    auto f1 = [](auto a, auto b){ return a + b; }

    // nullary lambda : 괄호를 빼도 됨
    auto f2 = [] { return 10; }

    // c++17 : ()함수를 constexpr함수로
    auto f3 = [](int a, int b) constexpr
    {
        return a + b;
    };

    int y[f3(1,2)];
}
```

```cpp
int main()
{
    auto f1 = [](int a, int b) { return a + b; }

    decltype(f1) f2;        // error - 디폴트 생성자가 람다에 없기에 에러
    decltype(f1) f3 = f1;   // ok - 복사생성자는 람다에 있음 ok
    decltype(f1) f4 = move(f1); // ok - move생성자도 있음.
}
```