---
title: "C++ Template : specialization"
permalink: cpp/template/specialization/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-06-20 00:00:00 -0000
last_modified_at: 2020-06-2 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
header:
  teaser: /file/image/cpp-page-teaser.gif
tag:
  - C++
  - Template
category:
  - specialization
excerpt: ""
---

```cpp
#include <iostream>
using namespace std;

template<typename T> class Stack
{
public:
  void push(T a) { cout << "T" << endl; }
};

template<typename T> class Stack<T*>
{
// 포인터 일때는 나를 써달라
public:
  void push(T* a) { cout << "T*" << endl; }
};

template<> class Stack<char*>
{
// char* 일때는 나를 써달라
public:
  void push(char** a) { cout << "char**" << endl; }
};

int main()
{
  Stack<int> s1; s1.push(0);
  Stack<int*> s2; s2.push(0);
  Stack<char*> s3; s3.push(0);
}
```

---

## Example

```cpp
#include <iostream>
using namespace std;

template<typename T, typename U> struct Test
{
  static void foo() { cout << "T, U" << endl; }
};

int main()
{
  // 오른쪽 주석과 같은 결과가 나오게 부분 특수화 해보자.
  Test<int, double>::foo();   // T, U
  Test<int*, double>::foo();   // T*, U
  Test<int*, double*>::foo();   // T*, U*
  Test<int, int>::foo();   // T, T
  Test<int, char>::foo();   // int, U
  Test<int, short>::foo();   // int, short
  Test<double, Test<char, short>>::foo();   // T, Test<U, V>
}
```

```cpp
// Test<int*, double>::foo();   // T*, U

template<typename T, typename U> struct Test<T*, U>
{
  static void foo() { cout << "T*, U" << endl; }
};
```

```cpp
// Test<int*, double*>::foo();   // T*, U*

template<typename T, typename U> struct Test<T*, U*>
{
  static void foo() { cout << "T*, U*" << endl; }
};
```

```cpp
// Test<int, int>::foo();   // T, T

template<typename T> struct Test<T, T>
{
  static void foo() { cout << "T, T" << endl; }
};
```

```cpp
// Test<int, char>::foo();   // int, U

template<typename T> struct Test<int, T>
{
  static void foo() { cout << "int, T" << endl; }
};

template<> struct Test<int, int>
{
  static void foo() { cout << "int, int" << endl; }
};

template<> struct Test<int, short>
{
  static void foo() { cout << "int, short" << endl; }
};
```

```cpp
// Test<double, Test<char, short>>::foo();   // T, Test<U, V>

template<typename T, typename U, typaname V> struct Test<T, Test<U, V>>
{
  static void foo() { cout << "T, Test<U, V>" << endl; }
};
```

---

## 주의사항

```cpp
#include <iostream>
using namespace std;

template<typename T> struct Test
{
  static void foo() { cout << typeid(T).name() << endl; }
};

template<typename T> struct Test<T*>
{
  static void foo() { cout << typeid(T).name() << endl; }
};

int main()
{
  Test<int>::foo();     // int
  Test<int*>::foo();    // int* ?? -> 'int'가 나온다
}
```

```cpp
#include <iostream>
using namespace std;

template<typename T, int N = 10> struct Stack
{
  T buff[N];
};

// template<typename T, int N = 10> struct Stack<T*, N>  // error
// 디폴트 값을 빼줘야한다.
template<typename T, int N> struct Stack<T*, N>
{
  T buff[N];
};

int main()
{
  Stack<int, 10> s1;
  Stack<int> s2;    // N = 10
  
  Stack<int*> s3;    // N = 10 - 기본버전에서 디폴트가 들어가기에 특수화 버전에서도 N = 10
}
```

```cpp
template<typename T> class Stack
{
public:
  T pop() {}
  void push(T a);
};

template<typename T> Stack<T>::push(T a)
{
  cout << "T" << endl;
}

// 멤버함수만 특수화가 가능하다.
template<> Stack<char*>::push(char* a)
{
  cout << "char*" << endl;
}

int main()
{
  Stack<int> s1; s1.push(0);
  Stack<char*> s2; s2.push(0);
}
```