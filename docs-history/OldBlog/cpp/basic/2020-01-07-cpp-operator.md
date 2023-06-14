---
title: "(C++) 연산자 재정의"
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-01-08 00:00:00 -0000
last_modified_at: 2020-04-04 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

연산자 재정의 방법은 크게 두 가지 방법이 있다.
1. 일반 함수로 구현
2. 멤버 함수로 구현

---

## 일반 함수로 연산자 재정의

```cpp
#include <iostream>

class Point
{
    int x;
    int y;
public:
    Point(int a = 0, int b = 0) : x(a), y(b) {}

    void print() const
    {
        std::cout << x << ", " << y << std::endl;
    }

    friend Point operator+(const Point& p1,const Point& p2);
};

Point operator+(const Point& p1,const Point& p2)
{
    Point temp;
    temp.x = p1.x + p2.x;
    temp.y = p1.y + p2.y;

    return temp;
}

int main()
{
    Point p1(1,1);
    Point p2(2,2);
    Point p3 = p1 + p2;   // operator+(p1,p2)
}
```

---

## 멤버 함수로 연산자 재정의

```cpp
#include <iostream>

class Point
{
    int x;
    int y;
public:
    Point(int a = 0, int b = 0) : x(a), y(b) {}
    
    void print() const
    {
        std::cout << x << ", " << y << std::endl;
    }
    
    // 멤버로 만드는 operator+
    Point operator+(const Point& p)
    {
        Point temp;
        temp.x = p.x + this->x;
        temp.y = p.y + this->y;
        return temp;
    }
};

int main()
{
    Point p1(1,1);
    Point p2(2,2);
    Point p3 = p1 + p2;   // p1.operator+(p2)
}
```

---

## 연산자 재정의를 일반함수와 멤버함수로 만들었을때의 차이점은?

* 우선순위가 다르며 멤버함수가 먼저 호출 된다.

---

## 연산자 재정의 주의 사항

* 인자가 모두 primitive 타입인 경우 오버로딩 불가

```cpp
int operator+(int a, int b)
{
    return a - b;
}
```

* 다음 연산자는 오버로딩 할 수 없다.

`.*`, `::`, `?:`, `sizeof`, `typeid`, `static_cast`, `dynamic_cast`, `reinterpret_cast`, `const_cast`<br>
단, `.`은 C++20부터 가능<br>

* 멤버 함수로만 오버로딩 가능한 연산자가 있다.

`=`, `()`, `[]`, `->`

* 새로운 연산자를 만들거나 인자의 개수를 변경하거나, 연산자 우선순위를 변경할 수 없다.
* 디폴트 파라미터를 사용할 수 없다.
* 다음 연산자는 중요하기에 뒤에서 다시 설명

`<<`, `++`, `[]`, `()`, `->`, `=`

---

## Example-cout을 직접만들어 보자.

primitive type return

```cpp
#include <cstdio>

namespace std {
    class ostream
    {
    public:
        ostream& operator<<(int n) { printf("%d", n); return *this; }
        // 참고) 리턴형을 그냥 ostream할 경우 자기 자신이 복사되어 리턴된다.
        ostream& operator<<(double n) { printf("%f", n); return *this; }
    };
    ostream cout;
}


int main()
{
    cout << 3;
    cout << 3.4;
}
```

사용자 정의 타입 return

```cpp
class Point
{
    int x;
    int y;
public:
    Point(int a = 0, int b = 0) : x(a), y(b) {}
    
    friend std::ostream& operator<<(std::stream& os, const Point& pt);
};

std::ostream& operator<<(std::ostream& os, const Point& pt)
// 참고로 std::ostream& os에 const를 붙이지 않는 이유는 
// const를 붙이게 되는 경우 os가 상수형 변수가 되고 상수형 함수만 호출가능
// os.operator는 상수형 함수가 아니기에 에러가 발생한다.
{
    os << pt.x << ", " << pt.y;
    return os;
}

int main()
{
    Point pt(1, 2);
    std::cout << pt;
}
```

