---
title: "(C++) advanced lambda"
date: 2020-02-22 00:00:00 -0000
---

```cpp
#include <iostream>
using namespace std;

int main()
{
    auto f = [](int a, int b) { return a + b; }

    // 아래와 같이 여러개의 변수를 받을 수 있는 람다표현식을 만들어보자.
    f(1, 2);
    f(1, 2, 3);
    f(1, 2, 3, 4);
}
```

```cpp
template<typename ... Types> class mi : public Types ...
{
public:
    mi(Types&& ... args) : Types(args)... {} 
    // 람다는 기본생성자는 delete되어있기에
    // 복사 생성자를 유도

    // 기반 클래스의 특정함수를 사용할수 있게 유도
    using Types::operator()...;
};

int main()
{
    mi f([](int a, int b) { return a+b; },
        [](int a, int b, int c) {return a+b+c; });
    // 여러 변수를 받을 수 있는 람다표현식을 생성.

    cout << f(1, 2) << endl;
    cout << f(1, 2, 3) << endl;
}
```