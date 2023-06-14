---
title: "(C++) this call"
date: 2020-01-28 00:00:00 -0000
---

## this call 이란?

> 우선, 함수는 객체마다 따로 만들어 지지않는다 라고 알고 있다.<br>
> (예를들어 아래의 p1, p2의 set이 따로 만들어지지 않음)<br>
> 그럼 p1의 set을 호출시 컴파일러는 어떻게 p1의 set임을 알까?<br>

```cpp
class Point
{
  int x = 0, y = 0;
public:
  void set(int a, int b)
  {
    x = a;
    y = b;
  }
};

int main()
{
  Point p1;
  Point p2;
  
  p1.set(10, 20);
}
```

> 컴파일러에서 어떻게 호출이 되는지 보자.

```cpp
// 컴파일러는 실제로 객체를 다음과 같이 호출하게 된다.
set(&p1, 10, 20);

// 받는쪽에서는
void set(Point* const this, int a, int b)
{
  this->x = a;
}
// 이런식으로 받게됨 -> 이런것을 this call이라 한다.
```

---

## 좀 더 정확하게 보자.

```cpp
p1.set(10, 20);   // 을 호출시
// push 20
// push 10
// (32bits) lea ecx, &p1
// (64bits) lea rcx, &p1
// call Point::set
// Point::set에서는 ecx or rcx를 사용
```

> 참고로 ecx과 rcx는 레지스터이고<br>
> lea는 메모리 복사함수이다.

* 멤버 함수는 1번째 인자로 객체의 주소(this)가 추가된다.

---

## 만약 static 멤버함수는 this call이 어떻게 되나?

```cpp
class Point {
  static void foo(int a)
  {
    x = a;
  }
};

Point::foo(10);
// push 10
// call Point::foo 이렇게 호출하게 되고...
```

```cpp
static void foo(int a)    // void foo(int a)
{
  x = a;    // this->x = a; 선언해야하는데 this가 없으니 error!
}
```

> 이러한 이유로 static멤버함수내에서는 멤버변수에 접근할 수 없다.

* static 멤버 함수는 개체의 주소(this)가 추가되지 않는다.

---

## Example-나만의 Thead를 만들어보자.

```cpp

class Thread
{
public:
  void run()
  {
    CreateThread(0, 0, threadMain, (void*)this, 0, 0);
  }

  // static을 선언한 이유를 알아야한다. - static을 선언하지 않으면 this가 넘어가 error발생!
  static DWORD __stdcall threadMain(void* p)
  {
    // Main();    
    // this가 없기에 이렇게 직접 호출이 어렵다.
    // 아래처럼 this를 전달하는 기법을 기억하자.

    Thread* const self = static_cast<Thread*>(p);
    self->Main();

    return 0;
  }

  virtual void Main()
  {}
};

class MyThread : public Thread
{
public:
  virtual void Main() {}    // 구현
};
```

---

## Example2-Timer를 만들어보자.

> 우선 다음을 기본으로 알아야 한다.<Br>
> 아래와 같이 타이머를 선언할 수 있다.

```cpp
#include <iostream>
#include "ecourse.hpp"
using namespace std;
using namespace ecouse;

void foo(int id)
{
  cout << "foo : " << id << endl;
}

int main()
{
  int n1 = ec_set_timer(500, foo);  // timer setting
  int n2 = ec_set_timer(1000, foo); // 타이머 구분용으로 리턴형으로 타이머의 id가 나온다.

  ec_process_message();              // timer 시작
}
```

> 다음을 기반으로 C++의 Timer를 생성해 보자.

```cpp
#include <iostream>
#include <string>
#include <map>
#include "ecourse.hpp"
using namespace std;
using namespace ecourse;

class Clock
{
  static map<int, Clock*> this_map;
  // 이런식으로 this를 map으로 많이 관리함을 기억하자.
  string name;
public:
  Clock(string n) : name(n) {}

  void start(int ms)
  {
    int id = ec_set_timer(500, timerHandler);
    this_map[id] = this;
  }

  // timerHandler의 인자가 하나여야하는데 인자가 두 개다.
  // void timerHandler(int id)
  static void timerHandler(int id)
  {
    // name에 접근이 불가능하다. + this를 받을 수도 없다?
    // cout << name << endl;
    
    Clock const self = this_map[id];
    cout << self->name << endl;
  }
};

map<int, Clock*> Clock::this_map;      // (필수) static으로 사용했기에 외부선언을 해준다.

int main()
{
  ec_process_messasge();
}
```

---

## Example 정리

* 1차 API는 보통 C언어, 2차 API는 보통 C++로 구현하게된다.
* 1차 API에서는 `CreateThread`, `SetTimer`와 같은 함수는 함수를 다시 호출하는 callback함수를 사용하게되는데
* 1차 API C언어에서는 클래스의 개념이 없기에 callback함수는 보통 static멤버 함수를 호출하게된다.(객체의 주소를 넘길 수 없기에)
* 결국 callback 함수에서 this를 사용하게 만들어 줘야한다. -> 그 두 가지 방법이 위의 예제다

