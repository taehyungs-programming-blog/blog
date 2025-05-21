---
layout: default
title: "5-0. Coroutine 이란?"
parent: (C++20)
grand_parent: (C++)
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## coroutine이란?

```

Foo() --------> void Foo()
                    |
                    |
                    |
        <모든 Foo의 동작을 마치고>
                    |
<------------------- <다시 main으로 돌아간다.>

* 이런 루틴(routine)을 sub-routine이라한다.

```

```

Foo() --------> void Foo()
                    |
                    |
<-------------------
|
|
-------------------->
                    |
                    |
<-------------------

* 중간중간 main으로 다시 돌아가며 마치 협업을 하듯 함수를 호출한다.
* 이런 루틴(routine)을 co-routine이라 한다.

```

## coroutine의 필요성

* Coroutine은 보통 어디에 쓰일까?

```cpp
void VeryDifficultFunction()
{
    for(int i = 0; i < 100000; i++)
    {
        //... 무거운 작업을 반복적으로 하는 함수
    }
}
// 이렇게 한 번 부르기도 부담스러운 함수가 존재한다고 할때
VeryDifficultFunction();
// 이 함수가 하나의 Thread를 점유하는 것은 부담스러운 일 일수있다.
    // 여기서 아이디어가 나오는데 매우 무거운 함수를 나눠서 실행할 수 있다면??
    // 100000번을 함수 내부에서 돌아야하는데 1000번씩 나눠서 돌릴수 없나?
```

* 기존 C++에서는 불가능! But Coroutine(C++20)에서는 가능해졌다.
* 그런데 이런 경우가 많이 있는가?? 좀 더 효율적인 예제는 없나?

```cpp
void KillMonster()
{
    /*
        몬스터 처치시 아래의 과정이 필요하다
        1. 몬스터 처치
        2. 아이템 획득시 DB에 처리요청 <-- 여기가 매우 느림
        3. 아이템 생성 및 인벤토리 추가
    */

    // * 아이템 획득시 DB에 처리요청
    // 이 과정을 Coroutine으로 처리하고자 하는데 자세한것은 뒤에서 설명
}
```

* 가령 예를 들면 이런식이다.

```cpp
#include <future>  // std::async, std::future를 사용하기 위함
#include <iostream>

// 가정: DB 처리를 비동기적으로 수행하는 함수
std::future<void> ProcessItemAcquisitionInDB() {
    // DB 처리를 비동기적으로 실행하고, 그 결과를 나타내는 future를 반환합니다.
    return std::async(std::launch::async, []() {
        std::this_thread::sleep_for(std::chrono::seconds(2)); // DB 처리 시간 가정
        std::cout << "아이템 DB 처리 완료!" << std::endl;
    });
}

// 가정: 코루틴으로 처리할 비동기 함수
    // 정확히는 co_return을 쓰기위해선 std::future<void> 을 쓸수 없음.
    // 우선 여기선 받아들이자.
auto ProcessItemAcquisition() -> std::future<void> {
    // DB에 아이템 획득 처리
    co_await ProcessItemAcquisitionInDB();
    
    // 아이템 생성 및 인벤토리 추가 로직
    std::cout << "아이템을 인벤토리에 추가했습니다." << std::endl;

    co_return;
}

void KillMonster() {
    std::cout << "몬스터 처치!" << std::endl;

    // 아이템 획득 처리 시작
    auto future = ProcessItemAcquisition();
    
    // 여기서 다른 처리를 계속할 수 있음
    std::cout << "다른 작업 진행중..." << std::endl;
    
    // 필요하다면 아이템 획득 처리의 완료를 기다림
    future.get();
}

int main() {
    KillMonster();
    return 0;
}
```

---

## coroutine의 종류

* 그럼 coroutine을 어떻게 쓰는지 살펴보자.
* coroutine은 아래 세 함수 중 하나라도 사용할 경우 coroutine이 된다.
    - `co_return`
    - `co_yield`
    - `co_await`

---

## 어떻게 coroutine이 동작하는걸까?

