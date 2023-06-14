---
title: "(C++11) mutex"
permalink: cpp/cpp11-thread/mutex/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-01-06 00:00:00 -0000
last_modified_at: 2020-10-06 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
  - thread
category:
  - mutex
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

```cpp
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

void PrintMessage(const std::string& message)
{
    static std::mutex sMutex;
    // 물론 이런식으로 static을 쓰는건 좋지 못한 습관

    sMutex.lock();
    std::cout << message << std::endl;
    sMutex.unlock();
}

int main() {
    std::thread thread(PrintMessage, "Message from a child thread");
    
    PrintMessage("Waiting the child thread");

    thread.join();

    return 0;
}
```

## Mutex 흔히 하는 실수

```cpp
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

void PrintMessage(const std::string& message)
{
    static std::mutex sMutex;
    // 물론 이런식으로 static을 쓰는건 좋지 못한 습관

    sMutex.lock();
    std::cout << message << std::endl;
    // sMutex.unlock();
    // unlock을 안한경우
}

int main() {
    std::thread thread(PrintMessage, "Message from a child thread");
    
    PrintMessage("Waiting the child thread");

    thread.join();

    return 0;
}
```

## 해결책 : std::scoped_lock(C++17)

```cpp
void PrintMessage(const std::string& message)
{
    static std::mutex sMutex;
    // 물론 이런식으로 static을 쓰는건 좋지 못한 습관

    std::scoped_lock<std::mutex> lock(sMutex);
    std::cout << message << std::endl;
}
```

std::scoped_lock(C++17) 사용법 추가

```cpp
void PrintMessage(const std::string& message)
{
    static std::mutex sMutex;
    // 물론 이런식으로 static을 쓰는건 좋지 못한 습관

    // 다음과 같이 나눌 경우 scoped_lock을 별도로 걸 수 있다.
    {
        std::scoped_lock<std::mutex> lock(sMutex);
        std::cout << message << std::endl;    
    }

    {
        std::scoped_lock<std::mutex> lock(sMutex);
        std::cout << message << std::endl;    
    }
}
```