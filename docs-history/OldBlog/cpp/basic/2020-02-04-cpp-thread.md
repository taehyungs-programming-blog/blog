---
title: "(C++) thread"
date: 2020-02-04 00:00:00 -0000
---

* [cppreference](https://en.cppreference.com/w/cpp/thread)

> 기존에 OS별 별도로 돌던 thead를 하나로 합쳐보자!<br>
> 단, C++표준이기에 모든 OS에서 돌아가게 만들었다.<br>
> 그에 따른 쓰기 불편한 점이 몇 가지 존재한다..<br>

---

* 다른 코스에서 쓰레딩 프로그래밍에 대해 배운다.
* 그래서 C++에서 쓰레딩 라이브러리 사용법에 초점을 맞춤
* 특히
    * 쓰레드
    * 뮤텍스
    * 조건 변수
    * 더 있으나, 여기선 배우지 않는다.

---

## C++11 이전의 멀티 쓰레딩

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

```c
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

## C++11 이전의 멀티쓰레딩

* C++11 전까지 표준 멀티쓰레딩 라이브러리가 없음
* OS마다 멀티쓰레딩 구현이 달랐음
    * 리눅스/유닉스 : POSIX(pthread)
    * 윈도우 쓰레드
    * 윈도우에서 pthread를 사용할순 있음(ver 1003.1만)

---

## 목차

* [쓰레드 개체 만들기 (thread 생성)](#쓰레드-개체-만들기-(thread-생성))
* [쓰레드 ID 구하기](#쓰레드-ID-구하기)
* [쓰레드 떼어 내기](#쓰레드-떼어-내기)
* [람다와 쓰레딩](#람다와-쓰레딩)
* [매개변수를 참조로 전달](#매개변수를-참조로-전달)
* [도우미 함수들(this_thread)](#도우미-함수들(this_thread))
* [다른 쓰레드에게 양보하기](#다른-쓰레드에게-양보하기)

* [Mutex](#Mutex)
* [Mutex 흔히 하는 실수](#Mutex-흔히-하는-실수)

* [notify, wait](#notify,-wait)

---

### 쓰레드 개체 만들기 (thread 생성)

```cpp
#include <iostream>
#include <string>
#include <thread>

void PrintMessage(const std::string& message)
{
    std::cout << message << std::endl;
}

int main() {

    std::thread thread(PrintMessage, "Message from a child thread");

    PrintMessage("Message from a main thread");

    return 0;
}
```

> 단, 위의 예는 Main이 끝나도 Thread가 계속 돌아가는 위험이 있음

### 해결책

```cpp
#include <iostream>
#include <string>
#include <thread>

void PrintMessage(const std::string& message)
{
    std::cout << message << std::endl;
}

int main() {
    std::thread thread(PrintMessage, "Message from a child thread");

    PrintMessage("Message from a main thread");

    thread.join();

    return 0;
}
```

---

### 쓰레드 ID 구하기

```cpp
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

void PrintMessage(const std::string& message)
{
    std::cout << message << std::endl;
}

int main() {
    std::thread thread(PrintMessage, "Message from a child thread");

    std::thread::id childThreadOD = thread.get_id();
    std::stringstream ss;
    ss << childThreadID;
    std::string childThreadISStr = ss.str();
    
    PrintMessage("Waiting the child thread(ID : " + childThreadIDStr + ")");

    thread.join();

    return 0;
}
```

---

## 쓰레드 떼어 내기

```cpp
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

---

## 람다와 쓰레딩

```cpp
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

---

## 매개변수를 참조로 전달

```cpp
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

---

## 도우미 함수들(this_thread)

* 네임스페이스 그룹
* 현재 쓰레드에 적용되는 도우미 함수들이 있음
    * `get_id()`
    * `sleep_for()`
    * `sleep_until()`
    * `yield()`

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

---

## 다른 쓰레드에게 양보하기

```cpp
auto end = std::chrono::high_resolution_clock::now() + delay;

while(std::chrono::high_resolution_clock_now() < end)
{
    std::this_thread::yield();
}
```

---

## Mutex

> 공유 자원 잠그기

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

---

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

### 해결책 : std::scoped_lock(C++17)

> 기본적으로 unlock을 잘하는게 정답이고..

```cpp
void PrintMessage(const std::string& message)
{
    static std::mutex sMutex;
    // 물론 이런식으로 static을 쓰는건 좋지 못한 습관

    std::scoped_lock<std::mutex> lock(sMutex);
    std::cout << message << std::endl;
}
```

### std::scoped_lock(C++17) 사용법 추가

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

---

## notify, wait

* 이벤트 개체
* 신호를 받을 때까지 현재 쓰레드의 실행을 멈춘다.
* `notify_one()`, `notify_all()`
    * 멈춰 놓은 쓰레드 하나 또는 전부를 다시 실행시킴
* `wait()`, `wait_for()`, `wait_until()`
    * 조건 변수의 조건을 충족시킬때까지 또는 일정 시간 동안 현재쓰레드의 실행을 멈춤
* `std::unique_lock`을 사용해야 함

* 기본적으로 scoped lock
* 생성시에 lock을 잠그지 않을 수도 있음(두 번째 매개변수로 std::defer_lock을 전달할 것)
* std::recursive_mutex와 함께 써서 재귀적으로 잠글 수 있음
* 조건 변수에 쓸 수 있는 유일한 lock

### 다음 코드의 문제가 뭘까?

```cpp
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

* `wait()` 함수는 현재 쓰레드 뮤텍스의 잠금을 풀고 `notify_one()`, `notify_all()`을 기다린다.
* 깨어나면 다시 뮤텍스를 잠근다.

> 위 코드의 문제점은 `notift_xxx()`가 `wait()`보다 먼저 호출되면 해당 쓰레드는 영원히 기다리는데 있다.

### 올바른 wait 사용법

```cpp
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