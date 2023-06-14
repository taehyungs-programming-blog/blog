---
title: "C++ Template : tuple get"
permalink: cpp/template/tuple-get/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-30 00:00:00 -0000
last_modified_at: 2020-04-30 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

```cpp
#include <iostream>
using namespace std;

struct Base
{
    int value = 10;
};
struct Derived : public Base
{
    int value = 20;
};

int main()
{
    Derived d;
    cout << d.value << endl;        // 20
    cout << d.Base::value << endl;  // 10

    cout << static_cast<Base>(d).value << endl;
    // 복사 후 임시객체를 생성
    cout << static_cast<Base&>(d).value << endl;
    // 참조로 호출

    static_cast<Base>(d).value = 30;        // error
    static_cast<Base&>(d).value = 30;       // ok
}
```

```cpp

int main()
{
    xtuple<int, double, char> t3(1, 3.4, 'A');

    cout << t3.value << endl;   // 1
    cout << static_cast<xtuple<double, char>&>(t3).value << endl;       // 3.4
    cout << static_cast<xtuple<char>&>(t3).value << endl;   // 'A'

    // xget<2>(t3); //이건어떻게 만들까??
}
```

```cpp
template<size_t N, typename TP>
튜플TP의 N번째 요소 타입&을 리턴
xget(TP& tp)
{
    return static_cast<TP의 N번째 기반클래스&>(tp).value;
}
```

그래서 어떻게 만들라고??

```cpp
int main()
{
    tuple_element<1, tuple<int, double, char>>::type n;

    cout << typeid(n).name() << endl;   // 1 번째의 타입리턴
}
```

```cpp
template<size_t N, typename TP> struct xtuple_element
{
    typedef TP type;
};

// 요소의 타입을 구할수 있도록 부분 특수화
template<typename T, typename ... Types> struct xtuple_element<0, xtuple<T, Types...>>
{
    typedef T type;
};

int main()
{
    tuple_element<0, tuple<int, double, char>>::type n;

    cout << typeid(n).name() << endl;   // int
    // 0번째 말고 1, 2 번째도 구해보자.
}
```

```cpp
template<size_t N, typename TP> struct xtuple_element
{
    typedef TP type;
};

// 요소의 타입을 구할수 있도록 부분 특수화
template<typename T, typename ... Types> struct xtuple_element<0, xtuple<T, Types...>>
{
    typedef T type;
    typedef xtuple<T, Types...> tupleType;
};

template<size_t N, typename T, typename ... Types> struct xtuple_element<N, xtuple<T, Types...>>
{
    typedef typename xtuple_element<N-1, xtuple<Types...>>::type type;
};

int main()
{
    tuple_element<2, tuple<int, double, char>>::type n;

    cout << typeid(n).name() << endl;   // char
    
    xtuple_element<2, xtuple<int, double, char>>::type n;   // char
    xtuple_element<2, xtuple<int, double, char>>:typeType t;    // tuple<char>
}
```

실제로 get을 만들어 보자.

```cpp
template<size_t N, typename TP>
xtuple_elelment<N, TP>::type& xget(TP& tp)
{
    return static_cast<typename xtuple_elelment<N, TP>::tupleType>(tp).value;
}

int main()
{
    tuple_element<2, tuple<int, double, char>>::type n;

    xget<1>(t3) = 1.1;
    cout << xget<1>(t3) << endl;        / 1.1
}
```