---

## 증가/감소 연산자 재정의

```cpp
// 우선 ++은 전위형/후위형이 있는데 각 구현은 아래와 같이 한다.
class Point
{
    int x;
    int y;
public:
    Point(int a = 0, int b = 0) : x(a), y(b) {}
    
    void print() const
    {
        std::cout << x << ", " << y << std::endl;
    }
  
  // 전위형
    Point& operator++() 
    {
        ++x;
        ++y;
        return *this;
    }
  
  // 후위형 - 왜 아래와 같은지는 문법이니 받아들이자.
    Point operator++(int) 
    {
        Point temp = *this;
        
        ++(*this);  // 전위형 호출
        // ++x;
        // ++y;
        return temp;
    }
};

int main()
{
    Point p(1, 1);
    ++p;      // 전위형
    ++++p;    // 이거도 가능!
    
    p++;   // 후위형
}
```

---

## 스마트 포인터 만들어보기

```cpp
#include <iostream>

class Car
{
    int color;
public:
    ~Car() { std::cout << "~Car()" << std::endl; }
    
    void Go() { std::cout << "Car Go" << std:: endl; }
};

class Ptr
{
    Car* pObj;
public:
    Ptr(Car* p = 0) : pObj(p) {}
    ~Ptr() { delete pObj; }
    
    Car* operator->() { return pObj; }
};

int main()
{
    Ptr p = new Car;    
    // p는 객체인데 포인터 처럼 사용중?
    // Ptr p(new Car); 라고 생각
    p->Go();    // p.operator()->Go()로 재정의 해줌
    
    // 위와 같이 스마트 포인터를 간단히 구현가능.
}
```

---

## 상태를 갖는 함수 만들기

```cpp
#include <iostream>

// 0~9 사이의 난수 얻기
int frand() { return rand() & 10; }

class URandom
{
public:
    int operator()()
    {
        return rand() % 10;
    }
};

int main()
{
    URandom orand;
    
    std::cout << ornad() << std::endl;  
    std::cout << ornad() << std::endl;
    std::cout << ornad() << std::endl;
    // 세 번의 호출 모두 같은 값이 나올 확률이 크다.
    
    std::cout << frand() << std::endl;
}
```

```cpp
class URandom
{
    char history[10];
public:
    URandom()
    {
        for(int i = 0; i < 10; i++)
        history[i] = 0;
        srand(time(0));
    }
    int operator()()
    {
        int n = -1;
        do {
        n = rand() % 10;
        } while (history[n] == 1)
        
        history[n] = 1;
        return n;
    }
};
```

---

## String 만들기

이런 생성 패턴이 자주 사용되니 기억하자.

```cpp
#include <iostream>
#include <cstring>
using namespace std;

class String
{
    char* buff;
    int size;
public:
    String(const String& s) : size(s.size)
    {
        buff = new char[size+1];
        strcpy(buff, s.buff);
    }
    
    String(const char* s)
    {
        size = strlen(s);
        buff = new char[size+1];
        strcpy(buff, s);
    }

    ~String() { delete[] buff; }
    
    // 자기 자신에 대입하면 문제가 발생
    String& operator=(const String& s)
    {
        // 대입연산자는 자신과의 대입을 조사
        if(&s == this)
        return *this;
        
        size = s.size;
        delete[] buff;
        buff = new char[size+1];
        strcpy(buff, s.buff);
        
        return *this;
    }
    
    friend ostraem& operator<<(ostream& os, const String& s);
};

ostraem& operator<<(ostream& os, const String& s)
{
    return os << s.buff;
}

int main()
{
    String s1 = "hello";
    cout << s1 << endl;
    
    String s2 = s1;
    cout << s2 << endl;
    
    String s3 = "world";
    cout << s3 << endl;
    
    s3 = s1;
    cout << s3 << endl;
}
```