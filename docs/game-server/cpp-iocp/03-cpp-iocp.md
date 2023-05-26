---
layout: default
title: "[이론] Lock 개념, SpinLock"
parent: "(C++) Multi Thread"
grand_parent: "Game Server 👾"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Lock 구현방법?

* Lock의 종류는?

* 무작정 기다린다. 👉 **SpinLock**
    * 컨텍스트 스위칭에 대한 비용이 작다.
    * 컨텍스트 스위칭은 래지스터에 저장되며 꽤 비용이 부담스럽다.
* 특정 시간을 대기 후 다시 mutex를 확인한다. 👉 **Sleep**
* 다른 매니저를 두고 mutex상태를 보고 받는다. 👉 **Event**

* 이제 하나하나 구현방법에 대해 설명

---

## SpinLock

* 아래를 SpinLock으로 구현해보자

```cpp
int32 sum = 0;
mutex m;

void Add()
{
    for(int32 i = 0; i < 10'000; i++)
    {
        lock_guard<mutex> guard(m);
        sum++;
    }
}

void Sub()
{
    for(int32 i = 0; i < 10'000; i++)
    {
        lock_guard<mutex> guard(m);
        sum--;
    }
}

int main()
{
    thread t1(Add);
    thread t2(Sub);

    t1.join();
    t2.join();
}
```

```cpp
int32 sum = 0;
mutex m;

class SpinLock
{
public:
    void lock()
    {
        // 만약 다른곳에서 lock를 해뒀다면 여기서 걸리게 된다.
        while(_locked)
        {

        }

        _locked = true;
    }

    void unlock()
    {
        _locked = false;
    }

private:
    bool _locked = false;
};

void Add()
{
    for(int32 i = 0; i < 10'000; i++)
    {
        lock_guard<SpinLock> guard(m);
        sum++;
    }
}

void Sub()
{
    for(int32 i = 0; i < 10'000; i++)
    {
        lock_guard<SpinLock> guard(m);
        sum--;
    }
}

int main()
{
    thread t1(Add);
    thread t2(Sub);

    t1.join();
    t2.join();
}
```

* 일단 문제가 두 가지 이다.
    * locked가 volatile하지 못하다
    * locked자체가 multi-thread safe하지 못하다

```cpp
// 1. SpinLock 변수에 volatile선언을 해줘야한다.
// volatile : 최적화를 하지말라

int32 a = 0;
a = 1;
a = 2;
a = 3;
a = 4;
// 컴파일러는 최적화를 하며 a = 4를 바로 넣게된다.

private:
    volatile bool _locked = false;
    // 만약 여기서 volatile을 선언하지 않으면 매번 컴파일러가 _locked를 체크하지 않는다
```

```cpp
// 2. thread t1, t2가 거의 동시에 들어와 값을 써버리는 경우가 발생한다.
// 이런경우 컴파일러에서 제어가 불가능하다
// atomic하게 접근이 되어야 한다.

class SpinLock
{
public:
    void lock()
    {
        bool expected = false;  // locked가 unlock일 경우 이 값이여라
        bool desired = true;    // lock이 될경우 locked를 이 값으로 변경해라

        /*
        // compare_exchange_strong 의사코드
        // 아래의 의사코드가 atomic하게 들어가게 됨.
        if(_locked == expected) 
        {
            // lock을 획득
            expected = _locked;
            _locked = desired;
            return true
        }
        else
        {
            // lock획득 실패 대기해 달라
            expected = _locked;
            return false;
        }
        */

        while(_locked.compare_exchange_strong(expected, desired) == false)
        {
            expected = false;
        }
        // 참고로 이런 SpinLock을 사용하는 이유는 이 메모리의 접근을 곧 할수 있을거라는 기대때문이다.
        // 굳이 컨텍스트 스위칭의 로스를 갖지않기 위해서 SpinLock을 사용하게 된다.
        // 단, CPU점유율이 높아진다.
    }

    void unlock()
    {
        _locked.store(false);
    }

private:
    atomic<bool> _locked = false;
};
```