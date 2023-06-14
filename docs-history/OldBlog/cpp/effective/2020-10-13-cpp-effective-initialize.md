---
title: "(Effective C++) 객체의 초기화"
permalink: cpp/effective/initialize/                # link 직접 지정
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
  - 객체의 초기화
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

* [참고사이트](https://www.youtube.com/watch?v=6lQt4UaOXII&list=PLtb6y_I5vgIT7IPpaUp4r2U0viXw2iFeX&index=4)

---

* 결국 하고싶은 말은 객체를 사용하기 전에 객체를 초기화 하자!

```cpp
class CInit 
{
private:
    int m_iVal;
    std::string m_sName;
    int const m_ciVal;
    std::string& m_csName;

public:
    CInit(int _iVal, std::string _sName) 
                                    : m_iVal(_iVal),
                                    m_sName(),
                                    m_ciVal(_iVal),
                                    m_csName(_sName)
    // 상수와 래퍼런스는 의무적으로 초기화 리스트에 넣어주자
    // 대입자체가 불가능 하기 때문이다.
    {
        // 여기서 하는 초기화는 초기화라 할 수 없고 대입이라 할 수 있다.
    }
}
```

```cpp
class CActionFirst
{
public:
    void output(void)
    {
        printf("CActionFirst::output\n");
    }
};

// 다른 파일
extern CActionFirst ActionFirst;
class CActionSecond
{
public:
    CActionSecond()
    {
        ActionFirst.output();
    }
};

// 만약 CActionFirst가 먼저 생성되지 않고 CActionSecond가 생성이 되어버리면 문제가 발생(정의 되지 않은 객체에 접근을 시도)

// 어떻게 해결하지??
```

```cpp
class CActionFirst
{
public:
    void output(void)
    {
        printf("CActionFirst::output\n");
    }
};
// 함수내부에서 객체를 생성후 리턴
CActionFirst& actionFirst(void)
{
    static CActionFirst instance;
    return instance;
}

// 다른 파일
class CActionSecond
{
public:
    CActionSecond()
    {
        actionFirst().output();
    }
};
CActionFirst ActionFirst;
```