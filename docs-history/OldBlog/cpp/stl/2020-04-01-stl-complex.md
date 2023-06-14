---
title: "STL complex"
permalink: cpp/stl/complex/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-01 00:00:00 -0000
last_modified_at: 2020-04-01 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

```cpp
#include <iostream>
#include <complex>
using namespace std;

int main()
{
    complex<double> c1(1, 0);

    cout << c1 << endl;
    cout << c1.real() << endl;
    cout << c1.imag() << endl;
}
```

다양한 삼각함수도 지원한다.

```cpp
complex<double> c2 = sin(c1);
```

complex는 double, float, long double만 지원함을 기억하자.

```cpp
complex<int> c3(1, 0);      // 컴파일은 가능하지만 연산시 오류가 난다.
```

복소수 리터럴

* 3.2i = complex<double>
* 3.2if = complex<float>
* 3.2il = complex<long double>

```cpp
complex<float> c5(3);       // 3, 0
complex<float> c6(3, 1);    // 3, 1
complex<float> c7(3if);     // 0, 3
```
