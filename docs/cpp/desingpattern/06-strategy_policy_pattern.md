---
layout: default
title: "6. Strategy, Policy Pattern 비교"
parent: (Desing Pattern)
grand_parent: C++
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 간단 정리

* **Strategy Pattern**
    * 속도 : 가상함수 기반으로 느리다.
    * 유연성 : 바로바로 교체가능(setStategy())
    * 특정 함수의 구현을 강제화 할 수 있다.(interface를 통하여)
* **Policy Base Design**
    * 속도 : 인라인 치환으로 빠르다
    * 유연성 : 컴파일 시간 정책 교체, 실행 시간에 교체 불가

---

## Example (Policy Base Design)

```cpp
#include <iostream>
using namespace std;

template<typename T> class List
{
public:
    void push_front(const T& a)
    {
        // ...
    }
};

List<int> s;        // 전역으로 둘 경우 멀티 스레드에 안전하지 못함.

int main()
{
    s.push_front(10);
}
```

```cpp
template<typename T> class List
{
public:
    void push_front(const T& a)
    {
        Lock()
        // DoSomeThing
        UnLock()
    }
};
```
🍅 이게 맞나 ...<br>
🍅 뭐 멀티스레딩 문제는 해결되겠지만, 멀티스레드를 사용하지 않는 유저입장에서는 엄청난 자원 손실이 된다.<br>
🍅 **Strategy Pattern**을 이용하여 멀티스레딩 지원을 사용자가 선택할 수 있게 하도록 변경해 보자.<br>

```cpp
struct ISync
{
    virtual void Lock() = 0;
    virtual void UnLock() = 0;
    virtual ~ISync() {}
};

template<typename T> class List
{
    ISync* pSync = 0;
public:
    void setSync(ISync* p) { pSync = p };
    void push_front(const T& a)
    {
        if( pSync != 0) pSync->Lock();
        // DoSomeThing
        if( pSync != 0) pSync->UnLock();
    }
};

class MutexLock : public IStnc
{
public:
    virtual void Lock() {}
    virtual void UnLock() {}
};

List<int> s;

int main()
{
    MutexLock m;
    s.setSync(&m);

    s.push_front(10);
}
```

🍅 위 코드에서 문제점은 `push_front`가 자주 호출되면 `pSync->Lock/UnLock`을 포인터 접근하는데 있어 오버헤드가 있다는 점이다.
이를 해결해 본다.

```cpp
template<typename T, typename ThreadModel> class List
{
    ThreadModel tm; // 동기화 정책
public:
    void push_front(const T& a)
    {
        tm.Lock();
        // DoSomeThing
        tm.UnLock();
    }
};

class MutexLock
{
public:
    inline void Lock() {}       // 인라인으로 접근할 경우 오버헤드가 없다
    inline void UnLock() {}
};

List<int, MutexLock> s;

int main()
{
    s.push_front(10);
}
```

🍅 정책 클래스를 템플릿기반으로 만들수 있다.<br>
🍅 이를 **단위 전략 디자인(Policy Base Design)** 패턴이라 한다.<br>