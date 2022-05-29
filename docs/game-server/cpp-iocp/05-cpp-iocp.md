---
layout: default
title: "5. Event Lock"
parent: "(C++ IOCP)"
grand_parent: C++
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Event

😺 이벤트로 쓰레드 점유를 확인하고 점유한다.

```cpp
#include <windows.h>

mutex m;
queue<int32> q;

void Producer()
{
    while(ture)
    {
        {
            unique_lock<mutex> lock(m);
            q.push(100);
        }

        this_thread::sleep_for(1000000000000ms);
        // 데이터를 밀어 넣는쪽이 드물게 발생한다고 가정해보자.
    }
}

void Consumer()
{
    while(true)
    {
        // 어차피 가져갈 데이터는 어쩌다 한 번들어오는데
        // 여기서 무한대기하는 것이 옳은 일인가?
        unique_lock<mutex> lock(m);
        if(q.empty() == false)
        {
            int32 data = q.front();
            q.pop();
            cout << data << endl;
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

---

## Event를 이용해서 Thread를 컨트롤해보자

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

        ::SetEvent(handle); // Event를 Signal상태로
        this_thread::sleep_for(1000000000000ms);
    }
}

void Consumer()
{
    while(true)
    {
        // Event가 Signal 상태이면 진행, Non-Signal 상태이면 무한정지
        ::WaitForSingleObject(handle, INFINITE);

        // 메뉴얼 리셋이 FALSE라 다시 Non-Signal 상태로 돌아가게 된다.

        unique_lock<mutex> lock(m);
        if(q.empty() == false)
        {
            int32 data = q.front();
            q.pop();
            cout << data << endl;
        }
    }
}

int main()
{
    // 커널 오브젝트이며
    // Signal / Non-Signal 두 가지 상태를 갖는다
    // Auto / Manual 상태를 갖는다
    handle = ::CreateEvent(NULL     /*보안속성 : 보통 Null*/, 
                        FALSE       /*메뉴얼 리셋*/, 
                        FALSE       /*초기상태*/, 
                        NULL        /*이름 : 보통 Null*/);

    std::thread t1(Producer);
    std::thread t2(Consumer);

    t1.join();
    t2.join();

    ::CloseHandle(handle);
}
```