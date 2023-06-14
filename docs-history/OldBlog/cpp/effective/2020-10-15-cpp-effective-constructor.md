---
title: "(Effective C++) 컴파일러에 의해 생성되는 생성자"
permalink: cpp/effective/constructor/                # link 직접 지정
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
  - constructor
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

* [참고사이트](https://www.youtube.com/watch?v=294SuLnsj6s&list=PLtb6y_I5vgIT7IPpaUp4r2U0viXw2iFeX&index=5)

---

```cpp
class CEmpty
{
    //아래의 생성자를 컴파일러가 만들어준다.
public:
    CEmpty();       // 기본 생성자
    ~CEmpty();      // 소멸자
    CEmpty(CEmpty const& rhs) {}                            // 복사 생성자
    CEmpty& operator=(CEmpty const& rhs) { return *this; }  // 복사 대입 연산자
};
```

큰 내용은 없음... 컴파일러가 제공해주는 기본 생성자를 사용하면서 얕은 복사를 사용하게되는데 그에 따른 문제에 대해 주의하자.