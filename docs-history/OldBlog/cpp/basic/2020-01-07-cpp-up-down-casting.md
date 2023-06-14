---
title: "(C++) up/down casting"
#permalink: cpp/basic/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-01-07 00:00:00 -0000
last_modified_at: 2020-03-16 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

## upcasting

* 기반 클래스 타입의 포인터로 파생 클래스를 가리킬 수 있다.
* 기반 클래스 타입의 참조로 파생 클래스를 가리킬 수 있다.

```cpp
class Shape
{
public:
  int color;
};

class Rect : public Shape
{
public:
  int x, y, w, h;
};

int main()
{
  Rect rect;
  
  Rect* p1 = &rect;   // ok
  int*  p2 = &rect;   // error - 데이터 타입이 다르다.
  Shape* p3 = &rect;  // ok
  
  Shape& r = rect;    // ok
}
```

---

기반 클래스 타입의 포인터로 파생 클래스를 가리킬 때,

→ 기반 클래스의 멤버는 접근할 수 있지만
→ 파생 클래스가 추가한 멤버는 접근할 수 없다.
→ 파생 클래스가 추가한 멤버에 접근 하려면 포인터를 파생 클래스타입으로 캐스팅 해야한다.

```cpp
class Shape
{
public:
  int color;
};

class Rect : public Shape
{
public:
  int x, y, w, h;
};

int main()
{
  Rect rect;
  Shape* p = &rect;
  
  p->color = 0;       // ok
  p->x = 0;           // error - shape에는 없다.
  static_cast<Rect*>(p)->x = 0; // ok
}
```

---

## upcasting Example

```cpp
class Shape
{
public:
  int color;
};

class Rect : public Shape
{
public:
  int x, y, w, h;
};

// void changeBlack(Rect* p)
void changeBlack(Shape* p)      // 좀더 효율성이 좋다.
{
  p->color = 0;
}

int main()
{
  Rect r;
  changeBalck(&r);
  
  Shape* buffer[10];    // 모든 도형을 보관 가능
}
```

---

* upcasting - 파생 클래스 포인터를 기반 클래스 타입으로 캐스팅 하는 것 (항상 안전)
* downcasting - 기반 클래스 포인터를 파생클래스 타입으로 캐스팅하는 것(안전하지 않을 수 있다.)

```cpp
void foo(Animal* p)
{
//  Dog* pDog = static_cast<Dog*>(p);
  Dog* pDog = dynamic_cast<Dog*>(p);
  if(pDog == 0) return;
  std::cout << pDog << std::endl;
}

int main()
{
  Animal a; foo(&a);
  Dog d; foo(&d);
}
```

## downcasting과 캐스팅 연산자

* static_cast - 잘못된 downcasting을 조사할 수 없다. (컴파일 시간에 캐스팅하기에 오버헤드 없음)
* dynamic_cast - 잘못된 downcasting을 하면 0을 반환(런타임 캐스팅하기에 오버헤드 발생)