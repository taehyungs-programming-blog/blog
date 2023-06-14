---
title: "C++ Template : rebind"
permalink: cpp/template/rebind/                # link 직접 지정
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
template<typename T> class allocator
{
public:
    T* allocate(int sz) { return new T[sz]; }
    void deallocate(T* p) { delete[] p; }

    tempate<typename U> struct rebind
    {
        typename allocator<U> other;
    }
};

// rebind
template<typename T, typename Ax = allocator<T>> class list
{
    struct NODE
    {
        T data;
        NODE* next, *prev;
    };
    //Ax ax;  // allocator<int> 버전임
    // 필요한거는 allocator<NODE>가 필요하다.
    
    // allocator<int>::rebind<NODE>::other ax;
    // allocator<NODE> ax;

    typename Ax::template rebind<NODE>::other ax;
public:
    void push_front(const T& a)
    {
        ax.allocate(1);
    }
};

int main()
{
    list<int> s;
    s.push_front(10);
}
```
