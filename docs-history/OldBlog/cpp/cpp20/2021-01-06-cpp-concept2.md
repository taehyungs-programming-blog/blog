---
title: "(c++20) concept2"
permalink: cpp/cpp20/concept2/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2021-01-06 00:00:00 -0000
last_modified_at: 2021-01-06 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
  - c++20
category:
  - concept
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

## concept

```cpp
#include <iostream>
#include <type_traits>

template<typename T> 
concept Integral = std::is_integral_v<T>;   // Integral이라는 concept은 조건이 integral일때만 가능

// template<typename T> requires std::is_integral_v<T>
template<typename T> requires Integral<T>
void foo(T a)
{
}

int main()
{
    bool b = Integral<int>; // int가 Integral을 만족하면 true가 나온다.
}
```

여러개의 제약조건을 하나의 이름으로 사용이 가능하다

---

## requires expression

```cpp
#include <type_traits>

template<typename T> concept Integral = std::is_integral_v<T>;
template<typename T> concept True = true;

// requires expression
template<typename T>
concept Modules = requires(T a, T b)    // 마치 함수처럼 쓴다
{
    // 요구사항 작성
    a % b;      // % 연산을 할 수 있어야 한다.
};

template<typename T> requires Modules<T>    // 이건 requires clauses (리콰이어 절(표현식))
T Mod(T a, T b)
{
    return a % b;
}

int main()
{
    Mod(10, 3);     //
    Mod(3.4, 2.1);  // Error : 실수는 %연산 불가
}
```

```cpp
#include <concepts>
#include <vector>

template<typename T>
concept LessThanComparable1 = requires(T a, T b)
{
    // < 연산이 가능해야 한다.
    a < b;
};

template<typename T>
concept LessThanComparable2 = requires(T a, T b)
{
    // {a < b} 의 결과가 bool로 변환이 가능해야한다.
    {a < b} -> std::convertible_to<bool>;
};

template<typename T>
concept Equality = requires(T a, T b)
{
    // {a < b} 과 {a != b} 의 결과가 bool로 변환이 가능해야한다.
    {a == b} -> std::convertible_to<bool>;
    {a != b} -> std::convertible_to<bool>;
};

template<typename T>
concept Container = requires(T c)
{
    // begin, end함수를 부를수 있어야 한다.
    c.begin();
    c.end();
};

template<typename T>
concept HasValueType = reuires
{
    // T안에 value_type이라는 변수가 있어야 한다.
    typename T::value_type;
};

template<typename T> requires Container<T>
void foo(T a) {}        // foo는 Container를 만족 (begin, end함수가 있다)에만 사용한다.

template<typename T> requires HasValueType<T>
void goo(T a) {}       // goo는 HasValueType를 만족 (value_type 변수가 있다)에만 사용한다

int main()
{
    std::vector<int> v = {1,2,3}
    foo(v);     // ok
    goo(v);     // ok

    foo(1);         // error
    goo(1);         // error
}
```

---

## partial ordering

```cpp
#include <iostream>

template<typename T>
concept Concept1 = sizeof(T) > 1;

template<typename T>
concept Concept2 = sizeof(T) > 1 && sizeof(T) < 8;

template<typename T> requires Concept1<T>
void foo(T a) { std::cout << "1" << std::endl; }

template<typename T> requires Concept2<T>
void foo(T a) { std::cout << "2" << std::endl; }

int main()
{
    foo(3);     // concept1, 2를 모두 만족하는데?? -> Error가 발생함
}
```

```cpp
#include <iostream>

template<typename T>
concept Concept1 = sizeof(T) > 1;

template<typename T>
concept Concept2 = Concept1<T> > 1 && sizeof(T) < 8;

template<typename T> requires Concept1<T>
void foo(T a) { std::cout << "1" << std::endl; }

template<typename T> requires Concept2<T>
void foo(T a) { std::cout << "2" << std::endl; }

int main()
{
    foo(3);     // 2 -> Concept1의 조건을 기본으로 만족하고 Concept2까지 만족한다
}
```

```cpp
#include <iostream>

template<typename T>
concept Concept1 = sizeof(T) > 1;

template<typename T> requires Concept1<T>
void foo(T a) { std::cout << "1" << std::endl; }

template<typename T> requires Concept1<T> && (sizeof(T) < 8)
void foo(T a) { std::cout << "2" << std::endl; }

int main()
{
    foo(3);     // 2 -> &&으로 조건을 추가해도 동작한다.
}
```

