---
title: "C++ Template : thin template"
permalink: cpp/template/thin_template/                # link 직접 지정
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
// Thin Template
// 함수의 갯수를 생각해 봅시다.

template<typename T> class Vector
{
    T* buff;
    int sz;
public:
    int size() const {}
    bool empty() const {}
    void push_front(const T& a) {}
    T& front() {}
};

int main()
{
    Vector<int> v1;
    Vector<short> v2;
    Vector<double> v3;
    // Vector에는 4개의 함수가 있고 각 타입에 따라 생성되니
    // 4 * 3 = 12개의 함수가 생성된다.
    // 줄여보자.
}
```

```cpp
// T를 사용하지 않는 모든 멤버는 기반클래스로 만들자
class VectorBase
{
protected:
    int sz;
public:
    int size() const {}
    bool empty() const {}
};

template<typename T> class Vector : public VectorBase
{
    T* buff;
public:
    void push_front(const T& a) {}
    T& front() {}
};

// 8개의 함수로 줄어 든다.
```

```cpp
// 아에 모든 멤버를 기반클래스로 올린다면?
class VectorBase
{
protected:
    T* buff;
    int sz;
public:
    int size() const {}
    bool empty() const {}
    void push_front(const void* a) {}
    void* front() {}
};

// void*를 바로 쓰면 캐스팅을 불편함이 있다.
// 캐스팅을 책임지는 파생 클래스를 만든다.
template<typename T> class Vector : public VectorBase
{
public:
    inline void push_front(const T& a) {
        VectorBase::push_front(static_cast<void*>(a));
    }
    inline T& front() {
        return static_cast<T&>(VectorVase::front());
    }
};
```
