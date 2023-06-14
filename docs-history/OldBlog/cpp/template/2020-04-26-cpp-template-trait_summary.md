---
title: "C++ Template : traits 정리"
permalink: cpp/template/traits_summary/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-26 00:00:00 -0000
last_modified_at: 2020-04-26 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

```cpp
#include <iostream>
#include <type_traits>
using namespace std;

template<typename T> void foo(T a)
{
    typename remove_pointer<T>::type n;     // 포이터를 제거한 타입
    bool b = is_pointer<T>::value;          // 포인터인지 확인
}

int main()
{
    int n = 0;
    foo(&n);
}
```

```cpp
#include <iostream>
#include <type_traits>
using namespace std;

template<typename T>
using remove_pointer_t = typename remove_pointer<T>::type;

template<typename T> void foo(T a)
{
    typename remove_pointer<T>::type n; // T에서 포인터를 제거한 타입을 확인
    // remove_pointer_t<T> n;           // 이렇게 간단하게 쓸 수 있다.(C++14부터는 그냥 제공)

    cout << typeid(n).name() << endl;   // int
}

int main()
{
    int n = 0;
    foo(&n);
}
```

```cpp
#include <iostream>
#include <type_traits>
using namespace std;

template<typename T> inline constexpr bool is_pointer_v = is_pointer<T>::value;
void foo_imp(true_type) {}
void foo_imp(false_type) {}

template<typename T> void foo(T a)
{
    bool b = is_pointer<T>::value;      // T가 포인터인지 알고 싶다.
    // bool b = is_pointer_v<T>;        // 역시 이렇게 간단하게 적을 수 있고 C++17부터 그냥 제공한다.

    if(is_pointer_v<T>)
    {
        cout << *a << endl;                 // compile error : 아직 포인터 인지 아닌지 모름!
    }
    // if constexpr (is_pointer_v<T>)       // ok
    foo_imp(is_pointer<T>());               // ok
}
```

```cpp
#include <iostream>
#include <type_traits>
using namespace std;

template<typename T> void foo(T a)
{
    if(is_array<T>::value)
    {
        cout << "array" << endl;        // 여기가 호출안됨
    }
}

int main()
{
    int x[3][2] = {1,2,3,4,5,6,7};
    foo(x);
}
```

```cpp
#include <iostream>
#include <type_traits>
using namespace std;

template<typename T> void foo(T& a)     // 참조로 해야
{
    if(is_array<T>::value)
    {
        cout << "array" << endl;        // 여기가 호출됨
    }
}

int main()
{
    int x[3][2] = {1,2,3,4,5,6,7};
    foo(x);
}
```


```cpp
#include <iostream>
#include <type_traits>
using namespace std;

template<typename T> void foo(T& a)
{
    if(is_array<T>::value)
    {
        cout << "array" << endl;        
        cout << extend<T, 0>::value << endl; //3
        cout << extend<T, 1>::value << endl; //2
    }
}

int main()
{
    int x[3][2] = {1,2,3,4,5,6,7};
    foo(x);
}
```

```cpp
template<typename T, typename U> void foo(T a, U b)
{
    bool ret = is_same<T, U>::value;        // 같은 타입이면 true가 나온다.
    cout << ret << endl;
}

int main()
{
    foo(int, const int)(0, 0);      // 만약 int, const int를 같은 타입으로 생각하게 만들고 싶다면?
}
```

```cpp
template<typename T, typename U> void foo(T a, U b)
{
    bool ret = is_same<typename remove_cv<T>::type, typename remove_cv<T>::type>::value;
    cout << ret << endl;
}

int main()
{
    foo(int[3], int*)(0, 0);      // 배열과 포인터가 같은 타입으로 생각하게 만들고 싶다면?
}
```

```cpp
template<typename T, typename U> void foo(T a, U b)
{
    bool ret = is_same<typename decay<T>::type, typename decay<T>::type>::value;
    cout << ret << endl;
}
```

뭐 이런 유사한 코드가 많다.