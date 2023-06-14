---
title: "STL : Container Basic"
permalink: cpp/stl/container-basic/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-11 00:00:00 -0000
last_modified_at: 2020-04-11 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

## Preview

* Sequence Container
    * 요소가 삽입된 순서대로 놓여있는 컨테이너
    * C++98(list, vector, deque), C++11(forward_list, array)
* Associative Container
    * 삽입된 순서와 상관없이 규칙을 가지고 요소를 보관
    * C++98(set, multi-set, map, multi-map), C++11(hash, unordered_set, unordered_multiset, unordered_map, unordered_multimap)
* Container Adapter
    * stack, queue, priority_queue

* [Run This Code](https://ideone.com/5MwVI8)

```cpp
#include <iostream>
#include <list>
#include <stack>
#include <set>
using namespace std;

struct Point
{
    int x;
    int y;
};

int main()
{
    list<Point> s;
    Point pt;

    s.push_back(pt);        // 값을 보관하는 것이 아니라 복사본을 보관함을 기억하자.

    list<Point>::value_type n;      // Point
    list<Point>::iterator p;        // 반복자 타입
    auto p = being(s);

    list<int> s;

    s.push_back(10);
    s.push_back(20);
    s.push_back(30);

    cout << s.pop_back();       // error - 제거만 한다.
    cout << s.back();           // 30 - 단 제거는 안한다.
    cout << s.back();
}
```

* STL은 예외가 거의 없기에 예외처리를 굳이 하지 않아도 된다.

---

## 단위 전략 디자인(Policy Base Desing)

* 클래스가 사용하는 다양한 정책을 '템플릿 인자'를 통해서 교체할 수 있게 하는 디자인 기법

```cpp
#include <iostream>
using namespace std;

template<typename T, typename Allocator = allocator<T>> class vector
// allocator<T>는 C++에서 제공해주는 메모리할당기 이다.
{
    // ...
    Allocator ax;
public:
    void push_back(const T& a)
    {
        // 메모리 할당이 필요하다.
        T* p = ax.allocate(1);      // Allocator : 메모리할당을 담당

        ax.deallocate(p, 1);
    }
};

int main()
{
    vector<int> v;
    vector<int, MyAlloc> v; // 내가 메모리할당기를 만들고 싶다면 이렇게 만들 수 있다.

    v.push_back(10);
}
```

```cpp
#include <iostream>
#include <string>
using namespace std;

int main()
{
    string s1 = "abcd";
    string s1 = "ABCD";

    if(s1 == s2)
        cout << "same" << endl;
    else
        cout << "not same" << endl;

    // 만약 대소문자 구분을 하고 싶지 않다면??
}
```

```cpp
// 나만의 비교정책을 만든다.
class mytraits : public char_traits<char>
{
public:
    // 코드는 생략...
};
using ci_string = std::basic_string<char, mytraits>;
// 내가 만든 비교정책을 적용한 string을 ci_string이라 부르겠다.

int main()
{
    ci_string s1 = "abcd";
    ci_string s1 = "ABCD";

    if(s1 == s2)
        cout << "same" << endl;
    else
        cout << "not same" << endl;

    // 만약 대소문자 구분을 하고 싶지 않다면??
}
```
