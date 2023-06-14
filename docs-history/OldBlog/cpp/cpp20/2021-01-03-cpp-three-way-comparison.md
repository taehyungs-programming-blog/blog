---
title: "(c++20) three way comparison"
permalink: cpp/cpp20/three-way-comparison/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2021-01-03 00:00:00 -0000
last_modified_at: 2021-01-03 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
  - c++20
category:
  - three way comparison
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

## operator <=> : three-way comparison operator

```cpp
#include <iostream>

int main()
{
    int a = 10, b = 20;

    bool ret1 = a < b;
    auto ret2 = a <=> b;

    if (ret2 > 0) std::cout << "a > b" << std::endl;
    else if (ret2 < 0) std::cout << "a < b" << std::endl;
    else if (ret2 == 0) std::cout << "a == b" << std::endl;

    std::cout << typeid(ret2).name() << std::endl;      // 이 타입이 뭘지는 뒤에서 다룬다.
}
```

```cpp
#include <compare>

class Int32
{
    int value;
public:
    explicit Int32(int v = 0) : value(v) {}

    friend bool operator<(const Int32& obj1, const Int32& obj2) { return obj1.value < obj2.value; }
    friend bool operator<(const Int32& obj, int n) { return obj.value < n; }
    friend bool operator<(int n, const Int32& obj) { return n < obj.value; }
};

int main()
{
    Int32 n1{10};
    Int32 n2{20};

    bool b1 = n1 < n2;
    bool b2 = n1 < 20;
    bool b3 = 10 < n1;
}
```

이걸 <=>로 간단화 해보자.

```cpp
#include <compare>

class Int32
{
    int value;
public:
    explicit Int32(int v = 0) : value(v) {}

    // friend bool operator<(const Int32& obj1, const Int32& obj2) { return obj1.value < obj2.value; }
    // friend bool operator<(const Int32& obj, int n) { return obj.value < n; }
    // friend bool operator<(int n, const Int32& obj) { return n < obj.value; }

    auto opertor<=>(const Int32& other) const { return value <=> other.value; }
    bool opertor==(const Int32& other) const { return value == other.value; }

    auto opertor<=>(int other) const { return value <=> other; }
    bool opertor==(int other) const { return value == other; }
};

// 과연 이게 모든 연산자를 지원해 줄까??

int main()
{
    Int32 n1{10}, n2{20};

    bool b1 = n1 == n2;     // n1.opertor==(n2)
    bool b2 = n1 != 20;     // !(n1 == n2) -> 컴파일러가 이렇게 만들어 준다.
    bool b3 = n1 < n2;      // (n1 <=> n2) < 0> -> 컴파일러가 이렇게 만들어 준다.
    bool b4 = n1 > n2;      // (n1 <=> n2) < 0 -> 컴파일러가 이렇게 만들어 준다.
    bool b5 = n1 < 20;      // (n1 <=> 10) < 0
    bool b6 = 10 < n1;      // (10 <=> n1) < 0
}
```

---

## 좀 더 추가

```cpp
#include <compare>

class Point3D
{
    int x;
    int y;
    int z;
public:
    Point3D(int x = 0, int y = 0, int z = 0) : x{x},y{y},z{z} {}

    auto operator<=>(const Point3D& pt) const = default;    // <=>, ==까지 모두 컴파일러가 만들어 준다, 좀 더 커스텀 하고 싶다면??
};

int main()
{
    Point3D p1{1,2,3}, p2{3,2,1};

    bool b1 = p1 == p2;
    bool b2 = p1 < p2;
    bool b3 = p1 <= p2;
    bool b4 = p1 <=> p2;
}
```

```cpp
class Point3D
{
    int x;
    int y;
    int z;
public:
    Point3D(int x = 0, int y = 0, int z = 0) : x{x},y{y},z{z} {}

    auto operator<=>(const Point3D& pt) const
    {
        return z <=> pt.z;
    } // 단, == 연산자를 만들어 줘야한다.

    bool operator==(const Point3D& pt) const = default;
};
```

---

## <=>의 리턴형에 대해 보자

<=>의 반환타입 : std::strong_ordering, std::partial_ordering, std::weak_ordering

