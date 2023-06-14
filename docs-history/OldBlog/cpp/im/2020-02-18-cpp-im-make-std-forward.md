---
title: "(C++) std::forward 구현해보기"
date: 2020-02-18 00:00:00 -0000
---

```cpp
// std::forward를 직접구현해 보자.
void goo(int& a) { cout << "goo" << endl; }
void hoo(int&& a) { cout << "hoo" << endl; }

template<typename F, typename T>
void chronometry(F f, T&& arg)
{
  f( std::forward<T>(arg) );
}

int main()
{
  int n = 0;
  chronometry(&goo, n);
  chronometry(&hoo, 1);
  cout << n << endl;
}
```

## std::forward

* lavalue를 인자로 받아서 T의 타입에 따라 lvalue 또는 rvalue로 캐스팅 한다.

---

```cpp
// 구현 시작

template<typename T> T&& xforward(T& arg)
{
  return static_cast<T&&>(arg);
}

template<typename F, typename T>
void chronometry(F f, T&& arg)
{
  f( xforward<T>(arg) );
}
```

```cpp
// move, forward 차이점

// static_cast<T&&>(arg);   T의 타입에 따라 r, l value 캐스팅

// 함수 인자 : l, r value 모두 받음
// 리턴 타입 : r value로 캐스팅
template<typename T>
typename remove_reference<T>::type&&
xmove(T&& obj)
{
  return static_cast<typename remove_reference<T>::type<T&&>(obj);
}

// 함수 인자 : l value 받음
// 리턴 타입 : T에 따라서 l, r value 캐스팅
template<typename T> T&& xforward(T& arg)
{
  return static_cast<T&&>(arg);
}

// 무조건 리턴이 rvalue여야 한다 -> move
// 상황에 따라서 유동적 -> forward
```

---

```cpp
void foo(int& a) { cout << "int&" << endl; }
void foo(int&& a) { cout << "int&&" << endl; }

class Test
{
  int data;
public:
  int& get() & { return data; } // lvalue 객체가 호출
  int get() && { return data; } // rvalue 객체가 호출
};

int main()
{
  Test t;
  foo(t.get());       // foo(int&) => foo(int&)
  foo(Test().get());  // foo(int) => foo(int&&)
}

template<typename T> void wrapper(T&& obj)
{
  foo(obj.get());
}
```

```cpp
int main()
{
  Test t;
  wrapper(t);         // lvalue -> foo(int&) -> okay
  wrapper(Test());    // rvalue -> foo(int&&) -> Nope! -> wrapper내부에 들어가면 이름이 생기기에 lvalue가 된다.
}
```

```cpp
template<typename T> void wrapper(T&& obj)
{
  foo( xforward<T>(obj).get() );  // 이렇게 수정하면 위 결과가 정상적으로 나올까?
  // Nope! -> get()까지 wrapping시켜야함!
  
  using Type = decltype( xforward<T>(obj).get() );
  foo( xforward<Type>(xforward<T>(obj).get()));   // ok!
}
```

```cpp
// l, r value 버전 모두 있어야한다.
template<typename T> T&& xforward(T& arg)   // l value ver.
{
  return static_cast<T&&>(arg);
}

template<typename T> 
T&& xforward(typename remove_reference<T>::type&& arg)    // r value ver.
{
  return static_cast<T&&>(arg);
}
```