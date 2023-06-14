---
title: "(C++) 생성자의 명시적 호출이 필요한 이유"
date: 2020-01-29 00:00:00 -0000
---

## 메모리할당과 생성자 호출을 분리하면 편할때가 많다.

```cpp
#include <iostream>
using namespace std;

class Point
{
    int x, y;
public:
    Point(int a, int b) : x(a), y(b)
    {
        cout << "Point(int, int)" << endl;
    }
};

int main()
{
    // Point 객체를 힙에 한 개 만들고 싶다
    Point* p1 = new Point(0, 0);        // ok

    // Point 객체를 힙에 10개 만들고 싶다.
    Point* p2 = new Point[10];  // error - 디폴트 생성자가 없기에 에러발생

    // 방법이 없나?
    // 1. 메모리만 먼저 힙에 할당
    Point* p2 = static_cast<Point*>(operator new(sizeof(Point)*10));

    // 2. 할당한 메모리에 객체를 생성(생성자 호출)
    for(int i = 0; i < 10; i++) 
        new(&p2[i]) Point(0,0);

    for(int i = 9; i >= 0; i--)
        p2[i].~Point();

    operator delete(p2);
}
```

> 대표적 예시는 vector가 있다.

```cpp
vector<Point> v(10, Point(0,0));
// 이렇게 선언하면 vector는 메모리를 10개 선언 후 Point(0.0)의 복사 생성자를 호출해서 복사한다.
```

---

## 대표적 예시는 vector이다.

```cpp
#include <iostream>
#include <vector>
using namespace std;

int main()
{
    vector<int> v(10, 0);

    v.resize(7);

    cout << v.size() << endl;           // 7
    cout << v.capacity() << endl;       // 10
    // 사용할 메모리는 7로 줄였지만 실제론 메모리가 10개 할당되어있음.(STL에서 결정)
}
```

```cpp
// DBConnect : 생성자에서 DB접속
vector<DBConnect> v2(10);

v2.resize(7);       // 메모리는 제거하지않지만 줄어든 객체의 소멸자는 호출해 줘야한다.
v2.resize(8);       // 새로운 객체에 대한 메모리는 있지만, 생성차를 호출해 줘야한다.
```

---

## 또 다른 예시

```cpp
#include <Windows.h>
#include <iostream>
using namespace std;

class Point
{
    int x, y;
public:
    Point(int a, int b) : x(a), y(b)
    {
        cout << "Point(int, int)" << endl;
    }
};

int main()
{
    // IPC : 공유메모리를 사용한 프로세스간 통신
    HANDLE hMap = CreateFileMappingA(INVALID_HANDLE_VALUE, 0, PAGE_REAWRITE, 0, sizeof(Point), 0);
    Point* p = (Point*)MapVeiwOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    // C언어로 구현되었기에 메모리만 할당되고 생성자는 호출되지 않음.

    new(p) Point(0, 0);     // 이런식으로 생성자를 호출한다.
}
```