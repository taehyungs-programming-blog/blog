---
title: "(C++) 임시객체와 함수"
date: 2020-02-08 00:00:00 -0000
---

```cpp
#include <iostream>
#include "Point.h"
using namespace std;

void foo(const Point& p)
{
  // 임시객체가 넘어오기에 매개변수는 항상 const로 받아야한다.
}

int main()
{
  Point pt(1, 1);
  foo(pt);
  
  // 임시객체를 사용한 인자 전달
  foo( Point(1, 1) );
  
  // 이걸 쓰나?
  // STL 비교문에서 함수를 전달할때 쓴다.
  sort(x, x + 10, greater<int>());
  
  cout << "end" < endl;
}
```

```cpp
// 임시객체와 함수의 리턴값
#include <iostream>
#include "Point.h"
using namespace std;

Point foo()
{
  Point pt(1, 1);
  return pt;
}

int main()
{
  Point ret(0,0);
  ret = foo();
  // 생성자 2회, 소멸자 2회 호출될까??
  // Nope! 생성자 2회, 복사생성자 1회, 소말자 2회, 복사소멸자1회 호출됨!!!
  
  // 값으로 return시 복사를해서 임시객체를 생성하게 된다.
  
  cout << "end" << endl;
}
```

```cpp
// 객체를 만들고 리턴하는거 보다
// 만들면서 리턴하는게 효울적
// RVO(Return Value Optimization)라 한다.
Point foo()
{
  return Point(1, 1);   // 복사생성자가 호출되지 않음.
  
  // 단, g++ 컴파일러는 NRVO라고 Named RVO를 해줘서 아래와 같이 코딩해도 자동으로 RVO를 적용한다.
  Point pt(1, 1);
  return pt;
}
```