---
title: "(C++) 임시객체의 개념과 수명"
date: 2020-02-08 00:00:00 -0000
---

## 임시객체의 개념과 수명

```cpp
int main()
{
  int a = 1, b = 2, c = 3;
  int sum = a + b + c;
  int sum = ((a + b) + c);  // CPU의 계산순서는 다음과 같고, (a+b)는 임시변수에 잠시저장된다.
}
```

```cpp
// C++ 진영에서의 임시객체
#include <iostream>
#include "Point.h"
using namespace std;

int main()
{
  Point p1(1, 1);   // 이름 : p1, 파괴 : 함수{}를 벗어날때.
  Point(1, 1);      // 이름 : 없다, 파괴 : 문장의 끝(;) -> 이름이 없기에...
                    // 임시객체 : 클래스이름()
                    
  
  cout << "end" << endl;
}
```

---

## 임시객체와 참조

```cpp
#include <iostream>
#include "Point.h"
using namespace std;

int main()
{
  Point pt(1, 1);   // pt : 이름이 있는 객체
  pt.x = 10;
  
  Point(1,1).x = 10;    // error. 특징 1. 임시객체는 lavalue가 될 수 없다.
  
  Point* p1 = &pt;    // ok
  Point* p2 = &Point(1,1);    // error. 특징 2. 임시객체는 주소를 구할수 없다.
  
  Point& r1 = pt;     // ok
  Point& r2 = Point(1,1);     // error. 특징 3. 임시객체는 참조할 수 없다.
  
  const Point& r3 = pt;   // ok
  const Point& r4 = Point(1,1); // ok 특징 4. 임시객체는 const참조로 참조할 수 있다.
  // 임시 객체를 const 참조로 받는 경우 임시객체의 수명또한 늘어난다.
  
  // 아니 막을꺼면 다 막지? 왜 const는 되나? -> 뒤에 나옴!
  
  r4.x = 10; // error
  
  // rvalue reference
  Point&& r5 = Point(1,1);    // ok. 특징 5. 임시객체는 rvalue 참조가 가능
  r5.x = 10;      // ok
}
```