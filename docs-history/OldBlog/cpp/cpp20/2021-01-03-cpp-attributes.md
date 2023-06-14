---
title: "(c++20) attributes"
permalink: cpp/cpp20/attributes/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2021-01-03 00:00:00 -0000
last_modified_at: 2021-01-03 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
  - c++20
category:
  - attributes
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

## Attributes = 컴파일러 지시문

```cpp
// GCC
__attributes_((...))

// MS
__declspec(...)
```

C++20에서 새로운 Attributes가 추가된다.

---

## nodiscard

```cpp
int foo() { return 0; }

int* allocate(int size)
{
    return new int[size];
}

int main()
{
    int* p = allocate(10);

    // 리턴을 받지 않아도 빌드가 된다.
    // 이걸 막아보자
    allocate(10);
}
```

```cpp
int foo() { return 0; }

// 리턴형을 무조건 받게해달라
[[nodiscard("이유를 여기 적는다.")]] int* allocate(int size)
{
    return new int[size];
}

int main()
{
    int* p = allocate(10);

    // nodiscard로 warning발생
    allocate(10);
}
```

```cpp
struct MyType
{
    MyType(int n, bool b) {}
};

enum class ErrorCode{ OK, WARNING, CRITICAL, FATAL };

ErrorCode foo() { return ErrorCode::CRITICAL; }

int main()
{
    // 리턴값을 무조건 처리하게 해달라!
    foo();

    MyType t1 = MyType(1, true);

    MyType(1, true);
}
```

```cpp
struct MyType
{
    [[nodiscard]] MyType(int n, bool b) {}
};

// 이 enum을 사용하는 리턴은 무조건 리턴을 받아야 한다.
enum class [[nodiscard]] ErrorCode{ OK, WARNING, CRITICAL, FATAL };

ErrorCode foo() { return ErrorCode::CRITICAL; }

int main()
{
    foo();      // warning 발생

    MyType t1 = MyType(1, true);

    MyType(1, true);      // warning 발생
}
```

---

## likely, unlikely

제어문등을 만들 때 특정 조건의 발생확률이 더 높음을 알려준다.

```cpp
int foo(int i)
{
    if (i > 0)      // 여기가 더 많이 실행됨을 알려주고 싶다면?
        i += 2;
    else
        i -= 2;

    return i;
}

int main()
{
    foo(10);

    return 0;
}
```

```cpp
int foo(int i)
{
    if (i > 0)  [[likely]]
        i += 2;
    else
        i -= 2;

    return i;
}

int main()
{
    foo(10);

    return 0;
}
```

---

## no_unique_addressl

```cpp
#include <iostream>

// Empty class : non-static 멤버 데이터와 가상함수(virtual)가 없는 클래스 (일반 함수는 있어도 됨)
// 크기는 1byte이다.
struct Empty
{
};

struct Data
{
    Empty e;
    int n;
};

int main()
{
    std::cout << sizeof(Empty) << std::endl;        // 1
    std::cout << sizeof(Data) << std::endl;         // 8 -> int는 4 bytes, Empty는 1이지만 padding이 들어가서 4bytes
}
```

```cpp
#include <iostream>

struct Empty
{
};

struct Data
{
    [[no_unique_address]] Empty e;
    // empty class가 구조체 멤버로 놓일 때 크기가 0으로 계산된다.
    int n;
};

int main()
{
    std::cout << sizeof(Empty) << std::endl;        // 1
    std::cout << sizeof(Data) << std::endl;         // 4     
}
```

```cpp
#include <iostream>

template<typename T, typename U> struct PAIR
{
    [[no_unique_address]] T first;
    [[no_unique_address]] U second;

    template<typname A, typename B>
    PAIR(A&& a, B&& b) : first(std::forward<A>(a)), second(std::forward<B>(b)) {}
};

// C++17, class template type deduction guide
template<typename A, typename B> PAIR(A&& a, B&& b) -> PAIR<A, B>;

int main()
{
    // 메모리 주소와 삭제자를 보관
    PAIR p(malloc(100), [](void* p){free(p);});

    std::cout << sizeof(p) << std::endl;
}
```

```cpp
#include <iostream>

template<typename T, typename U> struct PAIR
{
    T first;
    U second;

    template<typname A, typename B>
    PAIR(A&& a, B&& b) : first(std::forward<A>(a)), second(std::forward<B>(b)) {}
};

// C++17, class template type deduction guide
template<typename A, typename B> PAIR(A&& a, B&& b) -> PAIR<A, B>;

int main()
{
    // 메모리 주소와 삭제자를 보관
    PAIR p(malloc(100), [](void* p){free(p);});

    std::cout << sizeof(p) << std::endl;
}
```