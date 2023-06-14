---
title: "(C++) new vs operator new"
date: 2020-01-29 00:00:00 -0000
---

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
  // malloc : 메모리할당만
  Point* p1 = (Point*)malloc(sizeof(Point));
  free(p1);
  
  // new : 메모리할당 및 생성자 호출
  Point* p2 = new Point;
  delete p2;
}
```

```cpp
// 만약 메모리를 할당하고 싶지만 생성자는 호출하고 싶지않다면?
// 물론 C++ 방식으로
Point* p1 = static_cast<Point*>(operator new(sizeof(Point)));
operator delete(p1);
```

---

## Operator new 재 정의

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
  Point* p = new Point;
  // 1. 메모리 할당 (operator new)
  // 2. 생성자 호출
  delete p;
}
```

```cpp
// operator new 재 정의
void* operator new(size_t sz)
{
  cout << "my new" << endl;
  return malloc(sz);
}

// operator delete 재 정의
void operator delete(void* p) noexcept
{
  cout << "my delete" << endl;
  free(p);
}
```

```cpp
void* operator new(size_t sz, const char* s, int n)
{
  cout << "my new" << endl;
  return malloc(sz);
}

int main()
{
  Point* p = new Point;           // operator new(sizeof(Point))
  Point* p1 = new("AA", 2) Point; // operator new(sizeof(Point), "AA", 2)
}
```