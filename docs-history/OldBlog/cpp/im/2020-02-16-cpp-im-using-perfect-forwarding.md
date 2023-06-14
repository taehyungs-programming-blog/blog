---
title: "(C++) using perfect forwarding"
date: 2020-02-16 00:00:00 -0000
---

```cpp
#include <iostream>
#include <vector>
using namespace std;

class Point
{
    int x, y;
public:
    Point(int a, int b) { cout << "Point()" << endl; }
    ~Point() { cout << "~Point()" << endl; }
    Point(const Point&)
    {
        cout << "Point(const Point&)" << endl;
    }
};

int main()
{
    vector<Point> v;

    Point p(1, 2);
    v.push_back(p);

    // 소멸자가 몇번 호출될까?
    // 2회 호출된다 -> p가 생성되며 한 번 vecotr에 할당되며 한 번 총 2회

    // 이렇게 만들어지면 역시 오버헤드이다.

    // 해결책은?
}
```

```cpp
int main()
{
    vector<Point> v;

    v.emplace_back(1, 2);
    // 소멸자 호출 횟수 : 1
    // vector에 객체를 넣을때 push_back보다 emplace_back을 이용하자!
}
```

> 그런데 `emplace_back`이 perfact forwarding과 무슨상관인가?

```cpp
int main()
{
    vector<Point> v;

    int n = 20;
    v.emplace_back(1, 2, n);
    // n을 perfact forwarding으로 전달해 준다(STL에 그렇게 구현이 되어 있음.)
}
```

---

```cpp
// 또 다른 예제
#include <iostream>
#include <memory>
using namespace std;

class Point
{
    int x, y;
public:
    Point(int a, int b) { cout << "Point()" << endl; }
    ~Point() { cout << "~Point()" << endl; }
    Point(const Point&)
    {
        cout << "Point(const Point&)" << endl;
    }
};

int main()
{
    // 메모리 할당은 몇 번 될까?
    shared_ptr<Point> sp( new Point(1, 2) );
    // 총 2개의 메모리에 할당이 된다.
    // Point를 위한 메모리
    // 포인터 객체관리를 위한 메모리
    // 그런데 이런 할당은 메모리 파편화로 인한 성능 저하의 원인이 된다.

    // 해결법은 없나?
}
```

```cpp
shared_ptr<Point> sp = make_shared<Point>(1, 2);
// 객체와 포인트 객체 관리 메모리를 함께 생성해 달라

// 이 make_shared도 perfect forwarding으로 구성된다.
```