---
title: "C++ Template : Variadic Template"
permalink: cpp/template/variadic/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-27 00:00:00 -0000
last_modified_at: 2020-06-28 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
header:
  teaser: /file/image/cpp-page-teaser.gif
tag:
  - C++
  - Template
category:
  - Variadic Template
excerpt: ""
---

```cpp
template<typename T> class tuple
{
};

int main()
{
  tuple<int> t1;  // 인자를 여러개 보내고 싶다면?
}
```

```cpp
// 가변인자 클래스 템플릿(C++11)
template<typename ... Types> class tuple
{
};
```

```cpp
// 가변인자 함수 템플릿(C++11)
template<typename ... Types>
void foo(Types ... arg)
{
}
```

---

```cpp
// 값을 꺼내보자
#include <iostream>
using namespace std;

void goo(int n, double d, const char* s)
{
}

template<typename ... Types>
void foo(Types ... args)
{
  // args : Parameter Pack
  cout << sizeof...(args) << endl;    // 3
  cout << sizeof...(Types) << endl;    // 3
  
  goo(args);  // 파라미터 팩 자체를 인자로쓰진 못함
  goo(args...); // pack expansion이라하며 이런식의 사용은 가능
}

int main()
{
  foo(1, 3.4, "AAA");
}
```

---

## pack expansion 상세

```cpp
#include <iostream>
using namespace std;

void goo(int a, int b, int c)
{
}

int hoo(int a) { return -a; }

template<typename ... Types>
void foo(Types ... args)
{
  int x[] = { args... };
  int y[] = { (++args)... };    // 2, 3, 4
  
  int z[] = { hoo(args)... };   // hoo(1), hoo(2), hoo(3) | -1, -2, -3
  
  goo(args...);       // okay : goo(1,2,3)
  goo(hoo(args...));  // okay : goo(-1,-2,-3)
  goo(hoo(args)...);  // okay : goo(hoo(1), hoo(2), hoo(3))
  
  for(auto n : x)
    cout << n << endl;  // 1, 2, 3
}

int main()
{
  foo(1, 2, 3);   // Types : int, int, int | args : 1, 2, 3
}
```

```cpp
int print(int a)
{
  cout << a << ", ";
  return 0;
}

int print_v(int a)
{
  cout << a << ", ";
}


template<typename ... Types> void foo(Types ... args)
{
  print(args);    // error
  print(args...); // print(1,2,3) | error
  print(args)...; // print(1), print(2), print(3) | error -->> ????
  // pack expansion은 함수 호출의 인자 또는 list초기화에만 사용이 가능.
  
  int x[] = { 0, print(args)... }; // okay
  // 앞에 0,을 넣은 이유는? 
  // args에 아무것도 들어오지 않는 수가 발생 // foo()를 호출
  // 그럼 x[]를 초기화 할때 에러가 발생하는데 그 에러를 방지하기 위해서 이다
  
  int y[] = { 0, (print_v(args), 0)... };
  // void를 리턴하게 되면 또 에러가 발생 초기화 중 void를 int로 변경할수 없기에
  // (print_v(1), 0) --> 이 함수의 의미는 print_v(1)을 연산하고 결과가 없으면 ,0을 리턴으로 쓰겠다는 의미
  
  initializer_list<int> e = { (print(args), 0)...};  // 이런식의 선언도 가능하다.
}

int main()
{
  foo(1,2,3);   // args : 1, 2, 3
}
```

```cpp
#include <iostream>
#include <tuple>
using namespace std;

template<typename ... Types> void foo(Types ... args)
{
  int x[] = { args... };
  
  pair<Types...> p1; // pair<int, double> p1
  // 이런 표현도 가능하다
}

int main()
{
  foo(1, 3.4);
}
```

```cpp
#include <iostream>
#include <tuple>
using namespace std;

template<typename ... Types> void foo(Types ... args)
{
  int x[] = { args... };
  
  pair<Types...> p1;  // tuple<int, double>
  
  tuple<Types...> t1; 
  
  tuple<pair<Types...>> t2; // tuple<pair<int, double>> // ok
  tuple<pair<Types>...> t3;  // tuple<pair<int>, pair<double>> // error
  tuple<pair<int, Types>...> t4;  // tuple<pair<int, int>, pair<int, double>> // ok
  
  pair<tuple<Types...>> p2; // pair<tuple<int, double>> p2;   // error
  pair<tuple<Types>...> p3; // pair<tuple<int>, tuple<double>> // ok
}

int main()
{
  foo(1, 3.4);
}
```