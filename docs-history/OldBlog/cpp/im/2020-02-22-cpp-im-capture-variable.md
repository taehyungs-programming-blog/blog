---
title: "(C++) Capture Variable"
date: 2020-02-22 00:00:00 -0000
---

```cpp
#include <iostream>
using namespace std;

int g = 10;

int main()
{
    int v1 = 0, v2 = 0;
    auto f1 = [](int a) { return a + g; }   // 람다표현식에서 전역변수에 접근이 가능? -> Okay, 읽고, 쓰고 모두 가능

    auto f2 = [](int a) { return a + v1; }      // 지역변수 접근에는 에러가 난다.

    auto f3 = [v1](int a) { return a + v1; }    // 지역변수 캡처를 지정해 줘야한다.

    auto f4 = [v1](int a) mutable { v1 = 10; }  // mutable을 쓰면 값 변경도 가능!
}
```

```cpp
// 원리에 대해 좀 더 설명한다.
auto f0 = []() { return 0; }
auto f1 = [v1, v2]() { return v1 + v2; }

cout << sizeof(f0) << endl; // 1
cout << sizeof(f1) << endl; // 내부에 변수 v1, v2를 갖기에 - 8
```

```cpp
// 람다가 호출되면 아래와 같은 임시객체가 생성된다.
class ClosureType
{
    int v1;
    int v2;
public:
    ClosureType(int a, int b) : v1(a), v2(b) {}

    int operator()() const
    {
        return v1 + v2;
    }
};
auto f1 = ClosureType(v1, v2);
```

---

```cpp
// 지역변수 캡처시 값변경은?
auto f1 = [v1, v2]() {v1 = 10; v2 = 20; }   // error! -> operator()() const 상수이기에 변경 불가!

// mutable lambda : ()연산자 함수를 비상수 함수로
auto f1 = [v1, v2]() mutable {v1 = 10; v2 = 20; }
f1();

cout << v1 << endl;     // 0
cout << v2 << endl;     // 0
// 복사본을 변경했기에 원본은 변화가 없음.
```

```cpp
// reference capture
auto f1 = [v1, v2]() mutable { v1 = 10; v2 = 20; }
auto f2 = [&v1, &v2]() { v1 = 10; v2 = 20; }
```

```cpp
class ClosureType
{
    int& v1;
    int& v2;
public:
    ClosureType(int& a, int& b) : v1(a), v2(b) {}

    int operator()() const
    {
        v1 = 10;
        v2 = 20;    // operator()()가 상수더라도 값자체를 바꾸는 것이아니라 참조형이 가리키는 곳을 변경하기에 변경가능!
    }
};
auto f1 = ClosureType(v1, v2);
```

---

## 다양한 모양을 바탕으로 정리

```cpp
int main()
{
    int v1 = 0, v2 = 0, v3 = 0;

    // capture by copy, capture by reference
    auto f1 = [v1]() {};
    auto f2 = [&v1]() {};
    auto f3 = [v1, &v2]() {};

    // default capture
    auto f4 = [=]() {};     // capture by copy
    auto f5 = [&]() {};     // capture by reference
    auto f6 = [=, &v1]() {};
    auto f7 = [&, v1]() {};
    auto f8 = [=, v1]() {};     // error

    auto f9 = [x = v1, v2 = v2, v3]() {}
    auto f10 = [v1, y = v2, &r = v3]() {}

    string s = "hello";
    auto f11 = [s1 = move(s)]() {};

    f11();
    cout << s << endl; // empty string

    unique_ptr<int> p(new int);
    auto f12 = [p = move(p)]() {};
}
```

```cpp
void foo(int a, int b)
{
    auto f = [a, b]() { return a + b; }
}
```

```cpp
template<typename .. Types> void goo(Types ... args)
{
    auto f = [=]() { auto t = make_tuple(args...); };
    auto f = [args...]() { auto t = make_tuple(args...); };
}
```

---

## 멤버 데이터의 캡처

```cpp
class Test
{
    int data = 0;
public:
    void foo()
    {
        int v = 0
        auto f = []{ data = 10; }       // error! - 멤버 데이터 접근 불가
    }
};

int main()
{
    Test t;
    t.foo();
}
```

```cpp
class Test
{
    int data = 0;
public:
    void foo()
    {
        int v = 0
        auto f = [this]{ this->data = 10; } // this를 생략해도 좋음.
    }
};
```

```cpp
class Test
{
    int data = 0;
public:
    void foo()
    {
        int v = 0
        auto f = [*this]{ this->data = 10; } // 복사본으로 캡처
        // 값을 바꿀수 없다.
    }
};
```