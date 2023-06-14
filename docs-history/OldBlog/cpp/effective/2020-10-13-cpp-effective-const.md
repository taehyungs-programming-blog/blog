---
title: "(Effective C++) const"
permalink: cpp/effective/const/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-10-13 00:00:00 -0000
last_modified_at: 2020-10-13 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
category:
  - const
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

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