---
title: "(C++) 람다식(lambda expression)"
permalink: cpp/lambda/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-02-05 00:00:00 -0000
last_modified_at: 2020-09-21 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
category:
  - lambda
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

## lambda expression & type

```cpp
#include <iostream>
#include <algorithm>
using namespace std;

int main()
{
    // lambda expression 활용 1. 함수 인자로 사용
    sort(x, x+10, [](int a, int b){ return a < b; });

    // 활용 2. auto변수에 담아서 함수처럼 사용.
    auto f = [](int a, int b){ return a + b; };
    cout << f(1,2) << endl;
}
```

```cpp
int main()
{
    auto f1 = [](int a, int b) { return a + b; };
    cout << type(f1).name() << endl; // 컴파일러마다 type이 다르다.
    auto f2 = [](int a, int b) { return a + b; };
    cout << type(f2).name() << endl; // 호출될 때마다 type이 달라진다.

    f2 = [](int a, int b) { return a + b; };    // 이런 호출이 불가능하다.
}
```

---

## lambda & function argument

```cpp
int main()
{
    // lambda는 세 가지 방법으로 담을 수 있다.
    auto f1 = [](int a, int b) { return a + b; }
    int(*f2)(int, int) = [](int a, int b) { return a + b; }
    function<int(int, int)> f3 = [](int a, int b) { return a + b; }

    f1(1, 2);       // inline 치환 가능
    f2(1, 2);       // 함수포인터이기에 언제든 바뀔 수 있다. -> inline 치환 불가능
    f3(1, 2);       // 변수이기에 역시 언제든 바뀔 수 있다. -> inline 치환 불가능
}
```

```cpp
void foo()
{
    f(1, 2);
}

int main()
{
    // 함수에서 람다표현식을 받는 방법???
    foo( [](int a, int b) { return a + b; });
}
```

```cpp
void foo( int (*f)(int, int) )      // 함수포인터로 받을 수 있으나 inline 치환 불가능!
{
    f(1, 2);
}
```

```cpp
void foo(function<int(int, int)> f)     // 역시 인라인 치환 불가능
{
    f(1, 2);
}
```

```cpp
void foo(auto f)        // auto는 함수의 인자가 될 수 없다.
{
    f(1, 2);
}
```

```cpp
template<typename T> void foo(T f)
// template<typename T> void foo(T& f) // 이건 불가능 임시객체를 lvalue로 받을 수 없다.
// template<typename T> void foo(const T& f) // 이게 제일 좋은 코드
{
    f(1, 2);
}

int main()
{
    foo( [](int a, int b) { return a + b; });
    foo( [](int a, int b) { return a - b; });
}
```

---

## lambda & suffix return type

```cpp
int main()
{
    auto f1 = [](int a, int b) { return a + b; }        // 리턴타입을 표기하지 않았지만 컴파일러가 알아서 판단

    auto f2 = [](int a, int b) -> int { return a + b; }
    // 직접 표기도 가능하다.

    auto f3 = [](int a, double b) { 
        if (a == 1) 
            return a + b;
        else
            return b;
    };      // compile error -> 컴파일러가 리턴타입을 모른다.

    auto f4 = [](int a, double b) -> double { 
        if (a == 1) 
            return a + b;
        else
            return b;
    };      
}
```

---

## Capture Variable

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

---

## Conversion

```cpp
int main()
{
    int(*f)(int, int) = [](int a, int b)
    {
        return a + b;
        // return type은 어떻게 구현했을까?
    };
}
```

```cpp
class ClosureType
{
public:
    int operator()(int a, int b) const
    {
        return a + b;
    }
    static int method(int a, int b)
    {
        return a + b;
    }

    typedef int(*F)(int, int);

    operator F()
    {
        // return &ClosureType::operator();
        // 멤버 함수포인터를 일반함수 포인터에 넣을 수 없기에 불가능

        return &ClosureType::method;
    }
}
```

```cpp
// 단, 변수를 캡처한 람다표현식은 일반함수 포인터로 캐스팅이 불가능하다.
int main()
{
    int v = 0;
    int(*f)(int, int) = [v](int a, int b)   // error
    {
        return a + b;
    };
}
```

```cpp
class ClosureType
{
    int v;
public:
    ClosureType(int a) : v(a) {}
    int operator()(int a, int b) const
    {
        return a + b + v;
    }
    static int method(int a, int b)
    {
        return a + b + v; // error
        // static 함수는 멤버변수에 접근이 불가
    }

    typedef int(*F)(int, int);

    operator F()
    {
        return &ClosureType::method;
    }
}
```

---

## MISC

```cpp
#include <iostream>
using namespace std;

int main()
{
    // generic lambda : c++14
    auto f1 = [](auto a, auto b){ return a + b; }

    // nullary lambda : 괄호를 빼도 됨
    auto f2 = [] { return 10; }

    // c++17 : ()함수를 constexpr함수로
    auto f3 = [](int a, int b) constexpr
    {
        return a + b;
    };

    int y[f3(1,2)];
}
```

```cpp
int main()
{
    auto f1 = [](int a, int b) { return a + b; }

    decltype(f1) f2;        // error - 디폴트 생성자가 람다에 없기에 에러
    decltype(f1) f3 = f1;   // ok - 복사생성자는 람다에 있음 ok
    decltype(f1) f4 = move(f1); // ok - move생성자도 있음.
}
```

