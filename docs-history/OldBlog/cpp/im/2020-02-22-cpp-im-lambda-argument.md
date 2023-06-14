---
title: "(C++) lambda & function argument"
date: 2020-02-22 00:00:00 -0000
---

```cpp
int main()
{
    // lambda는 세 가지 방법으로 담을 수 있다.
    auto f1 = [](int a, int b) { return a + b; }
    int(*f2)(int, int) = [](int a, int b) { return a + b; }
    function<int(int, int)> f3 = [](int a, int b) { return a + b; }

    f1(1, 2);       // inline 치환 가능
    f2(1, 2);       // 함수포인터이기에 언제든 바뀔 수 있다. -> inline 치환 불가능
    f3(1, 2);       // 변수이기에 역시 언제든 바뀔 수 있다. -> inline 치환 불가능
}
```

```cpp
void foo()
{
    f(1, 2);
}

int main()
{
    // 함수에서 람다표현식을 받는 방법???
    foo( [](int a, int b) { return a + b; });
}
```

```cpp
void foo( int (*f)(int, int) )      // 함수포인터로 받을 수 있으나 inline 치환 불가능!
{
    f(1, 2);
}
```

```cpp
void foo(function<int(int, int)> f)     // 역시 인라인 치환 불가능
{
    f(1, 2);
}
```

```cpp
void foo(auto f)        // auto는 함수의 인자가 될 수 없다.
{
    f(1, 2);
}
```

```cpp
template<typename T> void foo(T f)
// template<typename T> void foo(T& f) // 이건 불가능 임시객체를 lvalue로 받을 수 없다.
// template<typename T> void foo(const T& f) // 이게 제일 좋은 코드
{
    f(1, 2);
}

int main()
{
    foo( [](int a, int b) { return a + b; });
    foo( [](int a, int b) { return a - b; });
}
```