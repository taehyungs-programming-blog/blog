---
title: "(C++) placement new"
date: 2020-01-29 00:00:00 -0000
---

## 생성자를 호출하고 싶다면? (placement new)

```cpp
class Point
{
    int x, y;
public:
    Point() { cout << "Point()" << endl; }
    ~Point() { cout << "~Point()" << endl; }
};

int main()
{
    Point p;

    p.Point();  // 이런 호출이 가능한가?
    // error 소멸자는 명시적 호출이 가능하나 생성자는 명시적호출이 불가능하다.
}
```

> 그렇다면 생성자를 명시적으로 호출할 방법은 아에없는것인가?

```cpp
void* operator new(size_t sz, Point* p)
{
    return p;
}

int main()
{
    Point p;

    new Point;          // 인자가 1개인 operator new() 호출
    new(&p) Point;      // 인자가 2개인 operator new() 호출

    // 우선 new의 동작 방식을 알아야한다.
    // 1. 메모리 할당 - operator new() 함수 사용
    // 2. 생성자 호출
    // 3. 메모리 주소를 해당 타입으로 변환

    // 즉 위 선언은 메모리 할당이 아닌 생성자를 호출하는 코드이다.
}
```

---

## 모든 타입의 생성자를 호출하고 싶다면?

```cpp
void* operator new(size_t sz, void* p)
{
    return p;
}
```

> 사실 위 함수는 C++표준이다.<br>
> 따라서 위 선언을 하지 않고 `new(&p) Point`만 해도 생성자가 호출이 된다.<br>

---

## new Point vs new(p1) Point

```cpp
int main()
{
    // malloc : 메모리만 할당
    Point* p1 = (Point*)malloc(sizeof(Point));

    // new : 객체의 생성(메모리 할당+생성자 호출)
    Point* p2 = new Point;

    Point* p3 = new Point;      // 새로운 메모리에 객체를 생성해 달라.
    Point* p4 = new(p1) Point;  // 기존 메모리에 객체를 생성해 달라(생성자 호출).
}
```

> 그럼 기존 메모리에 객체를 생성하는 경우가 잦나? -> 다음강에서 설명!