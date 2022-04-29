---
layout: default
title: "2. (기초) 추상 클래스 / interface"
parent: (Desing Pattern)
grand_parent: C++
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## 추상 클래스 

🦄 추상 클래스는 반드시 **특정 함수를 만들라고 강제**하고 싶을때 사용된다.

```cpp
class Shape     // 순수 가상함수를 포함할 경우 추상 클래스라 한다.
{
public:
    virtual void Draw() = 0;        // 순수 가상함수 : 구현부가 없다.
};

int main()
{
    Shape s;        // error - 추상 클래스는 객체를 생성할 수 없다.
    Shape* p;       // ok - 단, 포인터 정도는 만들 수 있다.
}
```

```cpp
class Rect : public Shape
{
public:
    virtual void Draw() {}      // 구현부를 제공하면 추상 클래스가 아니다.
};

int main()
{
    Rect r;     // ok
}
```

---

## interface 

🦄 하나의 예로서 설명해 보자.

```cpp
#include <iostream>
using namespace std;

class Camera
{
public:
    void take() { cout << "take picture" << endl; }
};

class People
{
public:
    void useCamera(Camera* p) { p->take(); }
};

int main()
{
    People p;
    Camera c;
    p.useCamera(&c);
}
```

🦄 새로운 카메라가 나왔다고 가정해보자.

```cpp
class HDCamera
{
public:
    void take() { cout << "HD take picture" << endl; }
};

class People
{
public:
    // 이렇게 무한정 늘릴순 없다.
    void useCamera(Camera* p) { p->take(); }
    void useCamera(HDCamera* p) { p->take(); }
};

int main()
{
    People p;
    Camera c;
    p.useCamera(&c);

    HDCamera hd;
    p.useCamera(&hd);       // 효율화가 필요하다
}
```

🦄 개선해 보자.

```cpp
#include <iostream>
using namespace std;

// 규칙 : 모든 카메라는 아래 클래스로부터 파생되어야 한다.(=모든 카메라는 아래 인터페이스를 구현해야 한다.)
struct ICamera
{
public:
    virtual void take() = 0;
    virtual ~ICamera() {}
};

class Camera : public ICamera
{
public:
    virtual void take() override { cout << "take picture" << endl; }
};

class HDCamera : public ICamera
{
public:
    virtual void take() override { cout << "HD take picture" << endl; }
};

class People
{
public:
    void useCamera(ICamera* p) { p->take(); }
};

int main()
{
    People p;
    Camera c;
    HDCamera hd;

    p.useCamera(&c);        // ok
    p.useCamera(&hd);       // ok
}
```

---

## 어렵게 설명하자면...

* **OCP(Open Close Principle)** - 기능 확장에는 열려있고, 코드 수정에는 닫혀 있어야 한다.(새로운 기능이 추가되어도 기존 코드의 수정이 없도록 만들어야 한다는 말.)
* 문제의 원인은 People과 Camera가 useCamera를 통해 강한결합으로 연결되어 있다.
이 부분을 약한결합으로 변경하면 될 것.
* 해결된 구현은 약한 결합이라 한다.
* 객체가 서로 상호 작용 하지만, 서로에 대해 잘 알지 못 하는 것. 교체/확장 가능한 유연한 디자인
* 객체는 상호 간에 인터페이스를 통해서 통신 해야 한다.