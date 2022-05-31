---
layout: default
title: "[이론] Memory model"
parent: "(C++) Multi Thread"
grand_parent: "Game Server 👾"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

😺 내용 정리가 생각보다 엉성해서 다시 정리한다.

* [과거정리자료 🌏](https://easycoding-7.github.io/blog/cpp/iocp/basic/6/#/)
* [과거정리자료2 🌏](https://easycoding-7.github.io/blog/cpp/concurrency-new/12/#/)
* [참고사이트 🌏](https://modoocode.com/271) : 내 설명보다 훨씬 좋음. 적극 참고할 것

---

## re-ordering problem

```cpp
int a = 0;
int b = 0;

// thread A
void foo()
{
    a = b + 1;
    b = 1;
}

// thread B
void goo()
{
    if(b == 1)
    {
        // a == 1 을 보장할수 있을까? -> 보장할순 없다.
        // 최적화 옵션에 따라 foo함수에서 b = 1이 먼저실행되기도 a = b + 1이 먼저 실행되기도 한다.
    }
}
```

😺 이게 무슨 소리지?? 👉 이게 re-ordering problem이다. CPU는 최적화를 위해 **코드를 재정렬** 하기도 한다.

```cpp
void foo()
{
    a = b + 1;  // a를 쓰기위해서 b을 읽어와야한다.
    b = 1;      // 어차피 여기서 b에 1을 넣어야 하기에
    // a = b + 1;에서 b을 읽어왔을때 b를 먼저 1을 넣어버린다면? -> re-ordering
    // 컴파일러 나름대로 성능향상을 볼 수 있을것이다.
    // 이것이 문제가 된다.
}
```

😺 해결방법은 없나? 👉 `fence` 활용

```cpp
#include <atomic>
// ...
void foo()
{
    a = b + 1;
    
    // 위 코드가 아래로 넘어갈수 없다.
    std::atomic_thread_fence(std::memory_order_release);
    b = 1;
}
```

---

## 또 다른 해결방안 (`memory_order`)

😺 우선 문제의 코드를 먼저보자.

```cpp
#include <thread>

int x = 0;
int y = 0;
// foo, goo를 볼때 
// 1. 스레드 세이프한가(atomic operation 가능)
// 2. re-ordering으로 코드의 순서가 변경될 일이 없는가
// 를 중점으로 확인해 보자

void foo()
{
    // 보면 알겠지만 atomic하지도 re-ordering이 나타지 않을 것이란 보장도 없다.
    // 멀티스레드에서는 사용하면 안되는 코드이다.
    int n1 = y;
    x = n1;
}

void goo()
{
    // foo와 동일
    int n2 = x;
    y = 100;
}

int main()
{
    std::thread t1(foo);
    std::thread t2(goo);
    t1.join(); t2.join();
}
```

### 해결책 1차

😺 memory_order 중 `std::memory_order_relaxed`를 써보자

```cpp
#include <thread>
#include <atomic>

std::atomic<int> x = 0;
std::atomic<int> y = 0;

void foo()
{
    int n1 = y.load(std::memory_order_relaxed);
    // std::memory_order_relaxed : atomic만 보장, re-ordering은 보장하지 않음, 단, 오버헤드가 가장 작다
    x.store(n1, std::memory_order_relaxed);
}

void goo()
{
    int n2 = x.load(std::memory_order_relaxed);
    y.store(100, std::memory_order_relaxed);
}

int main()
{
    std::thread t1(foo);
    std::thread t2(goo);
    t1.join(); t2.join();
}
```

😺 단적인 예로

```cpp
#include <thread>
#include <atomic>
#include <cassert>

std::atomic<int> data1 = 0;
std::atomic<int> data2 = 0;
std::atomic<int> flag = 0;

void foo()
{
    data1.store(100, std::memory_order_relaxed);
    data2.store(200, std::memory_order_relaxed);
    flag.store(1, std::memory_order_relaxed);
}

void goo()
{
    if(flag.load(std::memory_order_relaxed) > 0)
    {
        // re-ordering이 보장되지 않기에 assert이 발생할 수 있다.
        assert(data1.load(std::memory_order_relaxed) == 100);
        assert(data2.load(std::memory_order_relaxed) == 200);
    }
}

int main()
{
    std::thread t1(foo);
    std::thread t2(goo);
    t1.join(); t2.join();
}
```

---

### 해결책 2차

😺 그럼 re-ordering까지 막고싶다면? 👉 `std::memory_order_release`, `std::memory_order_acquire` 활용

```cpp
void foo()
{
    data1.store(100, std::memory_order_relaxed);
    data2.store(200, std::memory_order_relaxed);
    flag.store(1, std::memory_order_release);
    // std::memory_order_release 이전의 코드는 std::memory_order_acquire이후에 읽을수 있음을 보장
    // 무조건 data1.store(100, std::memory_order_relaxed);, data2.store(200, std::memory_order_relaxed); 실행을 보장
}

void goo()
{
    if(flag.load(std::memory_order_acquire) > 0)
    {
        // re-ordering이 보장되지 않기에 assert이 발생할 수 있다.
        assert(data1.load(std::memory_order_relaxed) == 100);
        assert(data2.load(std::memory_order_relaxed) == 200);
    }
}
```

### 해결책 3차

😺 뭐야 이거.. 무서워 간단히 쓸 방법은 없나?

```cpp
#include <thread>
#include <atomic>
#include <cassert>

std::atomic<int> data1 = 0;
std::atomic<int> data2 = 0;

int main()
{
    // std::memory_order_seq_cst : atomic, re-ordering 모두 보장해 달라
    data1.store(100, std::memory_order_seq_cst);
    data2.store(200, std::memory_order_seq_cst);
    data2.store(300);   // 디폴트가 std::memory_order_seq_cst이고 오버헤드가 가장 큼.
}
```

---

## 정리를 해 보자면?

* `std::memory_order_relaxed` - atomic은 보장, re-ordering 보장 못함
* `std::memory_order_release`, `std::memory_order_acquire` - atomic은 보장, re-ordering 부분 보장
* `std::memory_order_seq_cst` - atomic은 보장, re-ordering 보장

---

## 좀 더 어렵게 설명

```cpp
// atomic 선언
atomic<int64> num;

vod Thread_1()
{
    num.store(1);   // num = 1
}

vod Thread_2()
{
    num.store(2);   // num = 2
}

// ... 여러개의 Thread가 있다고 가정하자
void Thread_Observer()
{
    while(true)
    {
        int64 value = num.load();
        // 여기 사이에서 변할수 있지만... 이거는 일단 무시
        std::cout << value << std::endl;
        // 관찰 결과는?
    }
}
```

```
# 시간에 따라 atomic<int64> num이 아래와 같이 변경된다고 생각해보자.
-----(0)---(5)----(4)----(3)-------------<시간>
   ^            ^
   |            |
 (관찰1)      (관찰2)
```

* 관찰2 에서 현재 돌고있는 Thread는 atomic변수를 읽으니 모두 5라는 값을 리턴할까?
* 절대 아니다. -> why? 현재 cache된 변수가 어떤변수일지 보장이 없고 re-ordering또한 발생했을 수 있다.
* 단, 하나는 보장할 수 있다. -> 변수가 시간에 따라 저장되는 순서(0 -> 5 -> 4 -> 3 ...)

---

### 추가) atomic은 보장된다 말 할수 있나??

😺 그럼 atomic하게 CPU가 처리하고있는지 아닌지 어떻게 알지? 그냥 atomic만써주면 다 원자적으로 처리하는가?<br>
😺 당연히 아니다. 예를들어 32bits환경에선 64bits 변수를 atomic하게 데이터변경이 불가능(메모리주소를 2번 옮겨야함)<br>
😺 atomic하게 처리가능한지 확인할 방법(`is_lock_free`)이 있다.<br>

```cpp
// Example

atomic<int64> v;
cout << v.is_lock_free() << endl; 
// 1(True) - CPU 자체적 atomic 가능

struct Knight
{
    int32 level;
    int32 hp;
    int32 mp;
};

atomic<Knight> v;
cout << v.is_lock_free() << endl;   
// 0(False) - CPU 자체적 atomic 불가능
// 내부에서 lock을 하든 어떠한 방식으로 atomic하게 처리해줌.
```

* 추가의 결론
    * 변수가 시간에 따라 저장(변경)되는 순서는 보장 됐고
    * `is_lock_free`를 통해 atomic또한 보장된다면
    * 위의 memory_order만 지켜진다면 모든 thread가 특정시점(시간)에 같은 변수를 참조하게 만들수 있다는게 보장이 된다.