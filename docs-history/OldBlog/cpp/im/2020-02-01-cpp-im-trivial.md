---
title: "(C++) trivial 정리"
date: 2020-02-01 00:00:00 -0000
---

## trivial default constructor의 조건

```cpp
// trivial default constructor의 조건.
struct Base
{
    Base() {}
};

class A : // public Base            // 기반클래스가 없거나 trivial해야
{
    // Data data;                   // 객체형 멤버가 없어야
    // int n = 10;                     // 생성자에서 초기화가 없어야
public:
    // A() {}                       // user define 생성자가 없어야
    // virtual void foo() {}        // 가상함수가 없어야
};

int main()
{
    cout << is_trivally_constructible<A>::value << endl;
    // true
}
```
