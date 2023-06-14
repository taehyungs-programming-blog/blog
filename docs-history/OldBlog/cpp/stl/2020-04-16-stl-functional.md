---
title: "STL : function"
permalink: cpp/stl/function/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-16 00:00:00 -0000
last_modified_at: 2020-04-16 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

* [Run This Code](https://ideone.com/lLVPNW)

```cpp
#include <iostream>
#include <functional>
using namespace std;
using namespace std::placeholders;

void f1(int a) { printf("f1 : %d\n", a); }
void f2(int a, int b, int c) { printf("f2 : %d, %d, %d\n", a, b, c); }

int main()
{
    void(*f)(int) = &f1;        // ok
    void(*f)(int) = &f2;        // error

    // 함수 포인터는 signature가 동일해야한다

    function<void(int)> f;      // 리턴이 void이고 입력이 int면 다된다.
    
    f = &f1;        // ok
    f = bind(&f2, 1,2,_1)        // ok
    f(10);          // f2(1,2,10);

    bind(&f2, 1,2,_1)(10);  // 는 그냥 호출임을 기억
}
```

* [Run This Code](https://ideone.com/b3FARF)

```cpp
#include <iostream>
#include <functional>
using namespace std;
using namespace std::placeholders;

class Test
{
public:
    int data = 0;
    void f(int a, int b)
    {
        data = a;
        printf("f : %d, %d\n", a, b);
    }
};

int main()
{
    Test t;

    function<void(int)> f1;
    f1 = bind(&Test::f, &t, _1, 20);

    // 일반 함수 모양의 function
    f1(10); // t.f1(10, 20);

    // 객체를 function의 인자로 받는 방법
    function<void(Test*, int)> f2;
    f2 = bind(&Test::f, _1, _2, 20);
    f2(&t, 100);        // t.f(100, 20)

    function<void(Test, int)> f3;
    f3 = bind(&Test::f, _1, _2, 20);
    f3(&t, 200);        // t.f(200, 20)

    function<void(Test&, int)> f4;
    f4 = bind(&Test::f, _1, _2, 20);
    f4(&t, 300);        // t.f(200, 20)

    cout << t.data << endl;
}
```

```cpp
#include <iostream>
#include <functional>
using namespace std;
using namespace std::placeholders;

class Test
{
public:
    int data = 10;
};

int main()
{
    Test t1, t2;

    function<int()> f1;
    f1 = bind(&Test::data, &t1);    // t1.data

    cout << f1() << endl;           // t1.data get

    f1() = 20;                      // error - functiond이 값으로 리턴하기에

    function<int&()> f2;
    f2() = 20;                      // ok

    function<int&(Test*)> f2;
    f2 = bind(&Test::data, _1);
    f2(t1) = 20;                    // ok
    f2(t2) = 40;                    // ok
}
```