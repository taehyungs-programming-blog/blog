---
title: "(C++) Conversion"
date: 2020-02-22 00:00:00 -0000
---

```cpp
int main()
{
    int(*f)(int, int) = [](int a, int b)
    {
        return a + b;
        // return type은 어떻게 구현했을까?
    };
}
```

```cpp
class ClosureType
{
public:
    int operator()(int a, int b) const
    {
        return a + b;
    }
    static int method(int a, int b)
    {
        return a + b;
    }

    typedef int(*F)(int, int);

    operator F()
    {
        // return &ClosureType::operator();
        // 멤버 함수포인터를 일반함수 포인터에 넣을 수 없기에 불가능

        return &ClosureType::method;
    }
}
```

```cpp
// 단, 변수를 캡처한 람다표현식은 일반함수 포인터로 캐스팅이 불가능하다.
int main()
{
    int v = 0;
    int(*f)(int, int) = [v](int a, int b)   // error
    {
        return a + b;
    };
}
```

```cpp
class ClosureType
{
    int v;
public:
    ClosureType(int a) : v(a) {}
    int operator()(int a, int b) const
    {
        return a + b + v;
    }
    static int method(int a, int b)
    {
        return a + b + v; // error
        // static 함수는 멤버변수에 접근이 불가
    }

    typedef int(*F)(int, int);

    operator F()
    {
        return &ClosureType::method;
    }
}
```