---
title: "(c++20) coroutine"
permalink: cpp/cpp20/coroutine/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2021-01-09 00:00:00 -0000
last_modified_at: 2021-01-09 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
  - c++20
category:
  - coroutine
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

## coroutine concept

sub-routine과 coroutine

* sub-routine : 함수가 호출되고 함수가 모두 실행된 다음 함수 다음의 코드가 실행
* co-routine : 함수가 호출되고 함수내에서 대기하는 동안 함수 다음의 코드가 실행 다시 함수가 호출되면 대기하는 코드 아래부터 다시 실행

C++ coroutine이 좀 어러운게 가장 간단하게 만드려해도 최소 아래정도는 만들어야 corouine이 동작한다.

```cpp
#include <iostream>
#include <coroutine>
using namespace std;

struct Generator
{
    struct Promise
    {
        int value;
        Generator get_return_object()
        {
            return Generator{ std::coroutine_handle<Promise>::from_promise(*this) };
        }
        auto initial_suspend() { return std::suspend_always{}; }
        auto yield_value(int n) { value = x; return std::suspend_always{}; }
        auto return_void() { return std::suspend_never{}; }
        auto final_suspend() { return std::suspend_always{}; }
        void unhandled_exception() { std::exit(1); }
    };
    using promise_type = Promise;
    coroutine_handle<Promise> coro;

    Generator(coroutine_handle<Promise> h) : coro(h) {}
    ~Generator() { if (coro) coro.destroy(); }
    int value() { return coro.promise().value; }
    bool next() { coro.resume(); return !coro.done(); }
};

Generator foo(int n)
{
    std::cout << "\tRun 1 : " << std::endl;

    // 다시 caller에게 돌아가라
    co_await std::suspend_always{};

    std::cout << "\nRun 2" << std::endl;

    // 다시 caller에게 돌아가라
    co_await std::suspend_always{};

    std::cout << "\nRun 3" << std::endl;
}

int main()
{
    Generator f = foo(10);
    std::cout << "main 1 : " << std:endl;

    f.next();       // Run 1 실행
    std::cout << "main 2 : " << std:endl;

    f.next();       // Run 2 실행
    std::cout << "main 3 : " << std:endl;

    f.next();       // Run 3 실행
}
```

모두 동일한 Thread로 동작함이 핵심!

---

## 실제로 만들어 보자

```cpp
#include <iostream>
#include <coroutine>

void foo()
{
    std::cout << "Run 1" <<std::endl;

    co_await std::suspend_always {};    // caller에게 돌아가 달라
    // 단, 현재 빌드하면 promise에러가 발생

    std::cout << "Run 2" <<std::endl;
}

int main()
{
    foo();
}
```

```cpp
#include <iostream>
#include <coroutine>

class Generator
{
public:
    struct Promise
    {
        Generator get_return_object()
        {
            return Generator{ std::coroutine_handle<Promise>::from_promise(*this) };
        }
        auto initial_suspend() { return std::suspend_always{}; }
        auto return_void() { return std::suspend_never{}; }
        auto final_suspend() { return std::suspend_always{}; }
        void unhandled_exception() { std::exit(1); }
    };
    using promise_type = Promise;
    std::coroutine_handle<promise_type> coro;

    Generator(std::coroutine_handle<promise_type> c) : coro(c) {}
    ~Generator() { if(coro) coro.destroy(); }
};

Generator foo()
{
    std::cout << "Run 1" <<std::endl;

    co_await std::suspend_always {};
    // 여기까지 만들면 빌드가능!

    std::cout << "Run 2" <<std::endl;
}

int main()
{
    foo();
}
```

그래도 아직 실행은 안됨

```cpp
#include <iostream>
#include <coroutine>

class Generator
{
public:
    struct Promise
    {
        Generator get_return_object()
        {
            return Generator{ std::coroutine_handle<Promise>::from_promise(*this) };
        }
        auto initial_suspend() { return std::suspend_always{}; }
        auto return_void() { return std::suspend_never{}; }
        auto final_suspend() { return std::suspend_always{}; }
        void unhandled_exception() { std::exit(1); }
    };
    using promise_type = Promise;
    std::coroutine_handle<promise_type> coro;

    Generator(std::coroutine_handle<promise_type> c) : coro(c) {}
    ~Generator() { if(coro) coro.destroy(); }
};

Generator foo()
{
    std::cout << "Run 1" <<std::endl;

    co_await std::suspend_always {};

    std::cout << "Run 2" <<std::endl;
}

int main()
{
    Generator g = foo();

    std::cout << "\tmain1" << std::endl;
    g.coro.resume();

    std::cout << "\tmain2" << std::endl;
    g.coro.resume();

    std::cout << "\tmain3" << std::endl;
}
```

---

## co_yield

