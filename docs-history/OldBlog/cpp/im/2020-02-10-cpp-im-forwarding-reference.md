---
title: "(C++) forwarding reference"
date: 2020-02-10 00:00:00 -0000
---

```cpp
void f1(int& a){}     // lvalue만 인자로 전달 가능
void f2(int&& a) {}   // rvalue만 인자로 전달 가능 : f2(10) ok

template<typename T> void f3(T& a) {}   // T:int& 라면 -> 오잉?

int main()
{
  int n = 10;
  
  // T의 타입을 사용자가 지정할 경우
  f3<int>();    // f3(int&) -> lvalue ok
  f3<int&>();   // f3(int& &) -> f3(int&) -> lvalue전달 가능
  f3<int&&>();  // f3(int&& &) -> f3(int&) -> lvalue전달 가능.
  // 결국 lvalue만 받는다.
  
  // 사용자가 T타입을 지정하지 않은 경우
  f3(10);   // error - 컴파일러가 위에서 보듯 뭘로 변경해도 rlavue를 받을 수 없다.
  f3(n);    // ok
}
```

```cpp
// 모든 타입의 lavlue만 전달 가능
template<typename T> void f3(T& a) {}
```

```cpp
template<typename T> void f4(T&& a) {}

int main()
{
  int n = 10;
  
  // 사용자가 T의 타입을 명시적으로 전달
  f4<int>();    // f4(int&&) -> rvalue 전달
  f4<int&>();   // f4(int& &&) -> lvalue 전달
  f4<int&&>();  // f4(int&& &&) -> rvalue 전달
  
  // T의 타입에 따라서 l, rvalue가 달라지게 된다.
  
  // 사용자가 T타입을 지정하지 않은 경우
  f4(n);    // T -> int&로 변환
  f4(10);   // ok, 컴파일러가 T를 int로 결정 -> f4(T&&) -> f4(int&&)
}
```

```cpp
// T&& : forwarding reference라 한다.
// l, r value를 모두 전달 가능
// lvalue를 전달하면 T 는 lvalue reference로 결정
// rvalue 전달하면 T는 값 타입으로 결정
```

```cpp
// 정리
void f1(int& a) {}
void f2(int&& a) {}
template<typename T> void f3(T& a) {}
template<typename T> void f4(T&& a) {}

// int& : int형의 lvalue 전달 가능
// int&& : int형의 rvalue 전달 가능
// T& : 모든 타입의 lvalue전달 가능
// T&& : 모든 타입의 l, rvalue 전달 가능. -> forwarding reference
// lvalue를 전달하면(foo(n)) T : int& -> T&& : int& && -> int&
// rvalue를 전달하면(foo(10)) T : int&& -> T&& : int&&
```

```cpp
// forwarding reference 주의사항
template<typename T> void foo(T&& a) {}

template<typename T> class Test
{
public:
  void goo(T&& a) {}    // 마치 forwarding reference같다
  
  template<typename U> void hoo(U&& a)  {}    // 이게 forwarding reference.
};

int main()
{
  int n = 10;
  
  foo(n);   // ok
  foo(10);  // ok
  
  Test<int> t1;   // T가 int로 결정 -> void goo(int&& a)가 된다.
  t1.goo(n);    // error
  t1.goo(10);   // ok
  
  Test<int&> t2;    // T가 int&로 결정 -> void goo(int& &&) -> void goo(int&)
  t2.goo(n);    // ok
  t2.goo(10);   // error
}
```