---
layout: default
title: "02. C++ Thread 정리"
parent: "(C++) Multi Thread"
grand_parent: "Game Server 👾"
great_grand_parent: "Legacy Archive"
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## `std::thread` 생성

* 윈도우에 종속적인 API를 사용할수 있지만 C++11에서 추가된 std::thread를 이용해서 thread를 생성하는 것을 추천한다.

```cpp
#include <thread>

void HelloThread()
{
    cout <<" Hello Thread" << endl;
}

int main()
{
    // thread 생성
    std::thread t(HelloThread);

    cout << "Hello Main" << endl;

    // 이외에 여러함수가 지원됨.
    int32 count = t.hardwhare_concurrency();  // CPU코어 개수를 리턴
    auto id = t.get_id(); // thread id

    t.join();           // thread를 대기해준다.(이걸사용하지 않을경우 main이 먼저종료되어 Error처리됨)

    t.detach();         // main thread와 연결을 끊는다.(거의 사용안됨.)
    t.joinable();       // 연결(main thread에서 대기가능)이 가능한지 확인
}
```

* 여러 thread를 한 번에 생성해보자

```cpp
// 배개변수 받기, thread여러개 생성해보기
void HelloThread(int num)
{
    cout <<" Hello Thread" << num << endl;
}

int main()
{
    vector<std::thread> v;

    for(int32 i = 0; i < 10; i++)
    {
        v.push_back(std::thread(HelloThread, i));
    }

    for(int32 i = 0; i < 10; i++)
    {
        if(v[i].joinable())
            v[i].join();
    }
}
```

---

## atomic

* 우선 아래 문제를 생각해 보자.

```cpp
// 힙 or 데이터 영역의 변수
int32 sum = 0;

void Add()
{
    for(int32 i = 0; i < 1'000'000; i++)
    {
        sum++;
    }
}

void Sub()
{
    for(int32 i = 0; i < 1'000'000; i++)
    {
        sum--;
    }
}

int main()
{
    std::thread t1(Add);
    std::thread t2(Sub);

    t1.join();
    t2.join();

    cout << sum << endl;
    // 0이 안나오게 된다. -> Thread사용의 주의사항
}
```

* 해결은? 미리 언급했듯 Atomic을 활용한다
    * 단, atomic은 연산이 느리기에 절대적으로 필요한 경우에만 사용하자

```cpp
#include <atomic>

// 여러 Thread에서 접근하는 변수는 이렇게 선언
atomic<int32> sum = 0;

void Add()
{
    for(int32 i = 0; i < 1'000'000; i++)
    {
        sum.fetch_add(1);
    }
}

void Sub()
{
    for(int32 i = 0; i < 1'000'000; i++)
    {
        sum.fetch_add(-1);
    }
}

int main()
{
    std::thread t1(Add);
    std::thread t2(Sub);

    t1.join();
    t2.join();

    cout << sum << endl;
    // 0이 나온다
}
```

* 느낌이 오지만 한 번에 하나의 Thread에서만 접근이 가능하다

---

## `std::mutex`

* atomic보다 좀 더 효율적인?(편리한) 방법을 알려준다.

```cpp
vector<int32> v;

void Push()
{
    for(int32 i = 0; i < 10'000; i++)
    {
        v.push_back(i);
    }
}

int main()
{
    std::thread t1(Push);
    std::thread t2(Push);

    // crash!! -> vector는 multi thread환경에 safe하지 않다
    // vector는 공간이 부족할경우 메모리를 새로할당 받는데 그때 crash발생
    t1.join();
    t2.join();

    cout << v.size() << endl;
}
```

* 해결법1

```cpp
#include <mutex>

vector<int32> v;
mutex m;

void Push()
{
    for(int32 i = 0; i < 10'000; i++)
    {
        m.lock();
        v.push_back(i);
        m.unlock();

        // 하지만 이 방식은 동작속도가 매우 느리다. -> Mutual Exclusive(상호배타적)
        // 그리고 Deadlock을 조심해야한다.
    }
}

int main()
{
    std::thread t1(Push);
    std::thread t2(Push);

    t1.join();
    t2.join();

    cout << v.size() << endl;
}
```

* 조금 더 안전한 코드를 만들어 보자.
    * 현재코드의 위험은 unlock을 호출하지 않았을때 있다.

