---
title: "STL : algorithm and function"
permalink: cpp/stl/algorithm-function/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-10 00:00:00 -0000
last_modified_at: 2020-04-10 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

## functor

* [Run This Code](https://ideone.com/quZsYG)

```cpp
#include <iostream>
using namespace std;

struct Plus
{
    int operator()(int a, int b) const
    {
        return a + b;
    }
};

int main()
{
    Plus p;
    int n = p(1, 2);        // 구조체를 함수인거 처럼쓰는 행위 functor
    cout << n << endl;
}
```

* ()를 사용해서 호출하는 것
    1. 함수
    2. 함수 포인터
    3. ()를 재정의한 클래스
    4. 람다표현식

* [Run This Code](https://ideone.com/vHc9XX)

```cpp
#include <iostream>
#include <vector>
#include <functional>
using namespace std;

int main()
{
    plus<int> p;

    cout << p(1, 2) << endl;

    multiplies<int> m;
    cout << m(1, 2) << endl;

    return 0;
}
```

---

* [Run This code](https://ideone.com/EYOKmR)

```cpp
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

void foo(int a)
{
    cout << a << endl;
    a = a * 2;      // 이런 연산도 가능
}

int main()
{
    vector<int> v = { 1,2,3,4,5,6,7,8,9,10 };

    for_each(begin(v), end(v), foo);

    return 0;
}
```

* [Run This Code](https://ideone.com/P1mkIN)

```cpp
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <fstream>
using namespace std;

struct Show
{
    string sep;
    ostream& os;
    Show(ostream& o = cout, string s= "\n") : os(o), sep(s) {}
    void operator()(int a) const
    {
        os << a << sep;
    }
};

int main()
{
    vector<int> v = { 1,2,3,4,5,6,7,8,9,10 };

    Show s;
    // Show s(cout, "\t");
    for_each(begin(v), end(v), s);

    return 0;
}
```

---

* [Run This Code](https://ideone.com/pe2d3b)

```cpp
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int main()
{
    vector<int> v = { 1,2,3,4,5,6,7,8,9,10 };

    // 람다표현식으로 함수전달
    for_each(begin(v), end(v), [](int a){ cout << a << endl; });

    return 0;
}
```