---

## advanced lambda

```cpp
#include <iostream>
using namespace std;

int main()
{
    auto f = [](int a, int b) { return a + b; }

    // 아래와 같이 여러개의 변수를 받을 수 있는 람다표현식을 만들어보자.
    f(1, 2);
    f(1, 2, 3);
    f(1, 2, 3, 4);
}
```

```cpp
template<typename ... Types> class mi : public Types ...
{
public:
    mi(Types&& ... args) : Types(args)... {} 
    // 람다는 기본생성자는 delete되어있기에
    // 복사 생성자를 유도

    // 기반 클래스의 특정함수를 사용할수 있게 유도
    using Types::operator()...;
};

int main()
{
    mi f([](int a, int b) { return a+b; },
        [](int a, int b, int c) {return a+b+c; });
    // 여러 변수를 받을 수 있는 람다표현식을 생성.

    cout << f(1, 2) << endl;
    cout << f(1, 2, 3) << endl;
}
```

---

## 추가

```cpp
int main() {
    [](){ cout << "Hello" << endl; };   // 함수만 지정했지 호출은 하지 않음.

    auto func = [](){ cout << "Hello" << endl; };   // 이름없는 함수(람다)에 변수만 지정해 주었다
    func(); // Hello

    [](){ cout << "Hello" << endl; }(); // 물론 이런식으로 호출이 가능하다
}
```

```cpp
void test(void (*pFunc)()) {
    pFunc();
}

int main() {
    auto func = [](){ cout << "Hello2" << endl; };
    test(func);
}
```

---

## Lambda Parameters and Return Types

```cpp
auto pGreet = [](string name){ cout << "Hello" << name << endl; };

pGreet("Mike");     // Hello Mike
```

```cpp
void testGreet(void (*greet)(string)) {
    greet("Bob");
}

auto pGreet = [](string name){ cout << "Hello" << name << endl; };

testGreet(pGreet);  // Hello Mike
```

```cpp
auto pDivide = [](double a, double b){
    if(b == 0.0) return 0;      // int로 return하기에 error!
    return a / b;
};

cout << pDivide(10.0, 5.0) << endl;
```

```cpp
auto pDivide = [](double a, double b) -> double{
    if(b == 0.0) return 0;      // okay
    return a / b;
};

cout << pDivide(10.0, 5.0) << endl;
```

```cpp
void runDivide(double (*divide)(double a, double b)) {
    auto rval = divide(9, 3);
    cout << rval << endl;
}
```

---

## Lambda Capture Expressions

```cpp
int one = 1;
int two = 2;
int three = 3;

// 지역변수를 쓰고 싶다면?
[](){ cout << one << endl; }();

[one, two](){ cout << one << two << endl; }();

// 더 많은 지역변수를 쓰고싶다면?
[=](){ cout << one << two << endl; }();
// Capture all local variables by value

// Capture by reference
[=, &three](){ cout << one << two << endl; }();

[&](){ cout << one << two << endl; }();
```

---

## Capturing this With Lambdas

```cpp
class Test {
private:
    int one{1};
    int two{2};

public:
    void run() {
        int three = 3;  // {3} 가 더 좋은 표현
        int four = 4;   // {4}

        auto pLambda = [one, three, four](){        // error! - one은 private으로 접근 불가
            cout << three << endl;
            };
        pLambda();
    }
};

int main() {
    Test test;
    test.run();

    return 0;
}
```

```cpp
class Test {
private:
    int one{1};
    int two{2};

public:
    void run() {
        int three{3};
        int four{4};

        auto pLambda = [this, three, four](){
            cout << one << endl;   // 이런식으로 접근 가능
            one = 111;  // call by reference로 접근됨.
            cout << three << endl;
            };
        pLambda();
    }
};
```

---

## The Standard Function Type

```cpp
#include <functional>
#include <vector>
#include <algorithm>

int main() {
    int size = 5;
    vector<string> vec{"one", "two", "three"};

    int count = count_if(vec.begin(), vec.end(), [](string test){
        return test.size == size;
    });

    cout << count << endl;
}
```

```cpp
bool check(string &test) {
    return test.size() == 3;
}

int count = count_if(vec.begin(), vec.end(), check);
```

```cpp
class Check {
public:
    bool operator()(string &test) {
        return test.size() == 5;
    }
} check1;

int count = count_if(vec.begin(), vec.end(), check1);   // okay
```

```cpp
// function으로 람다를 받을 수 있다
void run(function<bool(string&)> check) {
    string test = "dog";
    cout << check(test) << endl;
}

auto lambda = [size](string test) {return test.size() == size;};

run(lambda);        // okay
run(check1);        // okay
run(check);        // okay
```

```cpp
function<int(int, int)> add = [](int one, int two){return one+two;};
cout << add(7, 3) << endl;
```

---

## Mutable Lambdas

```cpp
int cats = 5;

[cats](){
    cats = 8;       // error
    cout << cats << endl;
}();
```

```cpp
int cats = 5;

[cats]() mutable {
    cats = 8;       // okay
    cout << cats << endl;
}();
```