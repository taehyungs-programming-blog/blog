---
layout: default
title: "22. C++11 이후 thread"
parent: (C++)
grand_parent: C++
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## (C++11 이전) Thread : WIN32 Thread

```cpp
// Windwos
#include <Windows.h>

DWORD WINAPI PrintMessage()
{
    // ...
}

int main() {
    DWORD myThreadID;

    HANDLE myHandle = CreateThread(0, 0, PrintMessage, NULL, 0, &myThreadID);

    WaitForSingleObject(myHandle, INFINITE);
    // thread가 종료될때 까지 기다린다.

    CloseHandle(myHandle);

    return 0;
}
```

```cpp
// POSIX(pthread)
#include <pthread.h>

void *printMessage()
{
    // ...
}

int main() {
    pthread_t thread = 0;

    int result_code = pthread_create(&thread, NULL, printMessage, NULL);
    result_code = pthread_join(thread, NULL);

    return 0;
}
```

---

## (C++11 이전) 멀티쓰레딩

* C++11 전까지 **표준 멀티쓰레딩 라이브러리가 없음**
* **OS마다 멀티쓰레딩 구현이 달랐음**

* 리눅스/유닉스 : `POSIX(pthread)`
* 윈도우 쓰레드
    * 윈도우에서 pthread를 사용할순 있음(ver 1003.1만)
* **불편하네… 표준 Thread를 만들어 보자**

---

## (C++11 이후) 

```cpp
// Example 1
#include <iostream>
#include <string>
#include <thread>

void PrintMessage(const std::string& message)
{
    std::cout << message << std::endl;
}

int main() {
    std::thread thread(PrintMessage, "Message from a child thread");
    
    PrintMessage("Waiting the child thread");

    // 떼어 내기
    thread.detach();

    // 다시 붙이고 싶다면?
    if(thread.joinable())
    {
        thread.join();
    }

    return 0;
}
```

```cpp
// Example 2
#include <iostream>
#include <string>
#include <thread>


int main() {
    auto PrintMessage = [](const std::string& message)
    {
        std::cout << message << std::endl;
    };

    std::thread thread(PrintMessage, "Message from a child thread");
    
    PrintMessage("Waiting the child thread");

    thread.join();

    return 0;
}
```

```cpp
// Example 3
#include <iostream>
#include <string>
#include <thread>
#include <vector>

int main() {
    std::vector<int> list(100, 1);
    int result = 0;

    std::thread thread([](const std::vector<int>& v, int& result)
    {
        for( auto item : v )
        {
            result += item;
        }
    }, list, std::ref(result));

    thread.join();

    std::cout << "Result: " << result << std::endl;

    return 0;
}
```