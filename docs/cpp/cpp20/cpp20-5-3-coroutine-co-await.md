---
layout: default
title: "5-3. co_await"
parent: (C++20)
grand_parent: (C++)
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## co_await

```cpp
Job PrepareJob()
{
    // co_await [Awaitable]
    co_await std::suspend_never();
    
    // 여기도 동일하게 corotine 의사코드가 오게되며
    /*
        co_await prom.initial_suspend();
        try
        {
            // std::suspend_never();
        }
        catch (...)
        {
            prom.unhandled_exception();
        }
        co_await prom.final_suspend();
    */
    // 이렇게 된다.
}

// ...

auto job = PrepareJob();
job.start();
```

```cpp
class Job
{
public:
    struct promise_type;
    using handle_type = coroutine_handle<promise_type>;

    Job(handle_type handle) : _handle(handle)
    {

    }

    ~Job()
    {
        if (_handle)
            _handle.destroy();
    }

    void start()
    {
        if (_handle)
            _handle.resume();
    }

private:
    handle_type _handle;
    
public:
    struct promise_type
    {
        Job get_return_object() { return Job(handle_type::from_promise(*this)); }
        std::suspend_always initial_suspend() { cout << "Prepare Job" << endl;  return {}; }
        std::suspend_never final_suspend() noexcept { cout << "Do Job" << endl; return {}; }
        std::suspend_never return_void() { return {}; }
        void unhandled_exception() { }
    };
};
```

```cpp
// Example

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

    Generator( std::coroutine_handle<promise_type> c ) : coro(c) {}

    ~Generator() { if ( coro ) coro.destroy();}
};

Generator foo()
{
    std::cout << "Run 1" << std::endl;
    co_await std::suspend_always {};
    std::cout << "Run 2" << std::endl;
}

int main()
{
    foo();
}
```

---

## Example

```cpp
#include <iostream>
#include <future>
#include <coroutine>

template<typename T>
struct Task {
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;
    handle_type coro;

    Task(handle_type h) : coro(h) {}
    ~Task() { if (coro) coro.destroy(); }

    T get() {
        coro.resume();
        return coro.promise().value;
    }

    struct promise_type {
        T value;
        Task<T> get_return_object() {
            return Task<T>{handle_type::from_promise(*this)};
        }
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_value(T v) { value = v; }
        void unhandled_exception() { std::exit(1); }
    };
};

Task<int> AsyncAdd(int a, int b) {
    auto result = co_await std::async(std::launch::async, [a, b] { return a + b; });
    co_return result;
}

int main() {
    auto task = AsyncAdd(10, 20);
    std::cout << "Result: " << task.get() << std::endl;
    return 0;
}
```

```cpp
Task<int> AsyncAdd(int a, int b) {
    // co_await이 생략되면 무슨일이 발생할까?
    auto result = std::async(std::launch::async, [a, b] { return a + b; });
    co_return result;   // result로 promise가 바로 나가버림.
}

int main() {
    auto task = AsyncAdd(10, 20);
    std::cout << "Result: " << task.get() << std::endl;
    // get()이 호출되는 순간에 async의 작업이 남아있다면 thread가 대기하게 된다.
    return 0;
}
```