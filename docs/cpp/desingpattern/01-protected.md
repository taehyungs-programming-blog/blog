---
layout: default
title: "1. (기초) Protected 생성자/소멸자"
parent: (Desing Pattern)
grand_parent: C++
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Protected 생성자

🦄 **Protected 생성자**란?<br>
🦄 자기 자신은 생성할 수 없지만, 파생 클래스의 객체는 만들게 하고 싶을때 사용된다.<br>
🦄 간단히 말하면 **파생클래스의 생성만 가능**하게 하고싶을때 사용된다.

```cpp
class Animal
{
protected:
    Animal() {}
};

class Dog : public Animal
{
public:
    Dog() {}    // Dog() : Animal() {} <- 컴파일러에서 이런식으로 호출됨.
};

int main()
{
    Animal a;       // error - 생성자가 불러지지 않는다.
    // 이런식으로 객체의 생성을 막을 수 있다.
    Dog d;          // ok
}
```

---

## Protected 소멸자

🦄 **Protected 소멸자**란?<br>
🦄 스택에 객체를 생성하지 못하게 하고 싶을때, 참조 계수 기반의 객체 수명 관리 기업에서 주로 사용된다.<br>
🦄 어렵게 설명했는데 객체의 **소멸을 개발자가 만든 함수를 통해서만 가능**하게 해야할때(delete 불가) 사용된다.

```cpp
#include <iostream>
using namespace std;

class Car
{
public:
    Car() {}

protected:
    ~Car() { cout << "~Car" << endl; }
};

int main()
{
    Car c;      // error - 스택에 객체를 만들 수 없다.
    // 파괴될때 소멸자를 부르지 못하기 때문이다.

    Car* p = new Car;
    delete p;   // error
    // -> 소멸을 위한 함수를 별도로 만들어줘야한다.
}
```

```cpp
class Car
{
public:
    Car() {}

    // 이런식으로 소멸해야함.
    void Destroy() { delete this; }

protected:
    ~Car() { cout << "~Car" << endl; }
};
```