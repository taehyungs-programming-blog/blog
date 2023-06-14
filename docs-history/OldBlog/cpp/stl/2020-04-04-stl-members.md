---
title: "STL member"
permalink: cpp/stl/member/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-04 00:00:00 -0000
last_modified_at: 2020-04-04 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

## Lecture Node

* [Lecture Node : 멤버 타입 사용해보기](https://ideone.com/pBOYrR)

* STL 컨테이너 내에는 다양한 member type의 출력을 지원하는 변수가 있다.
* 여기서는 value_type을 예제로 보여준다, 나머지는 알아서 찾아볼 것

---

## 멤버 타입 사용해보기

```cpp
#include <iostream>
#include <list>
#include <vector>
using namespace std;

// 인자의 첫 번째 요소를 출력
void print(vector<int>& c)
{
    int n = c.front();
    cout << n << endl;
}

int main()
{
    vector<int> v = { 1, 2, 3 };
    print(v);
}
```

지금은 print함수가 int만 받을수 있기에 확장성을 늘려보자.

```cpp
template<typename T>
void print(vector<T>& c)
{
    T n = c.front();
    cout << n << endl;
}
```

그런데 list도 받을수 있게, 컨테이너도 받을수 있게는 못하나??

```cpp
template<typename T>
void print(T& c)
{
    // C++11이후 문법은 이렇게 만들면된다.
    // auto n = c.front();
    
    // 하지만 이전버전도 어떻게 만드는지는 알아야 겠지?

    // T : list<double> 이런식으로 넘어올텐데, double n을 어떻게 뽑을까??
    T n = c.front();
    cout << n << endl;
}
```

STL은 이런것을 지원한다.

```cpp
list<int> s = { 1, 2, 3 };
list<int>::value_type n = s.front();        // n = int가 된다.
```

활용해보자

```cpp
template<typename T>
void print(T& c)
{
    // typename : value_Type을 타입의 이름을 static이 아니라 타입의 이름으로 해석해 달라는 뜻..
    // 자세한건 C++ template 참조
    typename T::value_type n = c.front();        // double로 만들수 있다.
    cout << n << endl;
}
```

---

## value_type Example

```cpp
#include <iostream>
#include <list>
#include <vector>
using namespace std;

template<typename T>
void sum(T first, T last)
{
    typename T::value_type s = 0;

    // ... first부터 last까지 합을구한 후 return
}

int main()
{
    list<int> s = { 1, 2, 3 };
    sum( s.begin(), s.end() );
}
```

---

## C++ 17 문법

```cpp
#include <iostream>
#include <list>
using namespace std;

template<typnname T> class List
{
public:
    List(int sz, T initValue) {}
};

int main()
{
    List<int> s1(10, 0);
    List      s2(10, 0);        // C++17- ok
}
```