---
title: "(C++) Callable Object & invoke"
date: 2020-02-22 00:00:00 -0000
---

## Function object 정의

```cpp
#include <iostream>
#include <functional>
using namespace std;

void foo()
{

}

int main()
{
    foo(); // function

    less<int> f;
    f(1, 2);        // functor

    [](int a, int b) { return a + b; }(1, 2);       // lambda

    void(&r)() = foo;
    r();        // Nope
}
```
* Function object 정의
    - A FunctionObject type is the type of an object that can be used on the left of the function call operator.
    - 모든 종류의 함수 포인터
    - ()연산자를 재정의한 클래스
    - Lambda Expression
    - **함수와 함수를 가리키는 참조는 FunctionObject가 아니다**

```cpp
template<typename F, typename ... Types>
decltype(auto) chronometry(F f, Types&&& ... args)
{
    f(std::forward<Types>(args)...);
}

int main()
{
    chronometry(less<int>(), 10, 20);
}
```

```cpp
template<typename F, typename ... Types>
decltype(auto) chronometry(F&& f, Types&&& ... args)
{
    std::forward<F>(f)(std::forward<Types>(args)...);
}
```

---

```cpp
#include <functional>   // C++ 17 for invoke

template<typename F, typename ... Types>
decltype(auto) chronometry(F&& f, Types&&& ... args)
{
    invoke(std::forward<F>(f), std::forward<Types>(args)...);
}
```

---

```cpp
invoke(&foo, 10, 20);               // foo(10, 20);

Dialog dlg;
invoke(&Dialog::setColor, &dlg, 2); // dlg.setColor(2);
```

```cpp
invoke(&Dialog::color, &dlg) = 20;  // dlg.color = 20;
```
