---
title: "(C++) default vs value constructor"
date: 2020-02-05 00:00:00 -0000
---

```cpp
int main()
{
  // dirct initialization
  int n1(0);
  int n2{0};
  
  // copy initialization
  int n3 = 0;
  int n4 = {0};
  
  // ---
  int n5;       // default initizlization -> 쓰레기값 초기화
  int n6{};     // value initialization -> 0으로 초기화
  
  int n7();     // 주의, 이거는 그냥 함수의 선언이다.
}
```

```cpp
// 사용자 정의 타입에서 default, value initialization
#include <iostream>
using namespace std;

class Point
{
public:
  int x, y;
};

int main()
{
  Point p1;   // default
  Point p2{}; // value
  
  cout << p1.x << endl;     // 쓰레기 값
  cout << p2.x << endl;     // 0 출력
}
```

```cpp
// 사용자가 생성자까지 만든경우는?
#include <iostream>
using namespace std;

class Point
{
public:
  int x, y;
  
  Point() {}
};

int main()
{
  Point p1;   // default
  Point p2{}; // value
  
  cout << p1.x << endl;     // 쓰레기 값
  cout << p2.x << endl;     // 쓰레기 값 -> 자동으로 0이 초기화가 안된다.
}
```

```cpp
// default 생성자는?
#include <iostream>
using namespace std;

class Point
{
public:
  int x, y;
  
  Point() = default;
};

int main()
{
  Point p1;   // default
  Point p2{}; // value
  
  cout << p1.x << endl;     // 쓰레기 값
  cout << p2.x << endl;     // 0
}
```

> 생성자를 만들면서 아무것도 코딩을 안하는거 {}랑 컴파일러에게 만들어 달라는거 default는 큰 차이가 있다.

```cpp
// 마지막 정리!
#include <iostream>
using namespace std;

int main()
{
  int n1;       // default, 쓰레기 값
  int n2{};     // value, 0
  int n3();     // 함수선언
  
  int* p1 = new int;    // default, 쓰레기
  int* p2 = new int();  // value, 0
  int* p3 = new int{};  // value ,0
  
  cout << *p1 << endl;
  cout << *p2 << endl;
  cout << *p3 << endl;
  
}
```