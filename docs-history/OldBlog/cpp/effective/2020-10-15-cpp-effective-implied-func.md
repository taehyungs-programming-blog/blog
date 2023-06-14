---
title: "(Effective C++) 묵시적 함수 사용을 막아보자"
permalink: cpp/effective/implied-func/                # link 직접 지정
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
  - 묵시적 함수
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

* [참고사이트](https://www.youtube.com/watch?v=n83lC8qTZtk&list=PLtb6y_I5vgIT7IPpaUp4r2U0viXw2iFeX&index=6)

---

```cpp
class CNoneCopy
{
public:
    CNoneCopy() {};

// 방법1 . 접근권한을 막아버린다.
private:
    CNoneCopy(CNoneCopy const&);
    CNoneCopy& operator=(CNoneCopy const&);
};
```

```cpp
// 방법2 . 방법1을 조금 확장 상속으로 접근권한을 막는다
class CUncopyable
{
protected:
    CUncopyable() {};
    virtual ~CUncopyable() {};

private:
    CUncopyable(const CUncopyable&);
    CUncopyable& operator=(const CUncopyable&);
};

class CTest : private CUncopyable
{
    // ...
    // 자녀클래스에서는 복사, 복사 대입 선언을 적지 않아도 됨.
}
```

effective c++이 옛날에 만들어져서 이런 방법을 쓰는듯 ;; delete써버리면 된다.