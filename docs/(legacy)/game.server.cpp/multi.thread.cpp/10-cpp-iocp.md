---
layout: default
title: "10. TLS(Thread Local Storage)"
parent: "(C++) Multi Thread"
grand_parent: "Game Server 👾"
great_grand_parent: "Legacy Archive"
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* TLS 란?
    * Thread마다 각자의 스택영역이 존재하며
    * **Heap(new), 데이터(static)**은 공유해서 쓴다.
    * 하지만 여기서 빠진 영역이 하나있는데 그 영역이 **Thread Local Storage(TLS)**이다.
* TLS는 어떠한 경우에 쓸까?
    * Heap, 데이터 영역에 데이터를 항상 읽어쓰기에는 **경합이 자주발생해 속도가 느릴**수 있다.
    * TLS에 Heap, 데이터 영역의 데이터를 **미리 복사해 놓고** 사용하게 된다.
* 그럼 스택과 무슨차이인가?
    * 스택은 함수내에서 사용하는 불안정적인 데이터이다.
    * 한 Thread에서 사용할 데이터의 저장은 TLS에 저장해 두는것이 맞다

```cpp
// __declspec(thread) int32 value;  // 이전방식
thread_local int32 LThreadId = 0;   // TLS 선언

void ThreadMain(int32 threadId)
{
    LThreadId = threadId;
    while(true)
    {
        cout << "Hi I am Thread " << LThreadId << endl;
        this_thread::sleep_for(1s);
    }
}

int main()
{
    vector<thread> threads;
    for(int32 i = 0; i < 10; i++)
    {
        int32 threadId = i + 1;
        threads.push_back(thread(ThreadMain, threadId));
    }
    for(thread& t : threads)
        t.join();
}
```

---

* 조금더 예제를 원할경우? 👉 [예전 정리자료 🌏](https://easycoding-7.github.io/blog/cpp/concurrency-new/9/#/)

```cpp
#include <iostream>
#include <thread>
#include <string_view>

int next3times()
{
    // 싱글스레드일때는 정상적으로 동작
    static int n = 0;
    n = n + 3;
    return n;
}

void foo(std::string_view name)
{
    std::cout << name << " : " << next3times() << std::endl;
    std::cout << name << " : " << next3times() << std::endl;
    std::cout << name << " : " << next3times() << std::endl;
}

int main()
{
    foo("A");
    // 만약 멀티스레드환경이라면?
    std::thread t1(foo, "A");
    std::thread t2(foo, "\tB");
    t1.join();
    t2.join();
    // 결과부터 말하면 3 6 9 12 15 18 이나오게된다.
    // 만약 스레드별로 3 6 9 를 출력하고 싶다면?
    // 각각 스레드별로 변수를 갖고싶다(TLS : Thread Local Storage) : 스레드별로 스택을 만들어줘
}
```

* 예전에는 이렇게 TLS를 사용했다.
    * linux(gcc) : `_thread static int x`
    * windows(cl) : `__declspec(thread)`
    * C++표준이 나오고 부터 : `thread_local`
    
```cpp
int next3times()
{
    thread_local int n = 0;
    // thread_local static int n = 0;
    // static이 암시적으로 선언이 되기에 생략해도 된다.
    n = n + 3;
    return n;
}
```
