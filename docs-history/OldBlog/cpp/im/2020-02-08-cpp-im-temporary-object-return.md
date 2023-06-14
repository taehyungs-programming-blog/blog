---
title: "(C++) 참조리턴 vs 값리턴"
date: 2020-02-08 00:00:00 -0000
---


```cpp
Point p;        // 전역변수

Point foo()
{
    return p;
}

Point& goo()        // 참조리턴은 임시객체가 생성되지 않음
{
    return p;
}

int main()
{
    foo().x = 10;       // 가능한가? -> error
    goo().x = 20;       // ok

    cout << p.x << endl;
}
```

* 아무리 전역의 `Point p`를 리턴하더라도 값리턴은 임시객체가 리턴됨을 기억하자.

```cpp
int x;

int foo() {return x;}
int& goo() {return x;}

int main()
{
    foo() = 10;     // error
    goo() = 10;     // ok.
}
```