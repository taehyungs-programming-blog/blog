---
title: "(C++) delegate constructor"
permalink: cpp/delegate-constructor/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-02-05 00:00:00 -0000
last_modified_at: 2020-09-22 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
category:
  - delegate-constructor
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

```cpp
#include <iostream>
using namespace std;

struct Point
{
  int x, y;
  Point() : x(0), y(0) {}
  Point(int a, int b) : x(a), y(b) {}
  // 생성자 안에서 하는 일이 많다면?? 생성자 모두에 다 구현해줘야할까?
};

int main()
{
  Point p;
  
  cout << p.x << endl;
  cout << p.y << endl;
}
```

그래서 그런데 ...

```cpp
struct Point
{
  int x, y;
  Point() : {
    Point(0, 0);    // 이런선언이 가능할까? -> Nope 임시객체를 만드는 선언이지 생성자를 재 호출하는 선언이 아니다.
  }
  Point(int a, int b) : x(a), y(b) {}
};
```

위임생성자의 탄생

```cpp
struct Point
{
  int x, y;
  Point() : Point(0,0) {}     // delegate constructor(위임 생성자)
  Point(int a, int b) : x(a), y(b) {}
};
```

---

## 추가

```cpp
class Parent {
  int dogs;
  string text;

public:
  /*
  Parent() {
    dogs = 5;
    cout << "No parameter parent constructor" << endl;
  }
  */

  Parent(string text) {
    dogs = 5;
    this->text = text;
    cout << "string parameter parent constructor" << endl;
  }
};

class Child : public Parent {
public:
  Child() { // error - 부모도 기본생성자가 있어야 한다.

  }
};

int main()
{
  Parent parent("Hello");
  Child child;

  return 0;
}
```

```cpp
class Parent {
  int dogs;
  string text;

public:
  /*
  Parent() {
    dogs = 5;
    cout << "No parameter parent constructor" << endl;
  }
  */

  Parent(string text) {
    dogs = 5;
    this->text = text;
    cout << "string parameter parent constructor" << endl;
  }
};

class Child : public Parent {
public:
  Child() : Parent("Hello") { // okay

  }
};

int main()
{
  Parent parent("Hello");
  Child child;

  return 0;
}
```

C++11방식으로 이 문제를 해결해보자

```cpp
class Parent {
  int dogs;
  string text;

public:
  Parent() : Parent("hello") {
    dogs = 5;
    cout << "No parameter parent constructor" << endl;
  }

  Parent(string text) {
    dogs = 5;
    this->text = text;
    cout << "string parameter parent constructor" << endl;
  }
};

class Child : public Parent {
public:
  Child() = defualt;
};
```

---

## -fno-elide-constructors

```cpp
class Test {
public:
  Test() {
    cout << "constructor" << endl;
  }

  Test(int i) {
    cout << "parameterized constructor" << endl;
  }

  Test(const Test& other) {
    cout << "copy constructor" << endl;
  }

  Test& operator=(const Test& other) {
    cout << "assignment" << endl;
    return *this;
  }

  ~Test() {
    cout << "destructor" << endl;
  }

  friend ostream &operator<<(ostream& out, const Test& test);
};

ostream &operator<<(ostream& out, const Test& test) {
  out << "Hello from test";
  return out;
}

Test getTest() {
  return Test();
}

int main() {
  Test test1 = getTest();
  cout << test1 << endl;

  // constructor
  // copy constructor
  // destructor
  // copy constructor
  // destructor
  // Hello from test
  // destructor
  // 간단한 동작에 이렇게 만은 함수가??
}
```

```cpp
Test getTest() {
  return Test();    // constructor
}

int main() {
  Test test1 = getTest(); // copy contructor and destructor
  cout << test1 << endl;  // copy, destruc, hellwo, destruc 순서 호출
}
```

`-fno-elide-constructors` C++에 옵션을 줘서 최적화 가능

---

## Constructors and Memory

```cpp
#include <memory.h>

class Test {
private:
  static const int SIZE = 100;
  int * _pBuffer;
public:
  Test() {
    cout << "constructor" << endl;
    // _pBuffer = new int[SIZE];
    // memset(_pBuffer, 0, sizeof(int)*SIZE);

    // C++11 방식으로 초기화
    _pBuffer = new int[SIZE]{};   // 쉽다!

    for(int i = 0; i < SIZE; i++) {
      _pBuffer[i] = 7 * i;
    }

    // 복사생성자에서 메모리초기화는 이렇게
    // memcpy(_pBuffer, other._pBuffer, SIZE*sizeof(int));
  }

  // ...

  ~Test() {
    cout << "destructor" << endl;

    delete [] _pBuffer;
  }

  friend ostream &operator<<(ostream& out, const Test& test);
};

// ...
```

---

## Example

```cpp
class dog {
public:
  dog() { ... }
  dog(int a) { dog(); doOtherThings(a); }
};

// C++ 03:
class dog {
  init() { ... };
public:
  dog() { init(); }
  dog(int a) { init(); doOtherThings(); }
};

// C++ 11:
class dog {
public:
  dog() { ... }
  dog(int a) : dog() { doOtherThings(); }
};
```