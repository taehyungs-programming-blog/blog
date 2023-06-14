---
title: "STL container"
permalink: cpp/stl/container/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-04 00:00:00 -0000
last_modified_at: 2020-04-04 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

* [Lecture Note : 문제의 접근](https://ideone.com/2mECCT)
* [Lecture Note : iterator만들어 보기](https://ideone.com/u6pZ4y)
* [Lecture Node : STL iterator사용해보기](https://ideone.com/a3LaFc)

* 핵심은 STL iterator가 어떻게 구성되어 있는지 이해하는 것이다.
* 그리고 이러한 container, iterator를 왜 쓰는지 이해하는 것인데, container, itnerator를 쓰면 공통의 함수 find, remove 등을 사용해서 다양한 자료형(Container)를 제어할 수 있다 이런 find, remove와 같은 함수를 Algorithm이라 한다.
* 따라서 STL은 자료구조와 알고리즘이 분리된 라이브러리라 한다.

---

## 문제의 접근

우선 간단한 single linked list node를 만들어보자.

```cpp
#include <iostream>
using namespace std;

template<typename T> struct Node
{
    T data;
    Node* next;

    Node(const T& a, Node* n) : data(a), next(n) {}
};

template<typename T> class slist
{
    Node<T>* head = 0;
public:
    void push_front(const T& a)
    {
        head = new Node<T>(a, head);
    }
};

int main()
{
    slist<int> s;
    s.push_front(10);
    s.push_front(20);
    s.push_front(30);
}
```

find()함수를 이용해서 slist에서 값을 검색할 수 있을까??<br>
**핵심은 다음 요소로 접근할때 ++를 쓸수 없다는 점이다**<br>
음... 그래도 확장을 하고 싶은데?

## iterator만들어 보기

```cpp
// efind.cpp
template<typename T1, typename T2>
T1 efind(T1 first, T1 last, T2 value)
{
    while(first != lat && *first != value)
        ++first;
    return first;
}
```

조건검사 (!=), 전위 연산(++), 포인터 출력(*)을 지원해야한다.<br>
반복자를 만들어서 쓰면될 것 같은데??<br>

```cpp
template<typename T> struct Node
{
    T data;
    Node* next;

    Node(const T& a, Node* n) : data(a), next(n) {}
};

template<typename T> class slist_iterator
{
public:
    Node<T>* current = 0;
public:
    slist_iterator(Node<T>* p = 0) : current(p) {}

    slist_iterator& operator++()
    {
        current = current->next;
        return *this;
    }

    T& operator*() { return current->data; }

    bool operator==(const slist_iterator& it)
    {
        return current == it.current;
    }

    bool operator!=(const slist_iterator& it)
    {
        return current != it.current;
    }
};

template<typename T> class slist
{
    Node<T>* head = 0;
public:
    void push_front(const T& a)
    {
        head = new Node<T>(a, head);
    }

    slist_iterator<T> begin()
    {
        return slist_iterator<T>(head);
    }

    slist_iterator<T> end()
    {
        return slist_iterator<T>(0);
    }
};

int main()
{
    slist<int> s;
    s.push_front(10);
    s.push_front(20);
    s.push_front(30);

    slist_iterator<int> p = s.begin();

    cout << *p << endl;
    ++p;
    cout << *p << endl;

    while(p != s.end())
    {
        cout << *p << endl;
        ++p;
    }
}
```

iterator의 이름을 좀 더 간단하게 해보자.

```cpp
template<typename T> class slist
{
    Node<T>* head = 0;
public:
    void push_front(const T& a)
    {
        head = new Node<T>(a, head);
    }

    typedef slist_iterator<T> iterator;
    // using iterator = slist_iterator<T>;

    iterator begin()
    {
        return iterator(head);
    }

    iterator end()
    {
        return iterator(0);
    }
};

int main()
{
    slist<int> s;
    s.push_front(10);
    s.push_front(20);
    s.push_front(30);

    slist<int>::iterator p = s.begin();

    cout << *p << endl;
    ++p;
    cout << *p << endl;

    while(p != s.end())
    {
        cout << *p << endl;
        ++p;
    }
}
```

```cpp
int main()
{
    slist<int> s;
    s.push_front(10);
    s.push_front(20);
    s.push_front(30);

    slist<int>::iterator p = s.begin();

    slist<int>::iterator p2 = efind(s.begin(), s.end(), 120);

    if (p2 == s.end())
        cout << "fail" << endl;
    else
        cout << "found" << endl;

    while(p != s.end())
    {
        cout << *p << endl;
        ++p;
    }
}
```

---

## STL iterator사용해보기

```cpp
#include <iostream>
#include <list>
#include <vector>
#include <algorithm>
using namespace std;

int main()
{
    //list<int> s = { 1, 2, 3, 4, 5 };
    vector<int> s = { 1, 2, 3, 4, 5 };

    reverse(s.begin(), s.end());

    // 모든 요소 출력
    for(int i = 0; i < s.size(); i++)
        cout << s[i] << endl;       // 단 list에서는 동작안함

    vector<int>::iterator p = s.begin();

    while( p != s.end() )
    {
        cout << *p << endl;
        ++p;
    }

    // c++ 17이후는 이렇게 사용
    for( auto& n : s )      // 참조로 받은건 복사를 방지하기 위해서
        cout << n << endl;
}
```