---
title: "(C++) trivial 개념"
date: 2020-02-01 00:00:00 -0000
---

## trivial 개념

* trivial은 하는일이 없는지 확인하는 것이고
* `is_trivially_constructible`를 활용하면 된다.

```cpp
#include <iostream>
#include <type_traits>
using namespace std;

// trivial : 사소한, 하찮은, 하는일이 없다.
// trivial default constructor : 기본생성자가 하는일이 없는 경우

class A
{
    // A는 생성자가 없기에 하는일이 없음
    // 그럼 trivial constructor라 할 수 있나? -> is_trivially_constructible 호출
public:
    virtual void foo() {}
};

int main()
{
    cout << is_trivially_constructible<A>::vale << endl;
    // false, 가상함수를 초기화 하기에 trivial constructor라 하기 힘들다.
}
```

```cpp
class A
{
public:
    void foo() {}
};

int main()
{
    cout << is_trivially_constructible<A>::vale << endl;
    // true, 가상함수가 없기에
}
```

```cpp
class A
{
    A() {}
public:
    virtual void foo() {}
};

int main()
{
    cout << is_trivially_constructible<A>::vale << endl;
    // false, cpp에 어떻게 contructor가 구현되어 있을지 모르니 컴파일러는 false를 출력한다.
}
```

```cpp
class A
{
    A() = default;
public:
    virtual void foo() {}
};

int main()
{
    cout << is_trivially_constructible<A>::vale << endl;
    // true, 컴파일러에게 직접 default로 구현해달라 했으니
}
```