---
layout: default
title: "23. notify, wait"
parent: (C++)
grand_parent: C++
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* **C++11 이후 기술**임을 주의

* 이벤트 개체이다. 신호를 받을 때까지 **현재 쓰레드의 실행을 멈춘다.**
* `notify_one()`, `notify_all()` : 멈춰 놓은 쓰레드 하나 또는 전부를 다시 실행시킴
* `wait()`, `wait_for()`, `wait_until()` : 조건 변수의 조건을 충족시킬때까지 또는 일정 시간 동안 **현재 쓰레드의 실행을 멈춤**
`std::unique_lock`을 사용해야 함
    * 기본적으로 scoped_lock 생성시에 lock을 잠그지 않을 수도 있음(두 번째 매개변수로 std::defer_lock을 전달할 것)
    * std::recursive_mutex와 함께 써서 재귀적으로 잠글 수 있음
    * 조건 변수에 쓸 수 있는 유일한 lock

```cpp
// Example - 아래 코드의 문제를 분석해 보자.
#include <iostream>
#include <mutex>
#include <queue>

static std::mutex sQueueLock;
static std::condigion_variable sEvent;
static std::queue<int> sQueue;

void Consume()
{
    while(true)
    {
        int val;
        {
            std::unique_lock<std::mutex> lock(sQueueLock);
            sEvent.wait(lock);

            val = sQueue.front();
            sQueue.pop();
        }

        std::cout << val << std::endl;
    }
}

void Produce()
{
    std::unique_lock<std::mutex> lock(sQueueLock);
    sQueue.push(1);

    sEvent.notify_one();
}

int main()
{
    std::thread producer(Produce);
    std::thread consumer(Consume);

    producer.join();
    consumer.join();

    return 0;
}
```

* `wait()` 함수는 현재 쓰레드 뮤텍스의 잠금을 풀고 `notify_one()`, `notify_all()`을 기다린다. 깨어나면 다시 뮤텍스를 잠근다.
* 위 코드의 문제점은 `notift_xxx()`가 `wait()`보다 먼저 호출되면 해당 쓰레드는 영원히 기다리는데 있다.

```cpp
// 해결책
#include <iostream>
#include <mutex>
#include <queue>

static std::mutex sQueueLock;
static std::condigion_variable sEvent;
static std::queue<int> sQueue;

void Consume()
{
    while(true)
    {
        int val;
        {
            std::unique_lock<std::mutex> lock(sQueueLock);
            sEvent.wait(lock, []{return !sQueue.empty();});
            // wait를 걸때 조건을 걸어둔다.

            val = sQueue.front();
            sQueue.pop();
        }

        std::cout << val << std::endl;
    }
}

void Produce()
{
    std::unique_lock<std::mutex> lock(sQueueLock);
    sQueue.push(1);

    sEvent.notify_one();
}

int main()
{
    std::thread producer(Produce);
    std::thread consumer(Consume);

    producer.join();
    consumer.join();

    return 0;
}
```