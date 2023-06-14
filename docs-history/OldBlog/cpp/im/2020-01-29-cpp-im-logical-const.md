---
title: "(C++) 논리적 상수성"
date: 2020-01-29 00:00:00 -0000
---

```cpp
class Point
{
  int x, y;
public:
  Point(int a = 0, int b = 0) : x(a), y(b) {}
  
  const char* toString() const    // 상수 개체를 대비해서 const 선언
  {
    static char s[16];
    sprintf(s, "%d, %d", x, y);
    return s;
  }
};

int main()
{
  Point p1(1, 1);
  
  // 객체내부의 값을 확인해보고 싶다.
  cout << p1.toString() << endl;
}
```

> toString이 여러번 호출될 것을 대비해서 static char s[16]에 데이터를 기록해보자.

```cpp
class Point
{
  int x, y;
  
  char cache[16];
  bool cache_valid = false;
  
public:
  Point(int a = 0, int b = 0) : x(a), y(b) {}
  
  const char* toString() const
  {
    if(cache_valid == false)
    {
      sprintf(cache, "%d, %d", x, y); 
      cache_valid = true;       // error - toString이 const이기에 -> mutable 도입
    }
    
    return cache;
  }
};
```

```cpp
class Point
{
  int x, y;
  
  mutable char cache[16];
  mutable bool cache_valid = false;
```

> 다른 방법으로 해결할 순 없나?

```cpp
struct Cache
{
    char cache[16];
    bool cache_valid = false;
}

class Point
{
  int x, y;
  Cache* pCache;
  
public:
  Point(int a = 0, int b = 0) : x(a), y(b) 
  {
    pCache = new Cache;
  }
  
  const char* toString() const
  {
    if(pCache->cache_valid == false)
    {
      sprintf(pCache->cache, "%d, %d", x, y); 
      pCache->cache_valid = true;       // error - toString이 const이기에 -> mutable 도입
    }
```

---

## (추가) 상수 멤버 함수 참고 사항

```cpp
struct Test
{
  void foo { cout << "foo()" << endl; }             // 1
  void foo const { cout << "foo() const" << endl; } // 2
  
  void goo() const;
};

void Test::goo() const    // 구현부에도 꼭 const를 붙임을 기억! 아니면 비상수 함수화 컴파일러가 혼동!
{
  
}

int main()
{
  Test t1;
  t1.foo(); // 1번 우선 없으면 2번 호출
  
  const Test t2;
  t2.foo(); // only 2번
}
```