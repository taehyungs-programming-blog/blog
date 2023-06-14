---
title: "(STL) unique_ptr"
permalink: cpp/stl/spointer/unique_ptr/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-15 00:00:00 -0000
last_modified_at: 2021-03-15 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
category:
  - unique_ptr
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

## unique_ptr이란?

* 메모리주소의 참조를 독점할 수 있는 포인터
* 왜쓰나? 굳이 소멸자를 호출하지 않아도 되는 장점이 있다!

```cpp
#include <iostream>
#include <memory>
using namespace std;

class Car
{
    int color;
    int speed;
public:
    ~Car() { cout << "~Car()" << endl; }
 
    void Go() { cout << "Car go" << endl; }
};

int main()
{
    shared_ptr<Car> sp1(new Car);
    shared_ptr<Car> sp2 = sp1;      // 참조 계수 2

    unique_ptr<Car> up1(new Car);   // 자원 독점
    unique_ptr<Car> up2 = up1;      // error
```

## 그런데 그래도 옮겨야할 때가 발생한다면?? (move이용)

```cpp
#include <iostream>
#include <memory>
using namespace std;

int main()
{
    unique_ptr<int> up1(new int);

    unique_ptr<int> up2 = move(up1);        // ok.
}
```

## 그래도 소멸자를 호출하고 싶다면? 

* 자동 소멸자를 생성해 보자(삭제자)

```cpp
#include <iostream>
#include <memory>
using namespace std;

struct Deleter
{
    void operator()(int* p) const
    {
        delete p;
    }
}

int main()
{
    unique_ptr<int> up1(new int);

    // 삭제자는 이렇게 선언
    unique_ptr<int, Deleter> up(new int);

    // 이런 삭제자도 가능 - 1
    unique_ptr<int, void(*)(int*)> up(new int, foo);

    // 이런 삭제자도 가능 - 2
    auto f = [](int*p){delete p;}
    unique_ptr<int, decltype(f)> up(new int, f);
}
```

```cpp
// 배열도 사용가능 (C++11)
unique_ptr<int[]> up(new int[10]);
```

---

## shared_ptr <-> unique_ptr 호환성

```cpp
#include <iostream>
#include <memory>
using namespace std;

int main()
{
    shared_ptr<int> sp(new int);
    unique_ptr<int> up(new int);

    shared_ptr<int> sp1 = up;       // error
    shared_ptr<int> sp2 = move(up); // ok

    unique_ptr<int> up1 = sp;       // error
    unique_ptr<int> up2 = move(sp); // error
}
```

---

## Example

```cpp
#include <iostream>
#include <memory>
using namespace std;

class Shape {};
class Rect : public Shape {};
class Circle : public Shape {};

Shape* CreateShape(int type)
{
    Shape* p = nullptr;

    switch(type)
    {
        case 1 : p = new Rect; break;
        case 2 : p = new Circle; break;
    }
    return p;
}

int main()
{
    Shape* p = CreateShape(1);
```

```cpp
#include <iostream>
#include <memory>
using namespace std;

class Shape {};
class Rect : public Shape {};
class Circle : public Shape {};

unique_ptr<Shape> CreateShape(int type)
{
    unique_ptr<Shape> p;

    switch(type)
    {
        case 1 : p.reset(new Rect); break;
        case 2 : p.reset(new Circle); break;
    }
    return p;
}

int main()
{
    // unique_ptr<Shape> p = CreateShape(1);
    shared_ptr<Shape> p = CreateShape(1);
    // unique, shared 모두 사용가능
```

## 배열을 사용하고 싶을 경우

```cpp
unique_ptr<uint8_t[]> mp_array;

// 선언
mp_array = std::make_unique<uint8_t[]>(48000 * 8);

// uint8_t* 의 호출이 필요할 경우
&mp_array[0];
```
