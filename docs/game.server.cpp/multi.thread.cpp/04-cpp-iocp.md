---
layout: default
title: "04. Sleep Lock (Spin + Sleep)"
parent: "(C++) Multi Thread"
grand_parent: "Game Server 👾"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Sleep

* Sleep은 운영체제의 스케쥴링과 연관이 된다.
* SpinkLock에서 Sleep만 추가해 주면 된다.

```cpp
class SpinLock
{
public:
    void lock()
    {
        bool expected = false;
        bool desired = true;

        // 여기서 무한대기를 하며 CPU점유률을 올리게 된다.
        while(_locked.compare_exchange_strong(expected, desired) == false)
        {
            expected = false;
        }
    }

    void unlock()
    {
        _locked.store(false);
    }

private:
    atomic<bool> _locked = false;
};
```

```cpp
class SpinLock
{
public:
    void lock()
    {
        bool expected = false;
        bool desired = true;

        while(_locked.compare_exchange_strong(expected, desired) == false)
        {
            expected = false;

            // 다른 Thread에게 소유권을 넘긴다.
            // this_thread::sleep_for(std::chrono::milliseconds(100));
            this_thread::sleep_for(100ms);  // 위와 동일 표현
            this_thread::yield();

            // sleep_for or yield 둘 중 뭘 써야하나?
            // yield == sleep_for(0ms) 동일 표현 얼마를 쉰다의 차이뿐이다.
        }
    }

    void unlock()
    {
        _locked.store(false);
    }

private:
    atomic<bool> _locked = false;
};
```