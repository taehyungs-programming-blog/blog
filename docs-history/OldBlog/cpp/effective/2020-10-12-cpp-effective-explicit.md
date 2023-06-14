---
title: "(Effective C++) explicit"
permalink: cpp/effective/explicit/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-10-12 00:00:00 -0000
last_modified_at: 2020-10-12 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
category:
  - explicit
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

* [참고사이트](https://www.youtube.com/watch?v=MDe3gcykmww&list=PLtb6y_I5vgIT7IPpaUp4r2U0viXw2iFeX)

---

## explicit

* explicit : 묵시적 변환을 막는다. 

```cpp
#include "stdafx.h"

class CBase
{
public:
    CBase(){}
};

// CNoneExplicit가 생성되어 버리면 CBase까지 생성되며 자원의 낭비가 일어남을 보여주기 위함.
class CNoneExplicit : CBase
{
public:
    CNoneExplicit(int iData)
    {
        std::cout << "NoneExplicit의 생성자 입니다." << std::endl;
    }

    CNoneExplicit(CNoneExplicit const& rhs)
    {
        std::cout << "NoneExplicit의 복사 생성자 입니다." << std::endl;
    }

    CNoneExplicit& operator=(CNoneExplicit const& rhs)
    {
        std::cout << "NoneExplicit의 대입 연산자 입니다." << std::endl;
        return *this;
    }

    void test() const
    {
        std::cout << "test" << std::endl;
    }
};

class CExplicit
{
public:
    explicit CExplicit(int iData)
    {
        std::cout << "Explicit 생성자 입니다." << std::endl;
    }

    explicit CExplicit(CExplicit const& rhs)
    {
        std::cout << "Explicit 복사 생성자 입니다." << std::endl;
    }

    CExplicit& operator=(CExplicit const& rhs)
    {
        std::cout << "Explicit 대입 연산자 입니다." << std::endl;
        return *this;
    }
};

void explictFunc(CExplicit const& kExplct)
{

}

void noneExpctFunc(CNoneExplicit const& kNoneExplct)
{
    kNoneExplct.test();
}

int main()
{
    CNoneExplicit kNoneExplct = 10;     // 생성자
    CNoneExplicit kTemp = kNoneExplct;  // 복사생성자
    kTemp = kNoneExplct;                // 복사대입연산자
    noneExpctFunc(10);                  // 클래스 참조자에 임시 객체를 넣어도 에러가 안남.

    std::cout<<std::endl<<std::endl;

    CExplicit kExplct(10);              // 명시적 호출
    // CExplicit kExplct = 10;          // 생성자 error
    CExplicit kTempCopy(kExplct);
    // CExplicit kTempCopy = kExplct;   // 복사생성자 error
    kTempCopy = kExplct;                // 복사대입연산자
    // explctFunc(10);                  // error
}
```