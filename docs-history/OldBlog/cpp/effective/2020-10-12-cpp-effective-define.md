---
title: "(Effective C++) define"
permalink: cpp/effective/define/                # link 직접 지정
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
  - define
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

* [참고사이트](https://www.youtube.com/watch?v=e3tCxGNBewM&list=PLtb6y_I5vgIT7IPpaUp4r2U0viXw2iFeX&index=2)

---

* define 대신 const, enum, inline을 쓰자
* define은 전처리기에서 실행되기 때문에 컴파일러가 에러 체크를 못한다.

```cpp
#define TEST_MSG "Yiku"
char const * const STR_TEST_MSG1 = "Yiku";      // 포인터의 완벽한 상수화 데이터 상수 and 포인터 상수
std::string const STR_TEST_MSG2("Yiku");        // 더 멋진 표현법이다.
```

* const에 대해 좀 더 설명하자면 ...

```cpp
int       iVal1 = 0;        // 비상수
int const iVal2 = 0;        // 상수
int const& riVal = 0;       // 상수
int const* piVal1 = 0;      // 포인터 비상수, 데이터 상수
int* const piVal2 = 0;      // 포인터 상수, 데이터 비상수
int const* const piVal3 = 0;// 포인터 상수, 데이터 상수

// 그냥 외어버리자
// * 를 기준으로 좌측에 const가 있으면 데이터 상수
// 우측에 const가 있으면 포인터 상수
```

```cpp
class AvailConstPointer_InitSuccess
{
private:
    static int const MAX_INDEX = 5;     // 좋지 못한 상수 선언
    // Why?
    // 구 컴파일러는 에러가 발생 될 수 있다.
    // 선언부분에서 값을 초기화 하는 것은 일관성에 문제가 있다.

    int m_iArr[MAX_INDEX];      // ok
};

class AvailConstPointer_InitFail
{
private:
    static int const MAX_INDEX;     // 상수 선언
    int m_iArr[MAX_INDEX];      // fail! : MAX_INDEX를 모른다
};

int const AvailConstPointer_InitFail::MAX_INDEX = 10;   // 상수 정의(cpp에 존재해야함.)
```

어찌해야하지??

```cpp
// enum의 활용
class AvilEnum
{
public:
    enum { MAX_INDEX = 5, };

private:
    int m_iArr[MAX_INDEX];
};
```

* #define의 문제에 대해 좀 더 살펴보자

```cpp
#define VALUE_COMPARE(a, b) ((a) > (b) ? (a) : (b))

int iNum1 = 5;
int iNum2 = 0;
std::cout << "원래 값 " << iNum1 << std::endl;

VALUE_COMPARE(++iNum1, iNum2);
std::cout << "값이 두 번 증가 " << iNum1 << std::endl;
// ((++iNum1) > (iNum2) ? (++iNum1) : (iNum2))

VALUE_COMPARE(++iNum1, iNum2+10);
std::cout << "값이 한 번 증가 " << iNum1 << std::endl;
// ???? 엥 결과가 다르네??
```

문제는 그렇다 치고 해결은 ???

```cpp
// define의 inline화
inline void VALUECOMPARE(int const& iLhs, int const& iRhs)
{
    ((iLhs) > (iRhs) ? (iLhs) : (iRhs));
}
```