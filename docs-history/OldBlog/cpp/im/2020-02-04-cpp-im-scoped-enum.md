---
title: "(C++) scoped enum"
date: 2020-02-03 00:00:00 -0000
---

## 우선 기존 enum 상수의 단점은

* 타입의 이름이 없이 사용이 가능하다.(Unscoped enum)
* 요소의 타입을 지정할 수 없다.

```cpp
#include <iostream>
#include <type_traits>      // for underlying_type_t
using namespace std;

enum Color { red = 1, green = 2 };    // int, double인지 지정할 수 없다

int main()
{
  int n1 = Color::red;
  int n2 = red;   // 타입의 이름이 없이 사용이 가능! -> 문제점이 많다
  cout << typeid(underlying_type_t<Color>).name() << endl;    // unsigned int -> 1, 2뿐인데 데이터 나입이다.(char도 충분)
}
```

```cpp
enum class Color : char { red = 1, green = 2 };
// 요소의 타입을 직접 지정가능

cout << typeid(underlying_type_t<Color>).name() << endl;
// char

int n1 = Color::red; // error - char타입이라 int형에 넣을시 에러
int n2 = red;        // error - 그냥 red는 이제 존재하지 않는다.
```