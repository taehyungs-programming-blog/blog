---
title: "C++ Template : C++ 표준 type_traits"
permalink: cpp/template/cpp_type_traits/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-06-25 00:00:00 -0000
last_modified_at: 2020-06-25 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
header:
  teaser: /file/image/cpp-page-teaser.gif
tag:
  - C++
  - Template
category:
  - cpp_type_traits
excerpt: ""
---

```cpp
#include <iostream>
#include <type_traits>    // for remove_pointer, is_pointer
using namespace std;

template<typename T> void foo(T a)
{
  typename remove_pointer<T>::type n;
  bool b = is_pointer<T>::value;
}

int main()
{
  int n = 0;
  foo(&n);
}
```

```cpp
#include <iostream>
#include <type_traits>    // for remove_pointer, is_pointer
using namespace std;
// C++14
// using remove_pointer_t = typename remove_pointer<T>::type;

template<typename T> void foo(T a)
{
  // 포인터를 제거한 타입을 구해보자.
  typename remove_pointer<T>::type n;
  // typename은 값으로 보지말고 type으로 봐달란 명령
  remove_pointer_t<T> n;    // C++14부터는 이렇게 간단하게 적을 수 있다.
}

int main()
{
  int n = 0;
  foo(&n);
}
```

```cpp
#include <iostream>
#include <type_traits>    // for remove_pointer, is_pointer
using namespace std;

// C++ 17 - 아래를 지원
template<typename T>
inline constexpr bool is_pointer_v = is_pointer<T>::value;

void foo_imp(true_type) {}
void foo_imp(false_type) {}

template<typename T> void foo(T a)
{
  // T가 포인터 인지 알고 싶다
  bool b = is_pointer<T>::value;
  // bool b = is_pointer_v<T>;
  
  if /*constexpr*/ (is_pointer_v<T>)    // 이렇게 쓰면 에러 안남.
  {
    cout << *a << endl;   // error
  }
  foo_imp(is_pointer<T>());
}

int main()
{
  int n = 0;
  foo(&n);
}
```

```cpp
#include <iostream>
#include <type_traits>    // for remove_pointer, is_pointer
using namespace std;

template<typename T> void foo(T& a) // 참조로 받음을 기억.
{
  // 배열인지 조사해 보자
  if(is_array<T>::value)
  {
    cout << extent<T, 0>::value << endl; // 3
    cout << extent<T, 1>::value << endl; // 2
  }
}

int main()
{
  int x[3][2] = {1,2,3,4,5,6};
  foo(x);
}
```

```cpp
template<typename T, typename U> void foo(T a, U b)
{
  bool ret = is_same<T, U>::value;    // 같은 타입인지 조사한다.
  cout << ret << endl;
}

int main()
{
  foo(0, 0);
  foo<int, const int>(0,0);   // int, const int는 다르기에 다른 타입으로 판별
  // 만약 const를 붙여도 같은 타입으로 판별하고 싶다면??
}
```

```cpp
template<typename T, typename U> void foo(T a, U b)
{
  // bool ret = is_same<T, remove_const<U>::type>::value;    // const를 제거해 준다.
  bool ret = is_same<T, remove_cv<U>::type>::value;           // const, volite을 모두 제거해 준다.
  cout << ret << endl;
}
```

```cpp
template<typename T, typename U> void foo(T a, U b)
{
  bool ret = is_same<remove_cv<T>::type, remove_cv<U>::type>::value;
  cout << ret << endl;
}
int main()
{
  foo<int[3], int*>(0,0); // 다른 타입으로 추론
  // 만약 배열의 주소와 포인터를 같은 타입으로 추론하고 싶다면??
}
```

```cpp
template<typename T, typename U> void foo(T a, U b)
{
  bool ret = is_same<typename decay<T>::type, typename decay<U>::type>::value;
  // 이 외에도 상당히 많은 trait가 있음 홈페이지를 참조하자
  cout << ret << endl;
}
int main()
{
  foo<int[3], int*>(0,0); // 다른 타입으로 추론
  // 만약 배열의 주소와 포인터를 같은 타입으로 추론하고 싶다면??
}
```
