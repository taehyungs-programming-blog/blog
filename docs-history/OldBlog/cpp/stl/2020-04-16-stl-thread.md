---
title: "STL : thread"
permalink: cpp/stl/thread/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-16 00:00:00 -0000
last_modified_at: 2020-04-16 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

```cpp
#include <iostream>
#include <thread>
#include <chrono>
using namespace std;

int main()
{
    thread::id id = this_thread::get_id();

    cout << id << endl;

    this_thread::sleep_for(3s);
    this_thread::sleep_until(chrono::system_clock::now()+3s);
    this_thread::yield();
}
```

* [Run This Code](https://ideone.com/h2geX0)

```cpp
#include <iostream>
#include <thread>
using namespace std;

void foo()
{
    cout << "thread start" << endl;
    this_thread::sleep_for(2s);
    cout << "thread end" << endl;
}

int main()
{
    thread t(&foo);
    t.join();       // 스레드 종료를 대기
    // or
    // t.detach();
}
```

```cpp
#include <iostream>
#include <thread>
using namespace std;

void f1() {}
void f2(int a) {}

struct Worker
{
    void Main() {}
};

struct Functor
{
    void operator()() {}
};

int main()
{
    thread t1(&f1);

    thread t2(&f2, 5);

    Worker w;
    thread t3(&Workder::Main, &w);

    Functor f;
    thread t4(f);

    thread t5([](){cout << "thread t5" << endl;});
}
```

```cpp
#include <iostream>
#include <thread>
using namespace std;

void f1(int a, int b)
{

}

int main()
{
    thread t1(&f1, 1, 2);
    thread t2(bind(&f1, 1, 2));
    // 둘 다 동일한 표현이다.

    t1.join();
    t2.join();
}
```

```cpp
int n = 10;
thread t1(&f1, 1, n);            // error
thread t1(&f1, 1, ref(n));       // ok
```