```cpp
#include <iostream>

template<typename T>
concept Concept1 = sizeof(T) > 1;

template<typename T>
concept Concept2 = sizeof(T) < 8;

template<typename T>
concept Concept3 = Concept1<T> && Concept2<T>;

template<typename T> requires Concept1<T>
void foo(T a) { std::cout << "1" << std::endl; }

template<typename T> requires Concept3<T>
void foo(T a) { std::cout << "2" << std::endl; }

int main()
{
    foo(3); // 2
}
```

---

```cpp
#include <vector>
#include <list>
#include <iostream>

int main()
{
    //std::vector<int> c = {1,2,3,4,5,6,7,8,9,10};
    std::list<int> c = {1,2,3,4,5,6,7,8,9,10};

    auto p = c.begin();

    // 반복자를 3칸 전진하고자 한다면?
    p = p + 3;      // list는 덧셈이 불가능!

    std::advance(p, 3); // 3칸 전진해 달라 -> 이걸만들어 보자.
}
```

```cpp
template<typename T> requires std::input_iterator<T>
void advance(T p, int N)
{
    while(N--) ++p;
}

template<typename T> requires std::random_access_iterator<T>
void advance(T p, int N)
{
    p = p + N;
}
```

---

## syntax sugar

제약을 표기하는 또 다른 표기법

```cpp
#include <concepts>

template<typename T> requires std::integral<T>
void f1(T a) { }

template<typename T> 
void f2(T a) requires std::integral<T>
{ }

template< T> 
void f3(T a)
{ }

void f4(std::integral auto a)   // template이 아닌거 같지만 template이 맞음.
{ }

void f5(std::integral a)   // auto를 빼는건 불가능
{ }

void f6(auto a)   // 이런표현도 가능한데 조건이라기 보단 
// template<typename T> void f6(T a) 과 동일한 표현이다.
{ }

int main()
{ 
    f6(10); // ok
}
```

---

## 표준 Concept

```cpp
#include <iostream>
#include <list>
#include <vector>
#include <algorithm>
#include <iterator>

template<typename T>
void sort(T first, T last)
{
    std::cout << "random_access_iterator" << std::endl;
}

int main()
{
    std::vector<int> v = {1,3,57,9,2,4,6,8,10};
    std::list<int> s = {1,3,57,9,2,4,6,8,10};

    std::sort(v.begin(), v.end());
    std::sort(s.begin(), s.end());  // error
    s.sort();   // ok

    // 사용법이 다르니 불변! -> 좋은 방법없나?
}
```

```cpp
/*
template<typename T> requires std::random_access_itertor
void sort(T first, T last)
{
    std::cout << "random_access_iterator" << std::endl;
}
*/

// 더 좋은 표현
template<std::random_access_itertor T> 
void sort(T first, T last)
{
    std::cout << "random_access_iterator" << std::endl;
}

template<std::bidirectional_iterator T> 
void sort(T first, T last)
{
    std::cout << "bidirectional_iterator" << std::endl;
}
```

```cpp
#include <iostream>
#include <algorithm>
#include <iterator>

template<typename T1, typename T2>
T1 find(T1 first, T1 last, T2 pred)
{
    std::cout << "predicate version" << std::endl;
    return first;
}

int main()
{
    int x[10]{1,2,3,4,5,6,7,8,9,10};

    auto p1 = std::find(std::begin(x), std::end(x), 3);
    auto p2 = std::find_if(std::begin(x), std::end(x), [](int n) { return n % 2 == 0;});
    // find_if 말고 그냥 find로 표현하고 싶다면?
    auto p3 = find(std::begin(x), std::end(x), [](int n) { return n % 2 == 0;});
}
```

```cpp
// T2가 인자로 T1::value_type을 받을수 있다면
template<typename T1, typename T2> requires std:predicate<T2, typename std::iterator_traits<T1>:value_type>   // T2가 T1을 받을수 있다
T1 find(T1 first, T1 last, T2 pred)
{
    std::cout << "predicate version" << std::endl;
    return first;
}

template<typename T1, typename T2> requires (!std:predicate<T2, typename std::iterator_traits<T1>:value_type>)
T1 find(T1 first, T1 last, T2 pred)
{
    std::cout << "value version" << std::endl;
    return first;
}
```