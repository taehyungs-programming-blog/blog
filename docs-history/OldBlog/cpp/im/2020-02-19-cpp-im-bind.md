---
title: "(C++) std::bind"
date: 2020-02-19 00:00:00 -0000
---

```cpp
#include <iostream>
#include <functional>
using namespace std;

void foo() {}
void goo(int a) {}

int main()
{
    void(*f)() = &foo;

    // 위와 동일한 표현이다.
    function<void()> f2 = &foo;

    f2();   // foo() 호출

    f2 = bind(&goo, 5); // f2에 goo를 넣는데 인자를 5로 고정해라
    f2();               // goo(5);
}
```

* std::function
    - 함수 포인터역활을 하는 템플릿
    - 일반 함수 뿐 아니라 함수 객체, 람다 표현식 등도 담을 수 있다.
    - bind와 함께 사용하면 인자의 개수가 다른 함수(함수 객체)도 사용할 수 있다.

```cpp
void goo(int& a) { a = 30; }

int main()
{
    int n = 0;
    
    function<void()> f;
    f = bind(&goo, n);      // goo는 참조로 받는데?
    f();    

    cout << n << endl;     // n이 30이 될까? -> Nope!
}
```

```cpp
f = bind(&goo, ref(n));     // 참조로 고정해준다.
f();

cout << n << endl;          // 30이 출력된다.
```

* std::bind
    - 함수 또는 함수 객체의 인자를 고정할 때 사용한다.
    - 인자의 값을 고정 할 때 값 방식을 사용한다.
    - 참조로 인자를 고정하려면 `ref()` 또는 `cref()`를 사용한다.

> 다음강좌에서 ref를 직접만들어본다.