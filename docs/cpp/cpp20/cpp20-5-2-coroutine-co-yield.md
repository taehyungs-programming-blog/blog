---
layout: default
title: "5-2. co_yield"
parent: (C++20)
grand_parent: (C++)
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## co_yield

* 다음은 co_yield를 어떻게 사용되는지 보자.
* co_yield는 이전상황을 계속 저장하며 **정지/실행**을 반복하는 것을 의미한다.

```cpp
Generator<int> GenNumbers(int start = 0, int delta = 1)
{
    int now = start;
    while (true)
    {
        co_yield now; // yield return - 여기까지 돌고 리턴된다
        now += delta;
    }
}

//...

auto numbers = GenNumbers(0, 1);
for (int i = 0; i < 20; i++)
{
    numbers.next();
    cout << " " << numbers.get();
}
```

```cpp
template<typename T>
class Generator
{
public:
    struct promise_type;
    using handle_type = coroutine_handle<promise_type>;

    Generator(handle_type handle) : _handle(handle)
    {

    }

    ~Generator()
    {
        if (_handle)
            _handle.destroy();
    }

    T get() { return _handle.promise()._value; }

    bool next()
    {
        _handle.resume();       // 중요) coroutine 재개
        return !_handle.done();
    }

private:
    handle_type _handle;        // coroutine의 handle을 갖고있는다

public:
    struct promise_type
    {
        // Generator(handle_type::from_promise(*this)); 이런식으로 핸들을 넘길 수 있는데 이 부분은 받아들이자
        Generator<T> get_return_object() { return Generator(handle_type::from_promise(*this)); }
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        // co_yield가 호출되면 yield_value가 호출됨.
        std::suspend_always yield_value(const T value) { _value = value; return {}; }
        std::suspend_always return_void() { return {}; }
        void unhandled_exception() { }
        T _value;
    };
};
```

* 이걸 현실에 반영하기는 쉽지는 않을듯... 코드를 읽는 수준에서 보자

---

## Example

```cpp
#include <iostream>
#include <coroutine>
#include <vector>

template<typename T>
struct Generator {
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

    struct promise_type {
        T current_value;
        static auto get_return_object_on_allocation_failure() { return Generator{nullptr}; }
        auto get_return_object() { return Generator{handle_type::from_promise(*this)}; }
        auto initial_suspend() { return std::suspend_always{}; }
        auto final_suspend() noexcept { return std::suspend_always{}; }
        void return_void() {}
        void unhandled_exception() { std::exit(1); }
        auto yield_value(T value) {
            current_value = value;
            return std::suspend_always{};
        }
    };

    handle_type coro;
    Generator(handle_type h) : coro(h) {}
    ~Generator() { if (coro) coro.destroy(); }

    bool move_next() { coro.resume(); return !coro.done(); }
    T current_value() { return coro.promise().current_value; }
};

Generator<int> CreateSequence(int start, int step) {
    for (int i = start;; i += step) {
        co_yield i;  // 현재 값을 생성하고 일시 중지
    }
}

int main() {
    auto seq = CreateSequence(0, 2);  // 0부터 시작하여 2씩 증가하는 시퀀스 생성
    for (int i = 0; i < 5; ++i) {
        seq.move_next();
        std::cout << "Generated value: " << seq.current_value() << std::endl;
    }
    return 0;
}
```