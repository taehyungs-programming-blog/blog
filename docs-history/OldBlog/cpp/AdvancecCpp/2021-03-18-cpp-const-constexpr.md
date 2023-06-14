---
title: "(Advanced C++) const, constexpr"
permalink: cpp/cpp11/const-constexpr/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2021-03-18 00:00:00 -0000
last_modified_at: 2021-03-18 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
  - advanced
category:
  - const, constexpr
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

### constexpr

`constexpr`은 컴파일 시간 상수 만 넣을 수 있다. (컴파일 할 당시 상수인 것)<br>

```cpp
int n = 10;

const int c1 = 10;    // 컴파일 시간에 알 수 있다.(컴파일 시간 상수)
const int c2 = n;     // 컴파일 시간에 알 수 없다. (실행시간 상수)

constexpr int c3 = 10;    // Ok
constexpr int c4 = n;     // Compile Error, 컴파일 시간 상수만 넣을 수 있다.
```

```cpp
// const double pi = 3.14;
constexpr double pi = 3.14;
```

---

### constexpr은 보통 이렇게 사용된다.

1. constexpr 상수
2. constexpr function
3. if constexpr

```cpp
constexpr int add(int a, int b)
{
  return a + b;
}

int main()
{
  int x = 1;
  int y = 1;
  
  int n1 = add(1, 1);   // 컴파일 시 인자의 값을 알 수 있다면 컴파일 시 계산
  int n2 = add(x, y);   // 그렇지 않다면 런타임에 계산
}
```

```cpp
#include <iostream>

constexpr int factorial(int n)
{
  // file open, new 등을 할 순 없다(컴파일 타임에 결정이 되지 못함.)
  if(n == 1)
    return 1;
  return n * factorial(n-1);
}

int main()
{
  int n = factorial(5);
  
  std::cout << n << std::endl;
}
```

```cpp
if constexpr(n == 0)
// 컴파일 시간에 조건식을 조사하라.
// 단, 컴파일 시간에 조건검사가 가능하게 변수가 지정이 되어 있어야함.
```

---

### 추가

```cpp
int arr[6];   // OK
int A() { return 3; }
int arr[A()+3];   // Compile Error

// C++ 11
constexpr int A() { return 3; }   // Force the computation to happen at Compile Time.
int arr[A() + 3];   // OK

constexpr int cubed(int x) { return x * x * x; }
int y = cubed(1789);    // computed at compile time
```

---

## const(Effective C++)

* [참고사이트](https://www.youtube.com/watch?v=6lQt4UaOXII&list=PLtb6y_I5vgIT7IPpaUp4r2U0viXw2iFeX&index=3)

---

```cpp
class COverLapCode
{
private:
    std::string m_sName;
public:
    explicit COverLapCode(std:string _sName) : m_sName(_sName) {}

    const char& operator[](std::size_t uiPos) const
    {
        return m_sName[uiPos];
    }

    // 위 아래의 차이점이 뭘까??

    char& operator[](std::size_t uiPos)
    {
        return m_sName[uiPos];
    }

    // COverLapCode를 const(상수)화 해서 사용하면 const를 붙인 operator가 호출되고
    // 상수화 하지 않으면 아래의 operator가 호출된다.

    // 근데 이거 너무 코드의 중복 아닌가?
};
```

```cpp
class COverLapCodeImprove
{
private:
    std::string m_sName;

public:
    explicit COverLapCodeImprove(std::string _sName) : m_sName(_sName) {}

    const char& operator[](std::size_t uiPos) const
    {
        return m_sName[uiPos];
    }

    char& operator[](std::size_t uiPos)
    {
        return const_cast<char&>(static_cast<const COverLapCodeImprove&>(*this)[uiPos]);

        // 자신(this)를 const 로 타입변경 후
        // const char&로 리턴받은 결과를 char&로 다시 캐스팅
    }
};
```

```cpp
class CPhysical
{
private:
    char* m_cpName;

public:
    CPhysical(char* _cpName) : m_cpName(NULL)
    {
        m_cpName = new char[sizeof(_cpName)];
        strcpy(m_cpName, _cpName);
    }

    char& operator[](std::size_t uiPos) const
    // 무리적 상수성 : 함수의 끝에 const를 붙여서 함수내에서 상수성을 보존해준다.
    {
        return m_cpName[uiPos];
    }
};
```

```cpp
class CLogical
{
private:
    char* m_cpName;

    // mutable - 데이터 멤버들은 어떠한 경우에도 수정이 가능하게 해준다.
    mutable std::size_t m_uiPos;        
    mutable bool m_bLengthCheck;

public:
    CLogical(char* _cpName) : m_cpName(_cpName), m_bLengthCheck(false) {}

    std::size_t length() const
    {
        if(!m_bLengthCheck)
        {
            m_uiPos = std::strlen(m_cpName);        // const 함수 내부에서 값 변경
            m_bLengthCheck = true
        }
        return m_uiPos;
    }
};
```

```cpp
// 물리적, 논리적 음... 근데 언제 쓰나??

const CPhysical Physical("Physical");
// const 객체의 const 멤버 함수
// 하지만 값 변경이 된다.
Physical[0] = 'H';
std::cout << Physical[0] << std::endl;      // operator로 리턴되는 타입이 그냥 레퍼런스 타입 - 수정이 가능해진다.

char* pc = &Physical[0];
*pc = 'c';
std::cout << Physical[0] << std::endl;

// 사실 operator에 리턴에 const를 붙이면 해결...
// 하고싶은 말은 이런 모순이 발생할 수 있다는 점.
```

---

## const (Advanced C++)

* [참고](https://www.youtube.com/watch?v=7arYbAhu0aw&list=PLE28375D4AC946CC3&index=1&t=4s)

```cpp
// const
// - A compile time constraint that an object can not be modified

int main()
{
    const int i = 9;
    i = 6;      // error

    const int *p1 = &i;     // data is const, pointer is not
    p1 = 5;     // error
    p1++;       // ok

    int* const p2;          // pointer is const, data is not
    const int* const p3;    // data and pointer are both const

    /*
        if const is on the left of *, data is const
        if const is on the righ of *, pointer is const
    */

    const_cast<int&>(i) = 6;    // ok
}
```

## const function

* [참고](https://www.youtube.com/watch?v=RC7uE_wl1Uc&list=PLE28375D4AC946CC3&index=2)

```cpp
// const used with functions
class Dog {
    int age;
    string name;
public:
    Dog() { age = 3;, name = "dummy"; }
    void setAge(int& a) { age = a; a++; /* 실수로 a를 수정해 버린다면?? */ }
};

int main() {
    Dog d;

    int i = 9;
    d.setAge(i);
    cout << i << endl;      // i값 자체가 변경되어 버린다.
}

// const를 쓰자
```

```cpp
// ...

    void setAge(const int& a) { age = a; }  // 내부에서 수정이 불가능하게 const로 막는다

    // 참고로 아래와 같은 코드는 효율적이지 못함. 
    // 어차피 value로 값이 들어오는데 굳이 const를 써야할까?
    void setAge(const int a) { age = a; }

    const string& getName() { return name; }    // 이런코드는 좋다고 할 수 있다.

// ...
```

```cpp
// const function
void printDogName() const { cout << name << endl; age++; /* 함수내부에서 수정이 불가능 */ }
```

```cpp
void printDogName() const { cout << name << endl; }
void printDogName() { cout << name << endl; }
// 둘 중 어느게 호출될까??

Dog d;          // void printDogName() 호출
const Dog d2;   // void printDogName() const 호출
```