```cpp
#include <iostream>
#include <coroutine>

class Generator
{
public:
    struct Promise
    {
    private:
        int value;
    public:
        int getValue() { return value; }
        Generator get_return_object()
        {
            return Generator{ std::coroutine_handle<Promise>::from_promise(*this) };
        }
        auto yield_value(int n)
        {
            value = n;
            return std::suspend_always{};
        }
        auto initial_suspend() { return std::suspend_always{}; }
        auto return_void() { return std::suspend_never{}; }
        auto final_suspend() { return std::suspend_always{}; }
        void unhandled_exception() { std::exit(1); }
    };
    using promise_type = Promise;
    std::coroutine_handle<promise_type> coro;

    Generator(std::coroutine_handle<promise_type> c) : coro(c) {}
    ~Generator() { if(coro) coro.destroy(); }
};

Generator foo()
{
    std::cout << "Run 1" <<std::endl;

    co_yield 10;        // 10을 넘기고 싶다면?

    std::cout << "Run 2" <<std::endl;
    co_yield 20;
}

int main()
{
    Generator g = foo();

    std::cout << "\tmain1" << std::endl;
    g.coro.resume();

    std::cout << g.coro.promise().getValue() << std::endl;      // 10

    std::cout << "\tmain2" << std::endl;
    g.coro.resume();

    std::cout << g.coro.promise().getValue() << std::endl;      // 20

    std::cout << "\tmain3" << std::endl;
}
```

## 좀더 진화시켜보자

```cpp
#include <iostream>
#include <coroutine>

template<typename T>
class Generator
{
public:
    struct Promise
    {
    private:
        T value;
    public:
        const T& getValue() { return value; }
        Generator get_return_object()
        {
            return Generator{ std::coroutine_handle<Promise>::from_promise(*this) };
        }
        auto yield_value(const T& n)
        {
            value = n;
            return std::suspend_always{};
        }
        auto initial_suspend() { return std::suspend_always{}; }
        auto return_void() { return std::suspend_never{}; }
        auto final_suspend() { return std::suspend_always{}; }
        void unhandled_exception() { std::exit(1); }
    };
    using promise_type = Promise;
    std::coroutine_handle<promise_type> coro;

    Generator(std::coroutine_handle<promise_type> c) : coro(c) {}
    ~Generator() { if(coro) coro.destroy(); }
};

Generator<int> Gen(int first, int last)
{
    for(int i = first; i <= last; i++)
        co_yield i;
}

int main()
{
    Generator<int> g = Gen(10, 20);

    while(1)
    {
        g.coro.resume();
        if(g.coro.done()) break;
        std::cout << g.coro.promise().getValue() << std::endl;
    }
}
```

좀 더 진화

```cpp
#include <iostream>
#include <coroutine>

template<typename T>
class Generator
{
public:
    struct Promise
    {
    private:
        T value;
    public:
        const T& getValue() { return value; }
        Generator get_return_object()
        {
            return Generator{ std::coroutine_handle<Promise>::from_promise(*this) };
        }
        auto yield_value(const T& n)
        {
            value = n;
            return std::suspend_always{};
        }
        auto initial_suspend() { return std::suspend_always{}; }
        auto return_void() { return std::suspend_never{}; }
        auto final_suspend() { return std::suspend_always{}; }
        void unhandled_exception() { std::exit(1); }
    };
    using promise_type = Promise;
    std::coroutine_handle<promise_type> coro;

    Generator(std::coroutine_handle<promise_type> c) : coro(c) {}
    ~Generator() { if(coro) coro.destroy(); }

    class Iter
    {
    public:
        void operator++() { coro.resume(); }
        const T operator*() const { return coro.promise().getValue(); }
        bool operator==(std::default_sentiel_t) const
        {
            return !coro || coro.done();
        }
        explicit Iter(std::coroutine_handle<promise_type> coroutine) : coro{corotine}{}

    private:
        std::corotine_handle<promise_type> coro;
    };

    Iter begin() { if(coro) { coro.resuem(); } return Iter{coro}; }
    std::default_sentinel_t end() { return {}; }
};

Generator<int> Gen(int first, int last)
{
    for(int i = first; i <= last; i++)
        co_yield i;
}

int main()
{
    Generator<int> g = Gen(10, 20);

    for( auto n : g )
        std::cout << n << std::endl;
}
```

---

## awaitable object

```cpp
#include <iostream>
#include <coroutine>
#include <thread>

class Generator
{
public:
    struct Promise
    {
        Generator get_return_object()
        {
            return Generator{ std::coroutine_handle<Promise>::from_promise(*this) };
        }
        auto initial_suspend() { return std::suspend_always{}; }
        auto return_void() { return std::suspend_never{}; }
        auto final_suspend() { return std::suspend_always{}; }
        void unhandled_exception() { std::exit(1); }
    };
    using promise_type = Promise;
    std::coroutine_handle<promise_type> coro;
    Generator(std::coroutine_handle<promise_type> c) : coro(c) {}
    ~Generator() { if(coro) coro.destroy(); }
};

struct resume_new_thread
{
    void await_suspend(std::coroutine_handle<> handle)
    {
        std::thread t([handle]() {handle().resume();});
        t.detach();
    }
    constexpr bool wait_ready() const noexcept { return false; }
    constexpr void await_resume() const noexcept {}
}

Generator foo()
{
    std::cout << "Run 1" <<std::endl;

    // co_await std::suspend_always {};

    // std::suspend_never awaiter;
    // co_await awaiter;   // never로 선언시 Run 2까지 다 실행 됨

    co_await resume_new_thread{};       
    // 주 Thread는 Run1까지 실행
    // Run2부터는 새로운 Thread로 실행

    std::cout << "Run 2" <<std::endl;
}

int main()
{
    Generator g = foo();
    g.coro.resume();
    std::cout << "main" <<std::endl;

}
```