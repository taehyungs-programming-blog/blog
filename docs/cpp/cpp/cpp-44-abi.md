---
layout: default
title: "44. ABI(Application Binary Interface)"
parent: "(C++) 문법"
grand_parent: (C++)
nav_order: 7
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* **ABI(Application Binary Interface)**는 애플리케이션 바이너리 인터페이스의 줄임말로, 컴파일된 프로그램이 운영체제나 다른 라이브러리와 상호작용하는 방식을 정의하는 저수준 인터페이스

---

## 호출 규약(Calling Convention)

* 함수 호출 시 매개변수를 어떻게 전달할지 정의
* 반환값을 어떻게 처리할지 결정
* 스택 정리 책임 (호출자 vs 피호출자)

```cpp
// 예시: Windows에서의 호출 규약
__stdcall int function(int a, int b);  // 매개변수를 오른쪽부터 스택에 푸시
__cdecl int function(int a, int b);    // C 표준 호출 규약
__fastcall int function(int a, int b); // 레지스터 사용 우선
```

## 데이터 타입 크기와 정렬

* 기본 데이터 타입의 크기 (int, long, pointer 등)
* 메모리 정렬 규칙
* 구조체 패딩

```cpp
// 플랫폼별 차이 예시
struct Example {
    char a;     // 1바이트
    int b;      // 4바이트, 하지만 정렬을 위해 패딩 추가
    double c;   // 8바이트
};
// 32비트: 크기가 16바이트일 수 있음
// 64비트: 크기가 24바이트일 수 있음
```

## 레지스터 사용 규칙

* 어떤 레지스터를 매개변수 전달에 사용할지
* 어떤 레지스터를 보존해야 하는지
* 반환값을 어떤 레지스터에 저장할지

## 스택 레이아웃

* 스택 프레임 구조
* 로컬 변수 배치
* 스택 포인터 정렬

## 예외 처리

* 예외 전파 방식
* 스택 언와인딩 메커니즘
* 예외 정보 저장 방식

## C++에서의 추가 고려사항

```cpp
// C++ 함수
void function(int x, double y);
// 컴파일 후 맹글링된 이름 (GCC 예시)
// _Z8functionid
```

```cpp
class Base {
public:
    virtual void func1() = 0;
    virtual void func2() = 0;
};
// vtable 레이아웃이 ABI에 영향
```

### vtable이란?

* vtable은 C++에서 가상 함수(virtual function)의 동적 디스패치(dynamic dispatch)를 구현하기 위한 메커니즘
    * 동적 디스패치(dynamic dispatch)

```cpp
class Animal {
public:
    virtual void speak() { cout << "Animal sound"; }
    virtual void move() { cout << "Animal moves"; }
    virtual ~Animal() = default;
    
    void nonVirtualFunc() { cout << "Non-virtual"; } // vtable에 없음
};

class Dog : public Animal {
public:
    void speak() override { cout << "Woof!"; }
    void move() override { cout << "Dog runs"; }
};

class Cat : public Animal {
public:
    void speak() override { cout << "Meow!"; }
    // move()는 오버라이드하지 않음 - 부모의 함수 사용
};
```

```
Animal 객체:
+---------------+
| vtable 포인터 | -> Animal vtable
+---------------+
| 멤버 변수들   |
+---------------+

Animal vtable:
+---------------+
| &Animal::speak|
| &Animal::move |
| &Animal::~Animal |
+---------------+

Dog 객체:
+---------------+
| vtable 포인터 | -> Dog vtable  
+---------------+
| 멤버 변수들   |
+---------------+

Dog vtable:
+---------------+
| &Dog::speak   |
| &Dog::move    |
| &Animal::~Animal |
+---------------+
```

```cpp
Animal* animal = new Dog();
animal->speak();  // 컴파일 시점에는 어떤 함수인지 모름

// 실제 실행 과정:
// 1. animal 객체의 vtable 포인터를 읽음
// 2. vtable에서 speak() 함수의 주소를 찾음 (보통 인덱스 0)
// 3. 해당 주소로 점프하여 Dog::speak() 실행
```

* 문제점?

```cpp
// GCC/Clang의 vtable 레이아웃
class Base {
    virtual void func1();
    virtual void func2(); 
};
// vtable: [func1, func2]

// MSVC의 경우 추가 정보가 포함될 수 있음
// vtable: [RTTI정보, func1, func2] 또는 다른 순서
```

```cpp
class Base1 {
    virtual void f1();
};

class Base2 {
    virtual void f2();
};

class Derived : public Base1, public Base2 {
    virtual void f1() override;
    virtual void f2() override;
    virtual void f3();
};

// 다중 상속 시 vtable 구조가 매우 복잡해짐
// 컴파일러마다 다른 레이아웃을 가질 수 있음
```

```cpp
class Base {
    virtual ~Base() = default;  // 가상 소멸자
};

// vtable에 소멸자 정보, RTTI 정보 등이 추가됨
// 이런 메타데이터의 위치와 형식이 컴파일러마다 다름
```

## C 스타일이 ABI에 안전한 이유

```cpp
// C 스타일 - ABI 안전
extern "C" {
    struct Point {
        double x, y;  // 단순한 데이터만
    };
    
    int add_numbers(int a, int b);
    void process_point(struct Point* p);
}

// C++ 스타일 - ABI 위험
class Point {
    double x, y;
public:
    virtual double distance() const;  // vtable 필요
    Point operator+(const Point& other);  // 이름 맹글링
};
```

```cpp
// C 함수 - 심볼 이름이 예측 가능
extern "C" int calculate(int x, int y);
// 심볼: calculate

// C++ 함수 - 컴파일러마다 다른 맹글링
int calculate(int x, int y);
// GCC: _Z9calculateii
// MSVC: ?calculate@@YAHHH@Z
```

```cpp
// C 구조체 - 예측 가능한 레이아웃
struct Data {
    int value;
    char flag;
    // 패딩은 있을 수 있지만 표준화됨
};

// C++ 클래스 - 복잡한 레이아웃
class Data {
    int value;
    char flag;
    virtual void method();  // vtable 포인터 추가
    static int count;       // 메모리 레이아웃에 영향 없음
};
```

```cpp
// C 함수 - 플랫폼별로 표준화된 호출 규약
extern "C" {
    int process_data(int* data, size_t count);
}

// C++ 멤버 함수 - 숨겨진 this 포인터
class Processor {
public:
    int process_data(int* data, size_t count);
    // 실제로는 process_data(this, data, count) 형태
};
```

```cpp
// C 스타일 - 오류 코드로 처리
extern "C" {
    int file_read(const char* filename, char* buffer, int* error_code);
}

// C++ 스타일 - 예외 처리로 인한 ABI 복잡성
class FileReader {
public:
    std::string read(const std::string& filename);  // 예외 던질 수 있음
};
```

