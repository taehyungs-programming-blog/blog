---
title: "C++ Template : Lazy Instantiation"
permalink: cpp/template/LazyInstantiation/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-21 00:00:00 -0000
last_modified_at: 2020-06-16 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
header:
  teaser: /file/image/cpp-page-teaser.gif
tag:
  - C++
category:
  - template
  - lazy instantiation
excerpt: ""
---

```cpp
class A
{
    int data;
public:
    void foo(int n) { *n = 10; }        // error - 역참조가 불가능!
};

int main()
{
    A a;
}
```

```cpp
template<typename T>
class A
{
    T data;
public:
    void foo(T n) { *n = 10; }        // ok - 아직은 foo를 사용하지 않았기에 컴파일은 된다. -> lazy instantiation이라 한다.
};

int main()
{
    A<int> a;
    a.foo(0);       // error
}
```

* [Run This Code](https://ideone.com/vk9b2v)

```cpp
#include <iostream>
using namespace std;

struct Resource1
{
    Resource1() { cout << "Resource1()" << endl;}
    ~Resource1() { cout << "~Resource1()" << endl;}
};

struct Resource2
{
    Resource2() { cout << "Resource2()" << endl;}
    ~Resource2() { cout << "~Resource2()" << endl;}
};

struct Test
{
    Resource1 res1;
    static Resource2 res2;
};
Resource2 Test::res2;           // 1. 생성자 호출

int main()
{
    cout << "main" << endl;     // 2. main 함수 호출
    Test t;                     // 3. 생성자 호출
}
```

```
Resource2()   # static으로 선언된 Resource2가 main보다 먼저 생성된다.
main
Resource1()   # Test 생성시점에 Resource1이 생성.
~Resource1()
~Resource2()  # 소멸은 제일 마지막..
```

![](/file/imagecpp-template-lazy-instantiation1.png)

* [Run This Code](https://ideone.com/BRX0z6)

```cpp
template<typename T> struct Test
{
    Resource1 res1;
    static Resource2 res2;
};
template<typename T> Resource2 Test<T>::res2;

int main()
{
    cout << "main" << endl;             // 1. main 함수 호출
    Test<int> t;                        // 2. 생성자 호출
}
```

```
main
Resource1()
~Resource1()
# 사용한적 없는 Resource2는 호출이 되지 않는다.
```

![](/file/imagecpp-template-lazy-instantiation2.png)

---

```cpp
template<typename T> void foo(T n)
{
    *n = 10;
}

int main()
{
    if(false)
        foo(0);     // 인스턴스화 될 것인가?
                    // if문은 실행시간에 결정이 되기에 우선은 인스턴스화가 된다.

    if constexpr (false)
        foo(0);     // 요거는 인스턴스화가 안됨.
}
```

---

```cpp
template<typename T> void foo(T n, int)
{
    *n = 3.4;
}

template<typename T> void foo(T n, double)
{

}

int main()
{
    foo(1, 3.4);        // template<typename T> void foo(T n, double)
    // 이 결정을 컴파일 시간에 할까 실행시간에 할까??
    // 만약 실행 시간에 한다면 foo 두 함수 모두 메모리에 올라갈 것!
    // 결론은 컴파일 시간에 한다 template<typename T> void foo(T n, int)는 메모리에 올라가지 않기에 컴파일이 된다.
}
```
