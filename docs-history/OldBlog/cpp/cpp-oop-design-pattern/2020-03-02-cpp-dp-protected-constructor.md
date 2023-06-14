---
title: "(C++) protected constructor"
date: 2020-03-02 00:00:00 -0000
---

## protected 생성자

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

* 자기 자신은 만들 수 없지만,
* 파생 클래스의 객체는 만들게 하고 싶을때

---

## protected 소멸자

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
    // 파괘될때 소멸자를 부르지 못하기 때문이다.

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

    void Destroy() { delete this; }

protected:
    ~Car() { cout << "~Car" << endl; }
};
```

* 스택에 객체를 생성하지 못하게 하고 싶을때
* 참조 계수 기반의 객체 수명 관리 기업에서 주로 사용된다.