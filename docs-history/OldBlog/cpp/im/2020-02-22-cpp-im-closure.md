---
title: "(C++) Closure Object"
date: 2020-02-22 00:00:00 -0000
---

```cpp
#include <iostream>
#include <algorithm>
#include <functional>
using namespace std;

bool cmp(int a, int b) { return a < b; }

int main()
{
    int x[10] = { 1, 3, 5, 7, 9, 2, 4, 6, 8, 10 };

    // 비교정책으로 함수 사용.
    sort(x, x+10, cmp);

    // 비교정책으로 함수객체 사용.
    sort(x, x+10, less<int>());

    // C++11 - 람다 표현식으로 사용
    sort(x, x+10, [](int a, int b) { return a < b; });
}
```

> 람다표현식을 쓰는 순간 컴파일러는 `operator()()`를 생성해준다.<br>
> 그때 생성되는 임시객체를 Closure라하고 아래와 같이 컴파일러가 생성해준다.

```cpp
class ClosureType
{
public:
    bool operator()(int a, int b) const
    {
        return a < b;
    }
};
sort(x, x + 10, ClosureType());
```

* 함수 객체를 좀 편하게 쓸 수 있게 만들어 줬다고 생각하자.