```cpp
// 원래 코드
Task AsyncTask(std::string name, int delay_ms) {
    std::cout << name << " 코루틴 시작\n";
    co_await DelayAwaiter{...};
    std::cout << name << " 중간 처리\n";
    co_await DelayAwaiter{...};
    std::cout << name << " 코루틴 완료\n";
}

// 컴파일러가 변환한 코드 (개념적으로)
Task AsyncTask(std::string name, int delay_ms) {
    // 1. 코루틴 프레임 생성 (상태 저장용)
    struct Frame {
        std::string name;
        int delay_ms;
        int resume_point = 0;
        Task::promise_type promise;
        // 기타 지역 변수들...
    };
    
    Frame* frame = new Frame{name, delay_ms};
    
    // 2. promise 객체 설정 및 Task 반환
    Task task = frame->promise.get_return_object();
    
    // 3. initial_suspend 처리
    if (!frame->promise.initial_suspend().await_ready()) {
        // 중단 로직...
        return task; // 호출자에게 Task 반환
    }
    
    // 4. 코루틴 본문 실행
    try {
        switch (frame->resume_point) {
            case 0: // 처음 시작
                std::cout << frame->name << " 코루틴 시작\n";
                
                // 첫 번째 co_await 처리
                {
                    auto awaiter = DelayAwaiter{...};
                    if (!awaiter.await_ready()) {
                        frame->resume_point = 1; // 다음 재개 지점 설정
                        awaiter.await_suspend(coroutine_handle_from_frame(frame));
                        return task; // 호출자에게 반환 (중요!)
                    }
                    awaiter.await_resume();
                }
                // fall through
                
            case 1: // 첫 번째 co_await 이후 재개 지점
                std::cout << frame->name << " 중간 처리\n";
                
                // 두 번째 co_await 처리
                {
                    auto awaiter = DelayAwaiter{...};
                    if (!awaiter.await_ready()) {
                        frame->resume_point = 2; // 다음 재개 지점 설정
                        awaiter.await_suspend(coroutine_handle_from_frame(frame));
                        return task; // 호출자에게 반환 (중요!)
                    }
                    awaiter.await_resume();
                }
                // fall through
                
            case 2: // 두 번째 co_await 이후 재개 지점
                std::cout << frame->name << " 코루틴 완료\n";
                break;
        }
    } catch (...) {
        frame->promise.unhandled_exception();
    }
    
    // 5. final_suspend 처리
    frame->promise.final_suspend();
    
    return task;
}
```

---

## Example

```cpp
// 콜백 지옥 피하기

#include <iostream>
#include <coroutine>
#include <functional>
#include <thread>
#include <chrono>

// Task<T> 구현
template<typename T>
class Task {
public:
    struct promise_type {
        T result;
        std::exception_ptr exception;

        Task get_return_object() {
            return Task(std::coroutine_handle<promise_type>::from_promise(*this));
        }
        
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        
        void return_value(T value) { result = value; }
        
        void unhandled_exception() {
            exception = std::current_exception();
        }
    };

    explicit Task(std::coroutine_handle<promise_type> h) : handle(h) {}
    
    ~Task() {
        if (handle) handle.destroy();
    }
    
    T get_result() {
        if (handle.promise().exception)
            std::rethrow_exception(handle.promise().exception);
        return handle.promise().result;
    }

private:
    std::coroutine_handle<promise_type> handle;
};

// 비동기 작업을 위한 Awaiter 템플릿
template<typename T>
class AsyncOperationAwaiter {
public:
    AsyncOperationAwaiter(std::function<void(std::function<void(T)>)> operation, 
                         const std::string& name)
        : operation(operation), name(name) {}

    bool await_ready() const { return false; }

    void await_suspend(std::coroutine_handle<> h) {
        std::cout << name << " 시작\n";
        
        // 비동기 작업 시작
        operation([h, this](T result) {
            this->result = result;
            std::cout << name << " 완료, 결과: " << result << "\n";
            h.resume(); // 코루틴 재개
        });
    }

    T await_resume() const { return result; }

private:
    std::function<void(std::function<void(T)>)> operation;
    std::string name;
    T result;
};

// Operation1Async 구현 - 콜백 버전
void Operation1Async(std::function<void(int)> callback) {
    std::thread([callback]() {
        // 비동기 작업 시뮬레이션
        std::this_thread::sleep_for(std::chrono::seconds(1));
        callback(100); // 결과 반환
    }).detach();
}

// Operation1Async 코루틴 버전
Task<int> Operation1Async() {
    // 콜백 버전을 Awaiter로 래핑
    co_return co_await AsyncOperationAwaiter<int>(
        [](std::function<void(int)> callback) {
            Operation1Async(callback);
        },
        "Operation1"
    );
}

// Operation2Async 구현 - 콜백 버전
void Operation2Async(int input, std::function<void(int)> callback) {
    std::thread([input, callback]() {
        // 비동기 작업 시뮬레이션
        std::this_thread::sleep_for(std::chrono::seconds(1));
        callback(input * 2); // 결과 반환
    }).detach();
}

// Operation2Async 코루틴 버전
Task<int> Operation2Async(int input) {
    // 콜백 버전을 Awaiter로 래핑
    co_return co_await AsyncOperationAwaiter<int>(
        [input](std::function<void(int)> callback) {
            Operation2Async(input, callback);
        },
        "Operation2"
    );
}

// 코루틴 사용 예시
Task<int> DoWorkAsync() {
    std::cout << "DoWorkAsync 시작\n";
    auto result1 = co_await Operation1Async();
    std::cout << "result1: " << result1 << "\n";
    
    auto result2 = co_await Operation2Async(result1);
    std::cout << "result2: " << result2 << "\n";
    
    co_return result2 + 42;
}

// 콜백 사용 예시
void DoWorkAsyncCallback(std::function<void(int)> callback) {
    std::cout << "DoWorkAsyncCallback 시작\n";
    Operation1Async([callback](int result1) {
        std::cout << "result1: " << result1 << "\n";
        Operation2Async(result1, [callback](int result2) {
            std::cout << "result2: " << result2 << "\n";
            callback(result2 + 42);
        });
    });
}

int main() {
    std::cout << "코루틴 버전 실행\n";
    auto task = DoWorkAsync();
    
    // 콜백 버전 실행
    std::cout << "콜백 버전 실행\n";
    DoWorkAsyncCallback([](int result) {
        std::cout << "콜백 최종 결과: " << result << "\n";
    });
    
    // 코루틴 결과 대기
    std::this_thread::sleep_for(std::chrono::seconds(3));
    int result = task.get_result();
    std::cout << "코루틴 최종 결과: " << result << "\n";
    
    // 프로그램이 바로 종료되지 않도록
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return 0;
}
```

