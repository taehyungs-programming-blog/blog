---
layout: default
title: "41. std::async vs coroutine 비교"
parent: "(C++) 문법"
grand_parent: (C++)
nav_order: 7
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 메모리 사용량과 컨텍스트 스위칭

```cpp
// 각 std::async 호출마다 새로운 스레드가 생성됩니다
void example_async() {
    std::vector<std::future<int>> futures;
    
    // 1000개의 작업을 생성한다고 가정
    for (int i = 0; i < 1000; i++) {
        futures.push_back(std::async(std::launch::async, []() {
            // 각 작업은 새로운 스레드에서 실행
            return heavyComputation();
        }));
    }
}
```

* std::async 메모리 영향:
    * 각 스레드는 기본적으로 1MB 정도의 스택 메모리를 할당받습니다
    * 1000개 스레드 = 약 1GB 메모리 사용
    * 운영체제의 스레드 컨텍스트 정보도 추가로 필요

```
Thread 1: 실행 중 -----> 대기
Thread 2: 대기 -----> 실행 중 -----> 대기
Thread 3: 대기 -----> 실행 중
...
```

* 운영체제가 스레드 간 전환할 때마다 CPU 레지스터 상태를 저장/복원
* 캐시 미스 발생 가능
* 실제 작업 외의 오버헤드 발생

```cpp
// C++20 코루틴 예제
generator<int> coroutine_example() {
    for (int i = 0; i < 1000; i++) {
        // 작업 수행
        auto result = heavyComputation();
        co_yield result;  // 제어권을 일시적으로 반환
    }
}

void use_coroutine() {
    auto gen = coroutine_example();
    for (auto value : gen) {
        // 값 처리
    }
}
```

* 메모리 영향:
    * 코루틴 프레임당 수백 바이트 정도의 작은 메모리만 사용
    * 1000개 코루틴 = 수백 KB 정도의 메모리 사용
    * 동일한 스레드에서 실행되므로 추가 스택 메모리 불필요

```
Coroutine 1: 실행 -> 중단점(co_yield) -> 
Coroutine 2: 실행 -> 중단점(co_yield) ->
...
```

* 같은 스레드 내에서 전환되므로 OS 수준의 컨텍스트 스위칭 없음
* 단순히 함수 호출처럼 동작
* 캐시 효율성이 더 좋음

```cpp
// 웹 요청을 처리하는 예제

// std::async 버전 - 각 요청마다 새 스레드
void handle_requests_async(const std::vector<Request>& requests) {
    std::vector<std::future<Response>> futures;
    for (const auto& req : requests) {
        futures.push_back(std::async(std::launch::async, [&req]() {
            return processRequest(req);  // 새 스레드에서 실행
        }));
    }
    // 많은 메모리 사용, 빈번한 컨텍스트 스위칭
}

// 코루틴 버전 - 단일 스레드에서 효율적으로 처리
task<void> handle_requests_coroutine(const std::vector<Request>& requests) {
    for (const auto& req : requests) {
        auto response = co_await processRequestAsync(req);
        // 다른 코루틴이 실행될 기회를 제공하면서
        // 동일 스레드에서 계속 실행
    }
}
```

```cpp
// 코루틴을 사용한 웹 서버 요청 처리
task<void> handle_client_connection(socket_t client_socket) {
    while (true) {
        // 클라이언트로부터 데이터 읽기
        auto request = co_await async_read(client_socket);
        if (request.empty()) break;

        // 데이터베이스 쿼리
        auto db_result = co_await async_query_database(request);

        // 외부 API 호출
        auto api_result = co_await async_call_external_api(db_result);

        // 결과를 클라이언트에게 전송
        co_await async_write(client_socket, api_result);
    }
}

// 여러 클라이언트 동시 처리
task<void> server_main() {
    while (true) {
        auto client = co_await accept_connection();
        // 새로운 클라이언트마다 코루틴 생성
        handle_client_connection(client); // 스레드 생성 없이 처리
    }
}
```

---

## 결론

* I/O 바운드 작업에서 스레드 블로킹으로 인한 자원 낭비를 방지하고, 단일 스레드 내에서 효율적인 컨텍스트 스위칭을 통해 동시성을 구현하기 위해 사용됩니다.