```cpp
#include <mutex>

vector<int32> v;
mutex m;

void Push()
{
    for(int32 i = 0; i < 10'000; i++)
    {
        std::lock_guard<std::mutex> lockGuard(m);
        // or
        std::unique_lock<std::mutex> uniqueLock(m);
        // std::unique_lock<std::mutex> uniqueLock(m, std::defer_lock);
        // std::defer_lock : 당장은 잠그지말고 .lock()을 호출하면 잠궈달라
        v.push_back(i);
    }
}

int main()
{
    std::thread t1(Push);
    std::thread t2(Push);

    t1.join();
    t2.join();

    cout << v.size() << endl;
}
```

---

## Deadlock

* lockguard로 모든 데드락을 방지할순 없다. 아래의 예시를 살펴보자.

```cpp
class User
{
    // ...
};

class UserManager
{
public:
    static UserManager* Instance()
    {
        static UserManager instance();
        return &instance;
    }

    User* GetUser(int32 id)
    {
        lock_guard<mutex> guard(_mutex);
        return nullptr;
    }

    void ProcessSave()
    {
        lock_guard<mutex> guard(_mutex);

        Account* account = AccountMnager::Instance()->GetAccount(100);

        // ...
    }

private:
    mutex _mutex;
};
```

```cpp
class Account
{
    // ...
};

class AccountManager
{
public:
    static AccountManager* Instance()
    {
        static AccountManager instance();
        return &instance;
    }

    Account* GetAccount(int32 id)
    {
        lock_guard<mutex> guard(_mutex);
        return nullptr;
    }

    void ProcessLogin()
    {
        lock_guard<mutex> guard(_mutex);

        User* user = UserManager::Instance()->GetUser(100);

        // ...
    }

private:
    mutex _mutex;
};
```

```cpp
void Func1()
{
    for(int32 i = 0; i < 100; i++)
    {
        UserManager::Instance()->ProcessSave();
    }
}

void Func2()
{
    for(int32 i = 0; i < 100; i++)
    {
        AccountManager::Instance()->ProcessLogin();
    }
}

int main()
{
    std::thread t1(Func1);
    std::thread t2(Func2);

    t1.join();
    t2.join();
}
```

* 어떤상황에 Deadlock이 발생할까 `ProcessSave()`, `ProcessLogin()`이 호출될 경우 lock이 발생하고 `ProcessSave()`, `ProcessLogin()`내부에서 `GetAccount`, `GetUser` 호출시 역시 lock이 발생하며 Deadlock이 된다.
* 해결책?? -> mutex lock을 잠그는 순서를 지정한다.
    * 결론적 ... 사용자가 조심히 써야한다는 말 ...

```cpp
void ProcessLogin()
{
    User* user = UserManager::Instance()->GetUser(100);

    // 아래서 lock
    lock_guard<mutex> guard(_mutex);

    // ...
}
```

---

## jthread(C++20)

* [참고사이트 🌎](https://taehyungs-programming-blog.github.io/blog/docs/cpp/concurrency/04.jthread/)

* 기존 thread의 문제점??

```cpp
#include <iostream>
#include <thread>
#include <chrono>
using namespace std::literals;

void foo(int a, double d)
{
    std::cout << "start foo" << std::endl;
    std::this_thread::sleep_for(2s);
    std::cout << "finish foo" << std::endl;
}

int main()
{
    std::thread t(foo, 10, 3.4);
    t.join();
    // 반드시 join, detach를 해야한다. -> 소멸자에서 자동으로 join하게 해준다면??
}
```

```cpp
// 이렇게 해결하곤 했다.
class mythread
{
    std::thread th;
public:
    template<typename F, typename ... ARGS>
    explicit mythread(F&& f, ARGS&& . args) : th(std::forward<F>(f), std::forward<ARGS>(args)...) {}
    
    ~mythread()
    {
        if(th.joinable())
            th.join();
    }
};
```

---

* 이걸 표준(C++20)에서 지원해줌

```cpp
int main()
{
    std::jthread f(foo, 10, 3.4);
}
```

```cpp
#include <iostream>
#include <thread>
#include <chrono>
using namespace std::literals;

void foo(int a, double d)
{
    for(int i = 0; i < 10; i++)
    {
        std::this_thread::sleep_for(500ms);
        std::cout << "foo : " << i << std::endl;
    }
}

void goo(std::stop_token token)
{
    for(int i = 0; i < 10; i++)
    {
        if(token.stop_requested())
        {
            std::cout << "중지요청" << std::endl;
            return;
        }
        std::this_thread::sleep_for(500ms);
        std::cout << "goo : " << i << std::endl;
    }
}

int main()
{
    std::jthread j1(foo);
    std::jthread j2(goo);
    std::this_thread::sleep_for(2s);

    // 중지요청
    j1.request_stop();
    j2.request_stop();
}
```