* 콜백 피하자고 이걸 적용?

---

## Example2

* 병렬작업이 필요한 케이스 가장 많이 사용된다.

```cpp
// 순차 실행 (비효율적)
UserData userData = co_await FetchUserDataAsync();       // 완료까지 1초
ProductData productData = co_await FetchProductDataAsync(); // 완료까지 1초
AnalyticsData analyticsData = co_await FetchAnalyticsDataAsync(); // 완료까지 1초
// 총 소요 시간: 약 3초

// 병렬 실행 (효율적)
Task<UserData> userTask = FetchUserDataAsync();          // 즉시 반환, 백그라운드에서 실행
Task<ProductData> productTask = FetchProductDataAsync();    // 즉시 반환, 백그라운드에서 실행
Task<AnalyticsData> analyticsTask = FetchAnalyticsDataAsync(); // 즉시 반환, 백그라운드에서 실행

UserData userData = co_await userTask;
ProductData productData = co_await productTask;
AnalyticsData analyticsData = co_await analyticsTask;
// 총 소요 시간: 약 1초 (가장 오래 걸리는 작업의 시간)
```

```cpp
// HTTP 클라이언트 예시
class HttpClient {
public:
    Task<HttpResponse> GetAsync(std::string url) {
        // 소켓 연결 (비동기)
        Socket socket = co_await ConnectAsync(url);
        
        // HTTP 요청 보내기 (비동기)
        co_await socket.SendAsync("GET " + url + " HTTP/1.1\r\nHost: example.com\r\n\r\n");
        
        // 응답 받기 (비동기)
        std::string responseData = co_await socket.ReceiveAsync();
        
        // 응답 파싱
        HttpResponse response = ParseResponse(responseData);
        
        co_return response;
    }
    
private:
    Task<Socket> ConnectAsync(std::string url) {
        // DNS 조회 (비동기)
        std::string ip = co_await DnsResolveAsync(url);
        
        // 소켓 생성
        Socket socket;
        
        // 연결 (비동기)
        co_await socket.ConnectAsync(ip, 80);
        
        co_return socket;
    }
};

// 실제 FetchUserDataAsync 구현
Task<UserData> FetchUserDataAsync() {
    HttpClient client;
    
    // 비동기 HTTP 요청
    HttpResponse response = co_await client.GetAsync("https://api.example.com/users/current");
    
    // JSON 파싱 (CPU 작업이지만 대용량 데이터의 경우 비동기로 처리할 수 있음)
    UserData userData = co_await JsonParser::ParseAsync<UserData>(response.body);
    
    co_return userData;
}
```