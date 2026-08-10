---
layout: default
title: "21. lambda"
parent: "(C++) 문법"
grand_parent: (C++)
great_grand_parent: "Legacy Archive"
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

> 너무 기초보단 몰랐던 내용위주로 정리 함.

## closure object

* **closure object** : 람다 표현식의 동작 원리에 해당한다. 람다가 작성되면 컴파일러는 closure object를 생성해 준다.

```cpp
sort(x, x+10, [](int a, int b){ return a < b; });

// 컴파일러는 아래와 같은 closure object를 생성해 준다.
    // 정확한 모양 아래와 같이 간단하진 않고 좀 더 복잡함.
class ClosureType
{
public:
    bool operator()(int a, int b) const
    {
        return a < b;
    }
};
```

---

## lambda의 type조사

```cpp
// 동일해 보이는 두 lambda 과연 type도 동일할까?
auto f1 = [](int a, int b) { return a + b; }
auto f2 = [](int a, int b) { return a + b; }

// 결론은 type은 다름
    // C++ 규정상 모든 lambda의 type은 유니크하다
cout << typeid(f1).name() << endl;
cout << typeid(f2).name() << endl;

// 따라서, 이런연산불가
f2 = [](int a, int b) { return a + b; }
```

---

## lambda는 뭘로 받을 수 있을까?

```cpp
// 세 가지 방법으로 받을 수 있음.
auto f1 = [](int a , int b) { return a + b; }
int(*f2)(int, int) = [](int a, int b) { return a + b; }
function<int(int, int)> f3 = [](int a, int b) { return a + b; }

// inline치환이 될까?
f1(1, 2);   // ok - inline치환 지원 - functor이기에 가능하다
f2(1, 2);   // nope - 함수포인터는 inline치환 미 지원(함수포인터는 가리키는 함수가 달라질수 있기에)
f3(1, 2);   // nope - 위와같은이유
```

---

## lambda의 리턴타입 지정

```cpp
// suffix return type이라 한다
auto f1 = [](int a, int b) -> int { return a + b; }
```

---

## Capture Variable

🐹 역시 너무 쉬운 내용은 생략함. 몰랐던 부분 위주로 정리

```cpp
int g = 10;

int main()
{
    int v1 = 0;

    auto f1 = [](int a) { 
        return a + g; // 내부에서 전역변수 g에 접근이 가능할까?
                      // ok - 전역은 접근이 가능, closure라는 객체내부에서 호출되기에 가능
    }

    auto f2 = [](int a) { 
        return a + v1; // 내부에서 지역변수에 접근이 가능할까?
                       // error - 지역은 접근이 불가능, closure기준에선 v1을 모른다.
    }

    // 지역변수에 접근하고 싶다면 지역변수 Capture를 사용해야한다.
    auto f3 = [v1](int a) { return a + v1; }

    auto f4 = [v1](int a) mutable { 
            v1 = 10;        // mutable을 붙이면 v1을 수정가능해지는데
                            // clousuer 객체의 operator()() const 속성을 제거해 주기때문이다.
                            // 단, v1을 수정했다 하더라도 복사본이 수정되기에 v1자체가 수정되진 않는다
            return a + v1; 
        }
}
```

```cpp
string s = "hello";
auto f11 = [s1 = move(s)]() {};

unique_ptr<int> p(new int);
// auto f12 = [p]() {} // Error - unique_ptr 캡처불가
auto f12 = [p = move(p)]() {}
```

```cpp
class Test
{
    int data = 0;
public:
    void foo()
    {
        auto f = [this]() { data = 10; }    // ok

        // C++17 부터 this의 복사본을 캡쳐가능
        auto f2 = [*this]() mutable { data = 10; } // ok - data에는 영향을 안줌
    }
}
```

---

## C++20에서의 Lambda

1. **템플릿** 사용 가능
2. **평가 되지 않은 표현식** 사용 가능
3. **디폴트 생성자, 대입 연산자** 사용 가능
4. 암시적 this 캡쳐가 deprecated 됨
5. **Parameter pack** 캡쳐 가능

### 템플릿 사용 가능

```cpp
// 람다의 매개변수로 double이 온다면?
auto add1 = [](int  a, int  b) {return a + b;};
```

```cpp
// 흠 ... a, b가 둘 다 double이면 double / int면 int 였으면 하는데?
auto add2 = [](auto a, auto b) {return a + b;};
```

```cpp
auto add3 = [](auto a, decltype(a) b) {return a + b;};
// std::cout << add3(1,   2.2) << std::endl; // 3 - 이걸 에러로 리턴하게 하고싶은데??
    // -> template의 필요성
```

