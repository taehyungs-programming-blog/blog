---
title: "STL : synchronization"
permalink: cpp/stl/synchronization/                # link 직접 지정
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
#include <string>
#include <mutex>
using namespace std;

mutex m;
int global = 0;

void f1()
{
    lock_guard<mutex> lg(m);
    // lock_guard는 mutex내에서 exception이 생길경우를 처리해준다.
    // unlock은 소멸자에서 자동으로 해준다.
    //m.lock();
    global = 100;
    global = global + 1;
    //m.unlock();
}

int main()
{
    thread t1(&f1);
    thread t2(&f1);

    t1.join();
    t2.join();
}
```

```cpp
#include <iostream>
#include <thread>
#include <future>
using namespace std;

void f1(promise<int>& p)
{
    this_thread::sleep_for(3s);
    p.set_value(10);
}

int main()
{
    promise<int> p;
    future<int> ft = p.get_future();

    thread t(&f1, ref(p));
    cout << "wait value " << endl;
    cout << "value : " << ft.get() << endl; // set value까지 대기한다.

    t.join();
}
```