---
layout: default
title: "5-1. co_return"
parent: (C++20)
grand_parent: (C++)
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## co_return

* 일단 그냥 써보자!

```cpp
// coroutine으로 리턴을 하면 coroutine이 될까?
void HelloCoroutine()
{
    // 이렇게 사용하면 coroutine이 되는 것인가?
    co_return;  // Error! - 현재 리턴형이 void인데
                // coroutine을 쓰기 위해선 coroutine 구조체를 리턴해야함
}
```

* ?!?! 무슨소리지? 
    * 아직 C++20의 coroutine이 그리 완벽하지 못하다. C++20에서 요구하는 coroutine의 형태로 만들어서 사용해야 한다...;
* 그럼 **coroutine 구조체**를 만들어 보자

```cpp
#include <coroutine>

// ...

struct CoroutineObject
{
/*
    * promise 객체는 다음과 같은 인터페이스를 제공해야 함
        * 요구하는 인터페이스의 경우는 co_return, co_yield, co_await에 따라 다르기에 그때그때 에러뜨는것을 보고 만들면 된다.

    - get_return_object : 코루틴 객체를 반환 (resumable object)
    - return_value(val) : `co_return XXX;` 으로 리턴할 변수를 담는 함수.
    - return_void() : `co_return;` 리턴이 void일 경우 처리할 부분을 담는 함수.
    - yield_value(val) : `co_yield XXX;` 으로 잠시 보유할 변수를 담는 함수.
    - initial_suspend() : 코루틴이 실행 전에 중단/연기될 수 있는지
    - final_suspend() : 코루틴이 종료 전에 중단/연기될 수 있는지
    - unhandled_exception() : 예외 처리시 호출됨
*/

    // 내부에 struct promise_type를 무조건 포함해야한다.
    struct promise_type
    {
        CoroutineObject get_return_object() { return {}; }
        std::suspend_never initial_suspend() const noexcept { return {}; }
        std::suspend_never final_suspend() const noexcept { return {}; }
        void return_void() { }
        void unhandled_exception() { }
    };
};

// ...

CoroutineObject HelloCoroutine()
{
    // 이제 이렇게 쓸 수 있다.
    co_return;
}
```

---

## co_return의 필요성 (Future, Promise랑 뭔 차이인가?)

* `co_return`은 다음과 같은 이유로 필요합니다:
    * 값 반환: 코루틴이 결과를 생성할 때, 그 값을 호출자에게 전달할 수 있습니다.
    * 종료 신호: 코루틴의 논리적 끝을 명시적으로 표시하고, 자원을 정리하고 코루틴의 생명주기를 관리할 수 있습니다.

```cpp
#include <iostream>
#include <coroutine>
#include <future>

// 사용자 정의 코루틴 반환 타입
template<typename T>
struct SimpleFuture {
    struct promise_type {
        T value;
        std::promise<T> p;
        SimpleFuture get_return_object() {
            return SimpleFuture{.h = std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        std::suspend_never initial_suspend() const noexcept { return {}; }
        std::suspend_never final_suspend() const noexcept { return {}; }
        void return_value(T v) {
            value = v;
            p.set_value(v);
        }
        void unhandled_exception() { std::rethrow_exception(std::current_exception()); }
        std::future<T> get_future() { return p.get_future(); }
    };
    std::coroutine_handle<promise_type> h;
};

// 비동기적으로 숫자를 두 배로 만드는 코루틴 함수
SimpleFuture<int> DoubleValue(int x) {
    co_return x * 2;  // 결과를 반환
}

int main() {
    auto future = DoubleValue(10).get_future();  // 코루틴 실행
    std::cout << "DoubleValue 결과: " << future.get() << std::endl;  // 결과 출력
    return 0;
}
```

* 여기까지 보면 future, pormise랑 뭐가 차이인가 싶다.
    * 이후 await이 나오면 corotine의 진가가 나온다.

---

## co_return의 내부동작 설명 (컴파일러 입장)

* 그럼 `co_return;`의 동작을 알아야 하는데
* `co_return;`이 작성되면 컴파일러는 내부적으로 아래와 같은 코드를 만들어 준다.