```cpp
#include <iostream>

auto add1 = [](auto a, auto b) {return a + b;};         // C++14
auto add2 = []<typename T>(T a, T b) {return a + b;};   // C++20

// 이런 표현도 가능하다
auto add3 = []<typename T1, typename T2>(T1 a, T1 b, T2 c) {return a + b + c;}; // C++20

int main()
{
    std::cout << add1(1,   2)   << std::endl; // 3
    std::cout << add1(1.1, 2.2) << std::endl; // 3.3
    std::cout << add1(1,   2.2) << std::endl; // 3.2

    std::cout << add2(1,   2)   << std::endl; // 3
    std::cout << add2(1.1, 2.2) << std::endl; // 3.3
//    std::cout << add2(1,   2.2) << std::endl; // error.

// 자료형을 직접지정하겠다고 아래처럼 쓰면 에러남.
//   std::cout << add2<int>(1,   2.2) << std::endl; // error.

// 자료형을 지정하고 싶다면아래처럼 할 것
   add2.operator()<int>(1, 2.2); // ok
}
```

---

### 평가 되지 않은 표현식 사용 가능

* **평가 되지 않은 표현식(unevaluated expression)** : 실행 시간에 실행되지 않고, 컴파일 시간에만 사용되는 표현식
    * 대표적으로 아래와 같은 함수가 평가 되지 않은 표현식 이다.
    * `sizeof`
    * `decltype`
    * `typeid`
    * `nocept`

```cpp
#include <iostream>
#include <memory>

int add(int a, int b); // { return a + b;}

int main()
{
    std::cout << sizeof(int)  << std::endl;  // 4

    // sizeof(함수호출식) : 리턴 타입의 크기
        // sizeof도 평가 되지 않은 표현식으로 컴파일 타임에 결정된다.
    std::cout << sizeof( add(1, 2) ) << std::endl; // 4

    decltype( add(1,2) ) n;  // int n

    // 평가되지 않은 표현식 안에 람다를 쓸 수있을까? -> C++20부터 가능
    std::cout << sizeof( [](int a, int b) {return a + b;} ) << std::endl;  // C++17.. error. C++20 ok
                            // 1 - 리턴형의 크기

    std::cout << sizeof( [](int a, int b) {return a + b;}(1,2) ) << std::endl; // 4 - 호출 결과의 크기

    // 아래와 같은 선언이 가능한이유가
        // unique_ptr의 두 번째 인자로 삭제자의 타입을 넣어야하는데
        // 람다의 경우 타입이랄게 개발자가 넣을수 없음.
        // 람다가 평가되지 않은 표현식을 람다에 쓸수없었던 C++20이전은 아래와 같은 표현이 불가능.
    std::unique_ptr<int, decltype( [](int* p) { delete p; }) > up( new int );
}
```

---

### 디폴트 생성자, 대입 연산자 사용 가능

```cpp
#include <iostream>

int main()
{
    int v1 = 10;

    auto f1 = [v1](int a, int b) {return a + b;};

                            // C++11 ~ C++17     C++ 20
    decltype(f1) f2;        // Error               Error
    decltype(f1) f3 = f1;   // OK                  OK
    f3 = f1;                // Error               Error

/*
    auto f1 = [](int a, int b) {return a + b;};
                            // C++11 ~ C++17     C++ 20
    decltype(f1) f2;        // Error               OK
    decltype(f1) f3 = f1;   // OK                  OK
    f3 = f1;                // Error               OK
*/
   
}
```

---

### 암시적 this 캡쳐가 deprecated 됨

```cpp
#include <iostream>
#include <functional>

struct Sample
{
	int value = 0;

	auto foo()
	{
		int n = 10;

        // [=] => [=, this]
		//auto f = [=](int a) { return a + n + value; };        
        //auto f = [=, this](int a) { return a + n + value; };        
    
        auto f = [=, *this](int a) { return a + n + value; };        
        
        std::cout << sizeof(f) << std::endl; // 8
		
        return f;
	}

};
std::function<int(int)> f;

void goo()
{
	Sample s;
	f = s.foo();
    std::cout << f(10) << std::endl;
}
int main() 
{
     goo(); 
     //....
     std::cout << f(10) << std::endl;
}
```

---

### Parameter pack 캡쳐 가능

```cpp
#include <iostream>

// Capture Parameter pack by value.
template<typename ... Args> auto f1(Args&&... args)
{
	return [...args = std::forward<Args>(args)](){ (std::cout << ... << args); };
}

// Capture Parameter pack by reference.
template<typename ... Args> auto f2(Args&&... args)
{
	return[&...args = std::forward<Args>(args)](){ (std::cout << ... << args); };
}

int main()
{
	f1(1, 2, 3, 4, 5)();

	std::cout << std::endl;

	int a = 1, b = 2, c = 3;
	f2(a, b, c)();
}
```