---
title: "STL : tratis"
permalink: cpp/stl/tratis/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-05 00:00:00 -0000
last_modified_at: 2020-04-05 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

## Lecture Note

* [Lecture Note : value_type 써보기](https://ideone.com/1vvi2K)
* [Lecture Note : tratis 써보기](https://ideone.com/VyRHbq)
* [Lecture Note : Example](https://ideone.com/uphwLB)

* 반복자가 가리키는 타입을 알고 싶을때 단순 value_type만 쓰지말고 tratis까지 쓰자.

---

## value_type 써보기

```cpp
#include <iostream>
#include <list>
using namespace std;

template<typename T>
typename T::value_type sum(T first, T last)
{
    typename T::value_type s = 0;
    // auto s = *first;     // C++11이후 버전은 이렇게 사용

    while(first != last)
    {
        s = s + *first;
        ++first;
    }
}

int main()
{
    list<int> s = {1,2,3,4,5,6,7,8,9,10};

    int n = sum(begin(s), end(s));

    cout << n << endl;
}
```

---

## tratis 써보기

위 코드에는 심각한 문제가 있다.<br>
만약, list가 가지 않고 진짜 배열이 들어간다면??<br>
T::value_type을 찾을 수 없다.

```cpp
template<typename T> struct iterator_traits
{
    using value_type = typename T::value_type;
};

template<typename T> struct iterator_traits<T*>     // 부분특수화가 가능!
{
    using value_type = T;
};

template<typename T>
typename T::value_type sum(T first, T last)
{
    typename iterator_traits<T>::value_type s = 0;

    while(first != last)
    {
        s = s + *first;
        ++first;
    }
}
```

단, iterator_traits는 iterator에서 제공해준다.

```cpp
template<typename T>
typename iterator_traits<T>::value_type sum(T first, T last)
{
    typename iterator_traits<T>::value_type s = 0;

    while(first != last)
    {
        s = s + *first;
        ++first;
    }
}
```

```cpp
template<typename T>
typename iterator_traits<T>::value_type sum(T first, T last)
{
    //typename iterator_traits<T>::value_type s = 0;

    typename remove_reference<decltype(*first)>::type s = 0;     // 이런식으로 처리 가능

    while(first != last)
    {
        s = s + *first;
        ++first;
    }
}
```

---

## Example

```cpp
template<typename T>
void eadvance_imp(T& p, int n, random_access_iterator_tag)
{
  cout << "rand ver." << endl;
  p = p + n;
}

template<typename T>
void eadvance_imp(T& p, int n, input_iterator_tag)
{
  cout << "input ver." << endl;
  while(n--) ++p;
}

template<typename T>
void eadvance(T& p, int n)
{
    // eadvance_imp(p, n, typename T::iterator_category());
    eadvance_imp(p, n, 
    typename iterator_traits<T>::iterator_category());
}

int main()
{
    // vector<int> s = {1,2,3,4,5,6,7,8,9,10};
    int s[10] = {1,2,3,4,5,6,7,8,9,10};

    auto p = begin(s);

    eadvance(p, 5);

    cout << "*p" << endl;
}
```