```cpp
CoroutineObject HelloCoroutine()
{
    co_return;
}

/*
CoroutineObject HelloCoroutine()
{
    // promise를 생성후
    Promise prom; (get_return_object)   // get_return_object를 호출해 coroutine 객체를 생성한다.

    // 이후 promise의 initial_suspend를 호출 등등
    co_await prom.initial_suspend();
    try
    {
        // co_return, co_yield, co_await를 포함하는 코드
    }
    catch (...)
    {
        prom.unhandled_exception();
    }
    co_await prom.final_suspend();


    // 보면 모두 CoroutineObject를 생성하면 만들었던 함수들이다.
}
*/
```

* 그런데 코드는 많이 적었는데 뭘 한 건지는 전혀모르겠다.
* 아주 정상임 이제 위 코드에 기능을 하나하나 추가할 예정

```cpp
Future<int> CreateFuture()
{
    // 리턴을 받는 Future를 만들어보자
    co_return 2021;
}
```

```cpp
template<typename T>
class Future
{
public:
    Future(shared_ptr<T> value) : _value(value) { }
    T get() { return *_value; }

private:
    shared_ptr<T> _value;

public:
    struct promise_type
    {
        // 리턴을 받아야하기에 return_value(T value)를 생성
        void return_value(T value) { *_ptr = value; }

        // 나머지는 기존과 동일하다고 볼 수 있다.
        Future<T> get_return_object() { return Future<T>(_ptr); }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void unhandled_exception() { }

        // 데이터
        shared_ptr<T> _ptr = make_shared<T>();
    };
};
```

* 실제사용은 어떻게 할까?

```cpp
int main() 
{
    // coroutine객체를 생성하고 ...
    auto future = CreateFuture();
    /*
        Future<int> CreateFuture()
        {
            co_return 2021;
        }
    */

    // TODO : 다른걸 하다...
    // 이후에 호출이 가능
    cout << future.get() << endl;
}
```

* 앞서 `co_return` 를 컴파일러가 어떻게 변경해 주는지 배웠으니

```cpp
// co_return이 호출되면 컴파일러에 의해 생성되는 코드는 아래이며 설명을 붙이자면
{
    Promise prom; (get_return_object)
    /*
        get_return_object 는,
        Future<T> get_return_object() { return Future<T>(_ptr); } 이기에
        Future 객체가 생성 그리고 _ptr = _value가 된다.
    */

    co_await prom.initial_suspend();
    try
    {
        // 여기에 co_return 2021; 가 들어가고
        // co_return 2021;는
            // void return_value(T value) { *_ptr = value; } 가 호출됨
            // _ptr에 value(2021)이 들어가게 된다.
    }
    catch (...)
    {
        prom.unhandled_exception();
    }
    co_await prom.final_suspend();
}
```

---

## 짤막 co_await 설명

* `co_await prom.initial_suspend()`의 설명을 위해서 co_await의 설명이 필요하다

```cpp
// co_await 플로우(아래 코드는 C++코드가 아니라 의사코드임 헷갈리지 말기)
{
    // await_ready() : 준비가 되었는지 안되었는지 확인
    /*
        준비여부는
            std::suspend_never initial_suspend() { return {}; }
            std::suspend_never final_suspend() noexcept { return {}; }
        * std::suspend_never - 무조건 넘어가달라
        * std::suspend_always - 무조건 멈춰달라
    */
    if awaitable.await_ready() returns false;   // await_ready()가 false(준비가 안됨)일 경우 아래를 실행
        suspend coroutine
        awaitable.await_suspend(handle) returns:    // coroutine을 suspend(잠시멈춰라)
            // awaitable.await_suspend(handle)의 리턴형이 void, bool, another에 따라서 별도 처리
            void:
                // 리턴이 void일 경우
                awaitable.await_suspend(handle);
                coroutine keeps suspended   // coroutine을 suspend해두고
                return to caller            // 호출자를 다시 호출한다
            bool:
                bool result = awaitable.await_suspend(handle);
                if (result)
                    coroutine keeps suspended
                    return to caller
                else
                    return awaitable.await_resume()
            another coroutine handle:
                anotherCoroutineHandle = awaitable.await_suspend(handle);
                anotherCoroutineHandle.resume();
                return to caller;
    return awaitable.await_resume();
}
```