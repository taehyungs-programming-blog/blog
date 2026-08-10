---
layout: default
title: "31. (C++17) std::apply"
parent: "(C++) 문법"
grand_parent: (C++)
great_grand_parent: "Legacy Archive"
nav_order: 5
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

{% raw %}

* [참고 사이트 🌍](https://docs.w3cub.com/cpp/utility/apply)

```cpp
#include <iostream>
#include <tuple>
#include <utility>
 
int add(int first, int second) { return first + second; }
 
template<typename T>
T add_generic(T first, T second) { return first + second; }
 
auto add_lambda = [](auto first, auto second) { return first + second; };
 
int main()
{
    // OK
    std::cout << std::apply(add, std::make_pair(1, 2)) << '\n';
 
   // Error: can't deduce the function type
   // std::cout << std::apply(add_generic, std::make_pair(2.0f, 3.0f)) << '\n'; 
 
   // OK
   std::cout << std::apply(add_lambda, std::make_pair(2.0f, 3.0f)) << '\n'; 
}
```

🎈 ??? 그냥 `add_lambda(2.0f, 3.0f);`로 호출해 버리면 안되나?<br>
🎈 물론 가능하다! 그런데 `add_lambda`가 매개변수로 받는다거나 컴파일 타임에 모르는 케이스가 있다.

```cpp
// Example

template<typename Ret, typename... Args>
class FuncJob
{
    using FuncType = Ret(*)(Args...);

public:
    FuncJob(FuncType func) : _func(func), _tuple(args...) {}

    Ret operator()()
    {
        // C++17 이후부터는 아래 표현이 가능
        std::apply(_func, _tuple);
    }

    Ret Execute()
    {
        std::apply(_func, _tuple);
    }

private:
    FuncType _func;
    // tuple에 Args를 보관할 예정
    std::tuple<Args...> _tuple;
};
```

{% endraw %}