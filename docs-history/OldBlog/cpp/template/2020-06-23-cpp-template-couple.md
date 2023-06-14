---
title: "C++ Template : couple template"
permalink: cpp/template/couple/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-06-23 00:00:00 -0000
last_modified_at: 2020-06-23 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
header:
  teaser: /file/image/cpp-page-teaser.gif
tag:
  - C++
  - Template
category:
  - couple
excerpt: ""
---

```cpp
#include <iostream>
using namespace std;

template<typename T> void printN(const T& cp)
{
  cout << T::N << endl;
}

template<typename T, typename U> struct couple
{
  T v1;
  U v2;
  enum { N = 2 };
};

int main()
{
  couple<int, double> c1;
  printN(c1);
}
```

```cpp
template<typename T, typename U> struct couple
{
  T v1;
  U v2;
  enum { N = 2 };
};

int main()
{
  couple<int, double> c1;
  couple<int, couple<int, char>> c2;
  couple<int, couple<int, couple<int, char>>> c3;
  printN(c1);   // 2
  printN(c2);   // 2
  printN(c3);   // 2
  // 흠 ... couple의 개수에 따라서 카운트를 증가 시키고 싶다면?
}
```

```cpp
template<typename T, typename U> struct couple
{
  T v1;
  U v2;
  enum { N = 2 };
};

template<typename A, typename B, typename C> 
struct couple<A, couple<B, C>>
{
  A v1;
  couple<B,C> v2;
  enum { N = couple<B,C>::N + 1 };
};
// 템플릿으로 자기 자신을 보내는 것을 주목하자

int main()
{
  couple<int, double> c1;
  couple<int, couple<int, char>> c2;
  couple<int, couple<int, couple<int, char>>> c3;
  printN(c1);   // 2
  printN(c2);   // 3
  printN(c3);   // 4
}
```

```cpp
template<typename A, typename B, typename C> 
struct couple<couple<A, B>, C>
{
  couple<A,B> v1;
  C v2;
  enum { N = couple<A,B>::N + 1 };
};

int main()
{
  couple<int, double> c1;
  couple<couple<int, char>, int> c2;
  couple<ouple<int, couple<int, char>>, int> c3;
  printN(c1);   // 2
  printN(c2);   // 3
  printN(c3);   // 4
}
```

```cpp
// 마지막으로 다음을 처리해 보자.
int main()
{
  couple<coupe<int, int>, couple<int, int>> c4;
  printN(c4);
}
```

```cpp
template<typename A, typename B, typename C, typename D> 
struct couple<couple<A, B>, couple<C, D>>
{
  couple<A,B> v1;
  couple<C,D> v2;
  enum { N = couple<A,B>::N + couple<C,D> };
};
```

---

## tuple using couple

```cpp
#include <iostream>
using namespace std;

template<typename T, typename U> struct couple
{
  T v1;
  U v2;
  enum { N = 2 };
};

int main()
{
  couple<int, couple<int, double>> c3;
  xtuple<int, int, double> t3;    // 요걸 만들고 싶다
}
```

```cpp
struct Null {};   // empty class

template<typename P1,
typename P2,
typename P3 = Null,
typename P4 = Null,
typename P5 = Null > 
class xtuple
{
  
};

int main()
{
  // xtuple은 2개 이상 5개 이하 자료형 처리가능
  xtuple<int, char, long, short, double> t5;    // okay
  xtuple<int, char, long> t3; // okay
}
```

```cpp
template<typename P1,
typename P2,
typename P3 = Null,
typename P4 = Null,
typename P5 = Null > 
class xtuple : public couple<P1, xtuple<P2, P3, P4, P5, Null>>
{
};

temaplte<typename P1, typename P2>
class xtuple<P1, P2, Null, Null, Null> : couple<P1, P2>
{
};

int main()
{
  // couple<short, double>
  // couple<long, xtuple<short, double, Null, Null, Null>>
  // couple<char, xtuple<long, short, double, Null, Null>>
  // couple<int, xtuple<char, long, short, double, Null>>
  xtuple<int, char, long, short, double> t5;
}
```