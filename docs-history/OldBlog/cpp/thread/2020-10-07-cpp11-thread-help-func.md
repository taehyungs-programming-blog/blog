---
title: "(C++11) thread 도우미 함수"
permalink: cpp/cpp11-thread/help-func/                # link 직접 지정
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
  - help-func
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

네임스페이스 그룹<br>
현재 쓰레드에 적용되는 도우미 함수들이 있음<br>

```cpp
get_id()
sleep_for()
sleep_until()
yield()
```

```cpp
#include <iostream>
#include <chrono>
#include <string>
#include <thread>

void PrintCurrentTime() {/* 현재 시간 출력 */}

void PrintMessage(const std::string& message)
{
    std::cout << "Sleep now ... ";
    PrintCurrentTime();

    std::this_thread::sleep_for(std::chrono::seconds(3));
    // thread 3초 자라

    std::cout << message << " ";
    PrintCurrentTime();
}

int main() {
    std::thread thread(PrintMessage, "Message from a child thread");
    
    PrintMessage("Waiting the child thread");

    thread.join();

    return 0;
}
```

**다른 쓰레드에게 양보하기**

```cpp
auto end = std::chrono::high_resolution_clock::now() + delay;

while(std::chrono::high_resolution_clock_now() < end)
{
    std::this_thread::yield();
}
```