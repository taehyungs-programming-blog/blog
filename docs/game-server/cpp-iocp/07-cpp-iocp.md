---
layout: default
title: "7. future & promise"
parent: "(C++ IOCP)"
grand_parent: "Game Server 👾"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Future는 언제 쓸까? 

😺 단발성 이벤트의 처리 방안 Thread를 쓰자니 데이터 관리나 Thread만들기에는 그리 대단한 작업은 아니고... 안쓰자니 느리고... 이럴때 쓴다 **어떻게 보면 쓰임세가 적긴함(가볍게 보자)**

```cpp
#include <future>

// Calculate()라는 연산량이 많은 함수가 있다고 가정해보자.
int64 Calculate()
{
    int64 sum = 0;
    for(int32 i = 0; i < 100'000; i++)
        sum += i;
    return sum;
}

int main()
{
    // 동기(Synchrononous) 실행 방식이라한다.
    Calculate();    // 이 함수를 모두 끝나고 다음줄이 실행됨.
    // 단, Calculate이 엄청나게 오래걸리는 함수라면? -> 비동기 방식의 호출이 필요해 진다.
}
```

```cpp
thread t(Calculate);
t.join();               // 이렇게 하면될까?
// 문제1. sum을 받기위해서 공용데이터(thread를 나누면 return으로 데이터를 받을 길이 없음)를 써야하며 안정성에 의문이 든다.
// 문제2. 단순 함수호출을 위해 thread를 생성하는 것이 정말 옳은일인가 싶다
```

😺 좀 더 가볍게 처리해보자. -> **이게 Future 사용의 핵심이라 봐도 무방**

```cpp
std::fucture<int64> future = std::async(std::launch::async, Calculate); // 여길호출하면, Calculate이 끝날때까지 여기서 정지해 있지 않고 다음줄로 넘어간다.
// do something ...
int64 sum = future.get();   // 결과물은 이렇게 받을 수 있음.
```

* `future`의 옵션
    * `deferred` -> 지연해서 실행(get을 호출시 실행, 지연실행일 뿐이지 multi thread가 아님)
    * `async` -> 별도의 쓰레드를 만들어 실행(실질적 multi thread)
    * `deferred | async` -> 둘 중 알아서 컴파일러가 결정해 주세요

```cpp
std::future_status status = future.wait_for(1ms);
if(status == future_status::ready)  // 완료되었나 확인이 가능
{
}

// ...

future.wait();  // 완료되기를 대기할 수도 있다.
```

😺 주의할 점은 특정 객체의 함수를 Future로 호출시

```cpp
class Myclass
{
public:
    int64 GetHp() {/*do something*/}
};

Myclass mc;
std::future<int64> future = std::async(std::launch::async, &Myclass::GetHp, mc);
```

---

## promise

😺 두 번째 future사용법, std::promise를 이용해 thread의 리턴값 쉽게 받기

```cpp
void PromiseWorker(std::promise<string>&& promise)
{
    promise.set_value("Secret Message");
}

// ...

// 미래에 결과물을 반환해줄꺼라 약속
// promise에 데이터가 set되면 미래에 get으로 데이터를 받을 수 있다.
std::promise<string> promise;
std::future<string> future = promise.get_future();
thread t(PromiseWorker, std::move(promise));    // move를 통해 다른 thread에 권한을 이전
string message = future.get();                  // future도 get을 호출시 삭제된다.(딱 한 번만 쓸 수 있음)
// 여기서 get이 된다는 말 자체가 set_value가 끝났다는 말임.
t.join();
```

```cpp
int64 Calculate()
{
    int64 sum = 0;
    for(int32 i = 0; i < 100'000; i++)
        sum += i;
    return sum;
}

// ...

void TaskWorker(std::packaged_task<int64(void)>&& task)
{
    task();
}

//...

// std::packaged_task<함수아웃풋(함수인풋)>
std::packaged_task<int64(void)> task(Calculate);
    // 이러면 async로 thread를 만드는 것과 동일하지 않나?
    // packaged_task는 개발자 자신이 thread를 만드는데 하나의 thread를 생성 후 여러 packaged_task를 처리하게 할 수도 있다.
    // 하나의 Thread에 일감(packaged_task)를 넘길 수 있으니 Thread를 만들었다 지웠다 할 필요가 없음
std::future<int64> future = task.get_future();
std::thread t(TaskWorker, std::move(task));
int64 sum = future.get();
t.join();
```

* 결론
    * `mutex`, `condition_variable`까지 가지않고 간단하게 `thread`를 사용할 수 있다.
    * 단발성 이벤트의 경우 `Future`를 사용하는 것이 오히려 쉬울수 있음.

* 또 하나 중요한 점은 비동기와 멀티쓰레드는 다르다는 것
    * 비동기 : 순차적으로 실행되진 않는다.
    * 멀티쓰레드 : 동시에 여러작업이 가능하다.