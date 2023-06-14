---
title: "(C++) 멤버 변수 포인터"
date: 2020-01-29 00:00:00 -0000
---

```cpp
class Point
{
public:
  int x, y;
};

int main()
{
  int n = 10;
  int* p1 = &n;
  
  // 멤버 변수 포인터
  int Point::*p2 = &Point::x;
  
  // 그런데 포인터를 선언한 적이 없는데 ??
  // 멤버 변수 포인터는 진짜 포인터 값이 아니라 offset값을 의미한다.
  int Point::*p2 = &Point::x;   // 0
  int Point::*p3 = &Point::y;   // 4
  printf("%d", p2);     // 참고로 멤버 변수 포인터는 cout으로 출력이 불가능
  
  Point pt;
  pt.y = 10;
  pt.*p3 = 20;
  
  cout << pt.y << endl; // 20
}
```