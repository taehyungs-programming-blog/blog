---
title: "(C++) 변환 연산자/변환 생성자"
date: 2020-02-01 00:00:00 -0000
---

```cpp
class Point
{
    int x, y;
public:
    Point() : x(0), y(0) {}
    Point(int a, int b) : x(0), y(0) {}
};

int main()
{
    int n = 3;
    double d = n;       // 표준타입일 경우 암시적형 변환이 일어난다.

    Point p1(1, 2);
    n = p1;             // 사용자 정의 타입도 암시적 형변환이 되나?
    // error! - 컴파일러는 p1을 integer로 변경하지 못함.
    // 만약 p1.opertor int() - 변환연산자가 있다면 가능하게 된다.

    cout << n << endl;
}
```

> p1.opertor int()를 만들어보자.

```cpp
class Point
{
    int x, y;
public:
    Point() : x(0), y(0) {}
    Point(int a, int b) : x(0), y(0) {}

    // int operator int()
    operator int()  // 이렇게 표기해도 된다.
    {
        return x;
    }
};

int main()
{
    Point p1(1, 2);
    n = p1;         // ok

    cout << n << endl;
}
```

---

## 변환 생성자

```cpp
class Point
{
    int x, y;
public:
    Point() : x(0), y(0) {}
    Point(int a, int b) : x(0), y(0) {}

    operator int() { return x; }
};

int main()
{
    int n = 3;
    Point p(1, 2);
    
    n = p;      // point to int -> p.operator int()
    p = n;      // int to point -> n.operator Point() ??? int은 객체가 아니라 error!
}
```

```cpp
class Point
{
    int x, y;
public:
    Point() : x(0), y(0) {}
    Point(int a, int b) : x(0), y(0) {}
    // 매개변수가 하나인 생성자를 만들어보자.

    Point(int a) : x(a), y(0) {}
    // 변환 생성자라 하며 다른 타입이 Point로 변환가능하게 해준다.

    operator int() { return x; }
};

int main()
{
    int n = 3;
    Point p(1, 2);
    
    n = p;
    p = n;      // ok
}
```