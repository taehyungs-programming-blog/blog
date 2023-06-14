---
title: "(C++) delete function"
date: 2020-02-03 00:00:00 -0000
---

```cpp
#include <iostream>
using namespace std;

void foo(int n)
{
  cout << "foo(int)" << endl;
}

int main()
{
  foo(3.4);   // 이런 암시적 변환이 버그를 유발하기도 한다.
}
```

```cpp
void foo(double) = delete;    // 이 함수를 사용하지 않겠다는 의미
```

---

## delete를 이용하여 복사 생성자를 막는 방안

```cpp
template<typename T> void goo(T a)
{
}

void goo(double) = delete;    // 이렇게 사용하면 아무리 goo라도 double형을 막을 수 있다.

class Mutex
{
public:

};

int main()
{
  goo(3.4);
  
  Mutex m1;
  Mutex m2 = m1;      // 복사 생성자가 자동으로 호출
  // 만약 복사 생성자를 막고 싶다면??
}
```

```cpp
// 방안 1
class Mutex
{
public:

private:
  Mutex(const Mutex&);
  // private에 둔다.
};
```

```cpp
class Mutex
{
public:
  // 방안 2
  Mutex(const Mutex&) = delete;
  void operator=(const Mutex&) = delete;
};
```

> 방안2가 더 좋음. delete라고 확실하게 명시하는게 좋다.

---

## default 생성자

```cpp
#include <iostream>
#include <type_traits>
using namepsace std;

struct Point
{
  int x, y;
  Point(int a, int b) : x(a), y(b) {}
};

int main()
{
  Point p1;   // 디폴트 생성자가 없기에 error
}
```

```cpp
struct Point
{
  int x, y;
  Point() {}            // 우리가 아는 일반 디폴트 생성자
  Point() = default;    // 이것도 디폴트 생성자이다.
  Point(const Point&) = default;
  Point(int a, int b) : x(a), y(b) {}
};
```

> 아니 저렇게 만드는게 무슨의미가 있나??<br>
> → 놉! 사용자가 만드는 디폴트 생성자와 컴파일러가 만드는 디폴트 생성자는 큰 의미의 차이가 있다.<br>
> → trivial과 연관이 있다.<br>

```cpp
struct Point
{
  int x, y;
  Point() {}           
  // 사용자가 생성자를 제공했기에 tirivial하지 못하다.
  Point(int a, int b) : x(a), y(b) {}
};

int main()
{
  Point p1{};   // value초기화
  
  cout << is_trivially_constructible<Point>::value << endl;
  // 0
  
  cout << p1.x << endl;
  // 쓰레기 값 출력
}
```

```cpp
struct Point
{
  int x, y;
  Point() = default;          
  // 컴파일러 입장에서 trivial함을 안다.
  Point(int a, int b) : x(a), y(b) {}
};

int main()
{
  Point p1{};   // value초기화
  
  cout << is_trivially_constructible<Point>::value << endl;
  // 1
  
  cout << p1.x << endl;
  // 0 출력(초기화 됨)
}
```