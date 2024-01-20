---
layout: default
title: "03. Lock 개념, SpinLock"
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

---

## 배우기 전, 선 질문

* Spin(Sleep), Event Lock중 뭘 써야할까?

* Event-based Lock :
    * 장점: CPU 자원을 절약합니다. 스레드가 락을 기다리는 동안 대기 상태로 들어가 다른 스레드나 프로세스가 CPU 자원을 사용할 수 있습니다.
    * 단점: Context switch로 인한 오버헤드가 있습니다. 스레드 상태가 실행에서 대기로 변경되는 데 비용이 들며, 이는 특히 락이 자주 해제되는 상황에서 부담이 될 수 있습니다.
* Spinlock:
    * 장점: Context switch가 없어 빠릅니다. 락을 획득할 때까지 스레드가 계속 실행 상태를 유지합니다.
    * 단점: CPU 자원을 지속적으로 소모합니다. 락을 기다리는 동안 스레드는 계속해서 CPU 시간을 소비하여, 다른 중요한 작업에 자원을 할당할 수 없게 됩니다.
* 결정 요소:
    * 대기 시간의 길이: 락을 기다리는 시간이 짧다면 spinlock이 효율적일 수 있습니다. 반면, 락을 기다리는 시간이 길다면 event-based lock이 더 적합합니다.
    * 락의 경합 정도: 락에 대한 경합이 심하지 않다면 spinlock이 더 나을 수 있습니다. 하지만 많은 스레드가 락을 경합한다면 event-based lock이 더 나을 수 있습니다.
    * 사용 빈도와 성능 요구 사항: 빈번하게 호출되며 높은 성능이 요구된다면 spinlock을 고려할 수 있습니다. 하지만 성능보다는 자원 절약이 중요하다면 event-based lock을 선택하는 것이 좋습니다.

---

* 이제 하나하나 구현방법에 대해 설명

## SpinLock, + volatile 이란?

* 아래를 SpinLock으로 구현해보자

```cpp
int32 sum = 0;
mutex m;

void Add()
{
    // 여기서 10'000번 돌면서(Spin) Lock을 탐지
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
    * locked가 volatile(컴파일러가 매번 체크하지 않음)하지 못하다
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
