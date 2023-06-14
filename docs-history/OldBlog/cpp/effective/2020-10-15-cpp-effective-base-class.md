---
title: "(Effective C++) 기본 클래스와 가상함수의 정리"
permalink: cpp/effective/base-class/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-10-15 00:00:00 -0000
last_modified_at: 2020-10-15 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
category:
  - 클래스
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

* [참고사이트](https://www.youtube.com/watch?v=Yx0L_ru9-nY&list=PLtb6y_I5vgIT7IPpaUp4r2U0viXw2iFeX&index=7)

---

```cpp
/*
class A
class B : public A

B kObj;     // 부모 클래스 소멸자에 virtual이 없어도 부모/자식 소멸자 모두 호출됨
A* pkBase = new B;  // 부모 클래스에 소멸자가 없으면, 자녀클래스 소멸자도 실행되지 않음.
delete pkBase;
*/

class NoneVtbl
{
public:
    NoneVtbl(){};
    ~NoneVtbl(){};
private:
    int m_iData;
};

class ExistVtbl
{
public:
    ExistVtbl()
    {
        std::cout << "ExistVtbl 생성자 실행" << std::endl;
    }

    virtual ~ExistVtbl()
    {
        std::cout << "ExistVtbl 소멸자 실행" << std::endl;
    }

private:
    int m_iData;
};

class ExistVblChild : public ExistVtbl
{
public:
    ExistVblChild()
    {
        std::cout << "ExistVblChild 생성자 실행" << std::endl;
    }

    ~ExistVblChild()
    {
        std::cout << "ExistVblChild 소멸자 실행" << std::endl;
    }
};
```

```cpp
std::cout << sizeof(NoneVtbl) << std::endl;     // 4 bytes
std::cout << sizeof(ExistVtbl) << std::endl;     // 8 bytes
// ???? 이게 뭔 일이지?? 
// NoneVtbl : int형 
// ExistVtbl : int형 + virtual - virtual도 4bytes를 먹는다

// 물론 32비트 기준이고 64비트면 8바이트 / 16바이트 먹음
```

```cpp
// virtual쓸때 주의사항
ExistVtbl* kTemp = new ExistVblChild;
::memset(&kTemp, 0, sizeof(kTemp)); // virtual이 날아가버림!!
delete kTemp;
```