```cpp
#include <iostream>
#include <compare>

int main()
{
    int n1 = 10, n2 = 20;
    double d1 = 10, d2 = 20;

    auto ret1 = (n1 <=> n2);
    auto ret2 = (d1 <=> d2);
    auto ret3 = (n1 <=> d2);

    if ( ret1 < 0 ) {}      // ret1 == std::strong_ordering::less
    if ( ret1 > 0 ) {}      // ret1 == std::strong_ordering::greater
    if ( ret1 == 0 ) {}      // ret1 == std::strong_ordering::equal

    std::cout << typeid(ret1).name() << std::endl;      // std::strong_ordering
    std::cout << typeid(ret2).name() << std::endl;      // std::partial_ordering
    std::cout << typeid(ret3).name() << std::endl;      // std::partial_ordering
}
```

우선 설명에 들어가기 앞서<br>
C++20에서는 ==연산자를 만들면 !=가 자동으로 생성되고<br>
<=>연산자를 만들면, <, >, <= ,>=가 자동으로 생성된다를 알아두고 

```cpp
#include <iostream>
#include <compare>

struct Rect
{
    int x, y, w, h;
    Rect(int x, int y, int w, int h) : x{x}, y{y}, w{w}, h{h} {}

    //auto operator<=>(const Rect& r) const       // 그냥 auto로 잡으면 strong_ordering으로 리턴됨
    std::weak_ordering operator<=>(const Rect& r) const
    {
        return (w*h) <=> (r.w * r.h);
    }
    // strong_ordering과 weak_ordering
    // strong_ordering : 완벽하게 같다.
    // weak_ordering : 내 정책상 같다고 볼 수 있다.
};

int square(int a) { return a * a; }

int main()
{
    Rect r1(1, 1, 5, 2);
    Rect r2(1, 1, 2, 5);

    auto ret = r1 <=> r2;

    if ( ret == std::strong_ordering::equal )
    {
        // r1, r2 동일하다!
    }

    int n1 = 10, n2 = 10;

    if ( (n1 <=> n2) == 0)
    {
        assert( square(n1) == square(n2) );
    }
}
```

또 다른 Example

```cpp
#include <iostream>
#include <string>
#include <cstring>

class ci_string
{
    std::string str;
public:
    ci_string(std::string s) : str(s) {}
    //auto operator<=>(const ci_string& s) const { return strcmpi(str.c_str(), s.str.c_str()) <=> 0; }
    // 정확히 동일하지 않기에 weak_ordering이 맞다
    std::weak_ordering operator<=>(const ci_string& s) const { return strcmpi(str.c_str(), s.str.c_str()) <=> 0; }
};

int main()
{
    std::string s1("ABCD");
    std::string s2("abcd");

    ci_string s3("ABCD");
    ci_string s4("abcd");

    auto ret1 = (s1 <=> s2);
    auto ret2 = (s3 <=> s4);

    if ( ret1 == 0 ) { std::cout << "s1 == s2" << std::endl; }
    if ( ret2 == 0 ) { std::cout << "s3 == s4" << std::endl; }

    std::cout << typeid(ret1).name() << std::endl;
    std::cout << typeid(ret2).name() << std::endl;
}
```

---

## partial_ordering

```cpp
#include <iostream>
#include <string>

struct People
{
    std::string name;
    int age;
    People(std::string n, int a) : name(n), age(a) {}

    //auto operator<=>(const People& p) const
    std::weak_ordering operator<=>(const People& p) const
    {
        return age <=> p.age;
    }
};

int main()
{
    People p1("Kim", 20);
    People p2("Lee", 10);
    People p3("Park", -10); // 나이가 잘못들어간 사람이 있다면??

    auto ret = (p1 <=> p2);
}
```

```cpp
struct People
{
    std::string name;
    int age;
    People(std::string n, int a) : name(n), age(a) {}

    //auto operator<=>(const People& p) const
    std::weak_ordering operator<=>(const People& p) const
    {
        if ( age < 1 || p.age < 1) return std::partial_ordering::unordered; // 비교할수 없음을 알려주자
        return age <=> p.age;
    }
};

int main()
{
    People p1("Kim", 20);
    People p2("Lee", 10);
    People p3("Park", -10);

    auto ret = (p1 <=> p3);

    if ( ret = std::parital_ordering:unordered )
    {
        std::cout << "can not ordered" << std::endl;
    }
}
```