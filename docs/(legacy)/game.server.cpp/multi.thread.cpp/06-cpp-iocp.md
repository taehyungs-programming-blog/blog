---
layout: default
title: "06. Condition Variable"
parent: "(C++) Multi Thread"
grand_parent: "Game Server 👾"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Event 방식의 문제점

* 기본적으로 Event방식이 Window에서만 사용이 된다.
* + 아래의 문제점도 있음

```cpp
#include <windows.h>

mutex m;
queue<int32> q;
HANDLE handle;

void Producer()
{
    while(ture)
    {
        {
            unique_lock<mutex> lock(m);
            q.push(100);
        }
        ::SetEvent(handle);
    }
}

void Consumer()
{
    while(true)
    {
        // 여기서 push가 들어가고

        ::WaitForSingleObject(handle, INFINITE);

        // 여기서 또 push가 들어갈 수 있음

        /* 
            결론만 말하자면 Event를 기다리는 부분과 
            lock을 잡는 부분이 atomic하지 못하니
            그 사이에서 multi-thread에 safe하지 못하다
        */

        unique_lock<mutex> lock(m);
        if(q.empty() == false)
        {
            int32 data = q.front();
            q.pop();

            // << 여기부터 읽으시오 >>
            // 여기서 size를 조사하면,
            // push, pop이 동작하니 항상 0~1사이값이 나올까?
            cout << q.size() << endl;

            // Nope 엄청큰수가 나올때가 있다.
            // -> mutex lock과 event과 묶여있지 않기에 순서에 의해 push가 더 많이 들어갈 수 있음.
        }
    }
}

int main()
{
    handle = ::CreateEvent(NULL, FALSE, FALSE, NULL);

    std::thread t1(Producer);
    std::thread t2(Consumer);

    t1.join();
    t2.join();

    ::CloseHandle(handle);
}
```

---

## Condition Variable

* Event에 기능이 추가된 개념!

```cpp
mutex m;
queue<int32> q;

// 여기서 부터 시작
// condition_varible은 표준이기에 그냥 사용이 가능!
// 참고, CV는 User-Level Object이다. (커널 오브젝트가 아니다)
condition_variable cv;


void Producer()
{
    while(ture)
    {
        // 1) Lock을 잡고
        // 2) 공유 변수 값을 수정
        // 3) Lock을 풀고
        // 4) CV를 통해 다른 쓰레드에게 통지(이벤트를 안씀)

        {
            unique_lock<mutex> lock(m);
            q.push(100);
        }
        
        // 여기선 lock이 풀리게 되고
        
        cv.notify_one(); // wait중인 쓰레드가 있으면 딱 하나만 깨운다.
    }
}

void Consumer()
{
    while(true)
    {
        unique_lock<mutex> lock(m);
        cv.wait(lock, [](){return q.empty() == false;}/*깨어나는 조건*/);
        // 1) Lock을 잡고
        // 2) 조건 확인
        // 조건 만족 -> 빠져 나와서 이어서 코드를 진행
        // 조건 불만족 -> lock을 풀어주고 대기 상태로 빠진다
        // 주의할 점은 일반 lock은 사용이 불가능(unique_lock만 사용가능, cv에서 lock을 풀수 없다)
        // 3) notify_one이 도착하면 1)부터 다시 시작

        // 그런데 notify_one을 받을꺼면, 굳이 깨어나는 조건을 정한 이유가 있나?
        // -> Spurious Wakeup(가짜 기상) : notify_one을 받았지만, 실제론 깨어날 이유가 없었을 수 있음 예를들어 q.empty인 경우

        // 결론적으로 event를 기다리며, 자기가 깨어날 이유가 없을 경우 스스로 스레드를 잠들게 한다.(좋구만)

        {
            int32 data = q.front();
            q.pop();
            cout << q.size() << endl;
        }
    }
}

int main()
{
    std::thread t1(Producer);
    std::thread t2(Consumer);

    t1.join();
    t2.join();
}
```