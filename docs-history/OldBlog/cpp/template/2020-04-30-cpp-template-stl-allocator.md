---
title: "C++ Template : STL allocator"
permalink: cpp/template/stl-allocator/                # link 직접 지정
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
// STL의 vector를 생각해 보자.
template<typename T> class vector
{
    T* buff;
public:
    void resize(int n)
    {
        // 버퍼 재할당이 필요하다면?
        // new?, malloc?, calloc? ... 뭘쓰나??
        // STL내부에는 메모리할당기를 제공한다.
    }
};

int main()
{
    vector<int> v(10);
    v.resize(20);
}
```

```cpp
template<typename T> class allocator
{
public:
    T* allocator() {}
    void deallocate() {}
};

template<typename T, typename Ax = allocator<T>> class vector
{
    T* buff;
    Ax ax;
public:
    void resize(int n)
    {
        T* p = ax.allocate(n);
        ax.deallocate(p);
    }
};


int main()
{
    vector<int, MyAlloc<int>> v(10);        // 이런 선언이 가능하다.
    v.resize(20);
}
```
