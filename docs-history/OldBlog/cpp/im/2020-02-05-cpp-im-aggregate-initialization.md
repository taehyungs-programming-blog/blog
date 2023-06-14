---
title: "(C++) aggregate initialization"
date: 2020-02-05 00:00:00 -0000
---

```cpp
struct Point
{
  int x, y;
  // Point() {}              // 1
  // Point(int a, int b) {} // 2
};

int main()
{
  Point p1;     // 컴파일러가 제공하는 디폴트 생성자
  Point p2{1,2};  // 컴파일러가 중괄호로 초기화 된경우 알아서 할당
  // 단 Point() {}가 있는경우 컴파일러가 알아서 못해주기에 에러발생
}
```

```cpp
struct Point
{
  int x, y;
   Point() = default;
};

int main()
{
  Point p1;   // ok
  Point p2{1,2};  // ok
}
```

* aggregate type : {}로 초기화 가능한 것, 구조체, 배열
* aggregate type의 조건이 있음 → cppreference에서 참조하자.(많음)