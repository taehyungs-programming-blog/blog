---
title: "(C++) Design Pattern - Factory"
date: 2020-01-22 00:00:00 -0000
---

## 목차

* [간단예제](#간단예제)
* [고급예제](#고급예제)

---

## 간단예제

```cpp
// Product 인터페이스
class Product
{
public:
    virtual void print() = 0;
};
```

```cpp
// Product 상속 클래스
class ConcreteProduct : public Product
{
public:
    void print() override { cout << "ConcreteProduct" << endl; }
};
```

```cpp
// Creator 클래스
class Creator
{
public:
    Product* AnOperation() { return FactoryMethod(); }
 
protected:
    virtual Product* FactoryMethod() = 0;
};
```

```cpp
// Creator 상속 클래스
class ConcreteCreator : public Creator
{
private:
    Product* FactoryMethod() { return new ConcreteProduct; }
};
```

```cpp
// 실제 사용
int _tmain(int argc, _TCHAR* argv[])
{
    ConcreteCreator pCreator;
 
    Product* pProduct = pCreator.AnOperation();
    pProduct->print();
 
    delete pProduct;
 
    return 0;
}
```

---

## 고급예제

* [(Factory Method) 한 점을 받는 오브젝트를 구현해보자.](#(Factory-Method)-한-점을-받는-오브젝트를-구현해보자.)
* [(Factory Method) 전체코드](#(Factory-Method)-전체코드)
* [Factory 구현 전체코드](#Factory-구현-전체코드)
* [Inner Factory](#Inner-Factory)
* [Abstract Factory](#Abstract-Factory)

---

### (Factory Method) 한 점을 받는 오브젝트를 구현해보자.

> cartesian, polar로 점을 받을 수 있다고 생각하면 생성자를 어떻게 만들어야할까?

```c
enum class PointType
{
    cartesian,
    polar
};

struct Point 
{
    //Point(float x, float y) : x(x), y(y) {}

    // param a this is either x or rho
    // param b this is either y or theta
    // param type 
    Point(float a, float b, PointType type = PointType::cartesian)
    {
        if(type == PointType::cartesian)
        {
            x = a;
            y = b;
        } else {
            x = a * cos(b);
            y = a * sin(b);
        }
    }

    float x, y;
};

int main()
{
    return 0;
}
```

> 흠... 이게 과연 최선인가? 생성자에 매번  cartesian, polar쓸지 적어주는게 최선인가?

---

### (Factory Method) 전체코드

> 이런 코드는 어떨까? 생성자 자체에서  cartesian, polar를 선택하게 해보자.

```c
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>

enum class PointType
{
  cartesian,
  polar
};

class Point
{
  /*Point(float a, float b, PointType type = PointType::cartesian)
  {
  if (type == PointType::cartesian)
  {
  x = a; b = y;
  }
  else
  {
  x = a*cos(b);
  y = a*sin(b);
  }
  }*/


  Point(const float x, const float y)
    : x{x},
      y{y}
  {
  }

public:
  float x, y;


  friend std::ostream& operator<<(std::ostream& os, const Point& obj)
  {
    return os
      << "x: " << obj.x
      << " y: " << obj.y;
  }

  static Point NewCartesian(float x, float y)
  {
    return{ x,y };
  }
  static Point NewPolar(float r, float theta)
  {
    return{ r*cos(theta), r*sin(theta) };
  }
};

int main_z()
{
  // will not work
  //Point p{ 1,2 };

  auto p = Point::NewPolar(5, M_PI_4);
  std::cout << p << std::endl;

  getchar();
  return 0;
}
```

---

### Factory 구현 전체코드

> Method보다 이런방식을 선호한다.

```c
#include <cmath>

enum class PointType
{
  cartesian,
  polar
};

class Point
{
  /*Point(float a, float b, PointType type = PointType::cartesian)
  {
    if (type == PointType::cartesian)
    {
      x = a; b = y;
    } 
    else
    {
      x = a*cos(b);
      y = a*sin(b);
    }
  }*/

  // use a factory method
  Point(float x, float y) : x(x), y(y){}
public:
  float x, y;

  friend class PointFactory;
};

class PointFactory
{
  static Point NewCartesian(float x, float y)
  {
    return Point{ x,y };
  }

  static Point NewPolar(float r, float theta)
  {
    return Point{ r*cos(theta), r*sin(theta) };
  }
};

int main()
{
    auto p = PointFacory::NewPolar(5, M_PI_4);
    cout << p << endl;

    return 0;
}
```

---

### Inner Factory

```c
#include <cmath>

// do not need this for factory
enum class PointType
{
  cartesian,
  polar
};

class Point
{
  /*Point(float a, float b, PointType type = PointType::cartesian)
  {
  if (type == PointType::cartesian)
  {
  x = a; b = y;
  }
  else
  {
  x = a*cos(b);
  y = a*sin(b);
  }
  }*/

  // use a factory method
  Point(float x, float y) : x(x), y(y) {}

  class PointFactory
  {
    PointFactory() {}
  public:
    static Point NewCartesian(float x, float y)
    {
      return { x,y };
    }
    static Point NewPolar(float r, float theta)
    {
      return{ r*cos(theta), r*sin(theta) };
    }
  };
public:
  float x, y;
  static PointFactory Factory;
};

int main_2()
{
  // will not work
  // Point p{ 1,2 };

  // nope!
  // Point::PointFactory pf;

  // if factory is public, then
  //auto p = Point::PointFactory::NewCartesian(3, 4);

  // at any rate, use this
  auto pp = Point::Factory.NewCartesian(2, 3);

  return 0;
}
```

---

### Abstract Factory

```cpp
// AbstractFactory.cpp
#include <iostream>
#include <memory>
#include <map>
#include "HotDrink.h"
#include "DrinkFactory.h"
using namespace std;

unique_ptr<HotDrink> make_drink(string type)
{
  unique_ptr<HotDrink> drink;
  if (type == "tea")
  {
    drink = make_unique<Tea>();
    drink->prepare(200);
  }
  else 
  {
    drink = make_unique<Coffee>();
    drink->prepare(50);
  }
  return drink;
}

int main53()
{
  auto d = make_drink("tea");

  DrinkFactory df;
  df.make_drink("coffee");

  getchar();
  return 0;
}
```

```c
// DrinkFactory.h
#pragma once
#include <string>
#include "HotDrink.h"
#include "TeaFactory.h"
#include "CoffeeFactory.h"
#include <map>
#include <functional>

struct HotDrink;

class DrinkFactory
{
  map<string, unique_ptr<HotDrinkFactory>> hot_factories;
public:
  DrinkFactory()
  {
    hot_factories["coffee"] = make_unique<CoffeeFactory>();
    hot_factories["tea"] = make_unique<TeaFactory>();
  }

  unique_ptr<HotDrink> make_drink(const string& name)
  {
    auto drink = hot_factories[name]->make();
    drink->prepare(200); // oops!
    return drink;
  }
};

class DrinkWithVolumeFactory
{
  map<string, function<unique_ptr<HotDrink>()>> factories;
public:

  DrinkWithVolumeFactory()
  {
    factories["tea"] = [] {
      auto tea = make_unique<Tea>();
      tea->prepare(200);
      return tea;
    };
  }

  unique_ptr<HotDrink> make_drink(const string& name);
};

inline unique_ptr<HotDrink> DrinkWithVolumeFactory::make_drink(const string& name)
{
  return factories[name]();
}
```

```c
// HotDrinkFactory.h
#pragma once
#include "HotDrink.h"

struct HotDrinkFactory
{
  virtual unique_ptr<HotDrink> make() const = 0;
};
```

```c
// CoffeeFactory.h
#pragma once
#include <memory>
#include "HotDrink.h"
#include "HotDrinkFactory.h"

struct HotDrink;

struct CoffeeFactory : HotDrinkFactory
{
  unique_ptr<HotDrink> make() const override
  {
    return make_unique<Coffee>();
  }
};
```

```c
// HotDrink.h
#pragma once
#include <memory>
#include <map>
#include <string>
using namespace std;

struct HotDrink
{
  virtual ~HotDrink() = default;

  virtual void prepare(int volume) = 0;
};

struct Tea : HotDrink
{

  void prepare(int volume) override
  {
    cout << "Take tea bag, boil water, pour " << volume << "ml, add some lemon" << endl;
  }
};

struct Coffee : HotDrink
{
  void prepare(int volume) override
  {
    cout << "Grind some beans, boil water, pour " << volume << "ml, add cream, enjoy!" << endl;
  }
};
```

* [TeaFactory.h](TeaFactory.h)
* [HotDrinkFactory.h](HotDrinkFactory.h)
* [HotDrink.h](HotDrink.h)
* [DrinkFactory.h](DrinkFactory.h)
* [CoffeeFactory.h](CoffeeFactory.h)
* [AbstractFactory.cpp](AbstractFactory.cpp)