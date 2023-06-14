---
title: "C++ Template : policy-based design"
permalink: cpp/template/policy-based-design/                # link 직접 지정
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
template<typename T> class List
{
public:
    void push_front(const T& a)
    {
        // ...
    }
};

List<int> s;        // 전역변수, 멀티스레드에 안전하지 않음.

int main()
{
    // lock
    s.push_front(10);
    // unlock 의 절차가 필요하다.
}
```

```cpp
template<typename T, typename ThreadModel> class List
{
    ThreadModel tm;
public:
    void push_front(const T& a)
    {
        tm.lock();
        // ...
        tm.unlock();
    }
};

class NoLock
{
public:
    inline void lock() {}
    inline void unlock() {}
};

class MutexNoLock
{
public:
    inline void lock() { // mutex something }
    inline void unlock() { // mutex something }
};

// List<int, NoLock> s;
List<int, MutexNoLock> s;

int main()
{
    s.push_front(10);
}
```
