---
title: "STL : membertype"
permalink: cpp/stl/membertype/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-05 00:00:00 -0000
last_modified_at: 2020-04-05 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

우선 single linked list의 iterator를 보자.

```cpp
template<typename T> class slist_iterator
{
    Node<T>* current = 0;
public:
    slist_iterator(Node<T>* p = 0) : current(p) {}

    slist_iterator& operator++()
    {
        current = current->next;
        return *this;
    }

    T& operator*() { return current->data; }

    bool operator== (const slist_iterator& it)
    {
        return current == it.current;
    }

    bool operator !=(const slist_iterator& it)
    {
        return current != it.current;
    }
};
```

자신의 테그를 넣어 줘야한다.

```cpp
#include <iterator>

template<typename T> class slist_iterator
{
    Node<T>* current = 0;
public:
    slist_iterator(Node<T>* p = 0) : current(p) {}

    using iterator_category = forward_iterator_tag;

    // 아래도 추가적으로 넣어 줌.
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using difference_type = std::ptrdiff_t;

    // ...
```

그런데 iterator를 만들때마다 위 코드를 모두 넣어줘야하나?

```cpp
// 아래의 iterator는 <iterator>에서 제공해준다.
template<typename Category, 
        typename T, 
        typename Distance = std::ptrdiff_t, 
        typename Pointer = T*, 
        typename Reference = T&>
struct iterator
{
    using iterator_category = forward_iterator_tag;
    using value_type = T;
    using pointer = Pointer;
    using reference = Reference;
    using difference_type = Distance;
};

template<typename T> class slist_iterator : 
    public iterator<forward_iterator_tag, T>
{
    Node<T>* current = 0;
public:
    // ...
```
