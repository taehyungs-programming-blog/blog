---
title: "STL : chrono"
permalink: cpp/stl/chrono/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-15 00:00:00 -0000
last_modified_at: 2020-04-15 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

* [Run This Code](https://ideone.com/JtU3zG)

```cpp
#include <iostream>
#include <chrono>
using namespace std;
using namespace chrono;

int main()
{
    hours h(1);
    minutes m(h);   // 60
    seconds s(h);   // 3600

    cout << m.count() << endl;
    cout << s.count() << endl;

    hours h2(s);    // error
    hours h2 = duration_cast<hours>(s); // ok

    using days = duration<int, ratio<3600*24, 1>>;  
    // 이런식으로 활용이 가능하다.
    days d(1);
    minutes m2(d);

    cout << m2.count() << endl; // 60 * 24
}
```

---

* [Run This Code](https://ideone.com/Wb8F5A)

```cpp
#include <iostream>
#include <chrono>
using namespace std;
using namespace chrono;

int main()
{
    seconds s1(3);        // ok
    seconds s2 = 3;       // erorr
    seconds s3 = 3s;      // ok
    seconds s4 = 4min;      // ok

    cout << s4.count() << endl;
```

```cpp
#include <iostream>
#include <chrono>
using namespace std;
using namespace chrono;

void foo(seconds s) {}

int main()
{
    foo(3);     // error
    foo(3s);    // ok

    seconds s5 = 3min + 40s;        // ok
```

* 현재 시간구하기

* [Run This code](https://ideone.com/wFdweh)

```cpp
#include <iostream>
#include <chrono>
#include <string>
using namespace std;
using namespace chrono;

int main()
{
    system_clock::time_point tp = system_clock::now();
    // 1970년 1월 1일 0시 기준 얼마나 흘렀나 나온다.
    nanoseconds ns = tp.time_since_epoch();

    cout << ns.count() << endl;

    hours h = duration_cast<hours>(ns);
    cout << h.count() << endl;

    time_t t = system_clock::to_time_t(tp);
    string s = ctime(&t);
    cout << s << endl;
```

![](/file/image/chrono.png)