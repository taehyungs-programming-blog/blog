---
title: "(C++) friend 함수"
#permalink: cpp/basic/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-01-07 00:00:00 -0000
last_modified_at: 2020-03-16 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

멤버 함수는 아니지만 private 멤버에 접근 할 수 있게 하고 싶을 때 사용<br>
특정 함수, 클래스에서 접근이 가능하게 하고 싶을때.<br>

```cpp
class Airplane
{
  int color;
  int speed;
  int engineTemp;
  
public:
  int getSpeed() { return speed; }
  // void fixAirplane(Airplane& a);
  friend void fixAirplane(Airplane& a);   // 내부 멤버에 접근가능!
};

void fixAirplane(Airplane& a)
{
  int n = a.engineTemp;   // engineTemp 접근 불가
}
```