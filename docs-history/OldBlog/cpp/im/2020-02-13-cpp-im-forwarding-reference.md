---
title: "(C++) forwarding reference"
date: 2020-02-13 00:00:00 -0000
---

```cpp
// int& : 정수형 lvalue
// int&& : 정수형 rvalue
// T& : 모든 타입 lvalue
// T&& : 모든 타입 lvalue, rvalue 모두

template<typename F, typename T> 
void chronometry(F f, T&& arg)    // forwarding reference, 이제 l, r 모두 받을 수 있음.
{
  f(static_cast<T&&>(arg));
}

// 결국 perfect forwarding의 버전을 l, r버전으로 만들지 않고 forwarding reference만 만들어도 된다는 말!!

// 어떻게 동작하는지 예를들어 설명한다.
int main()
{
  int n = 0;
  
  chronometry(&goo, n);
  // 인자로 lvalue가 전달되고 -> T : int& -> T&& : int & && -> int& 이 된다.
  // 잉 ?? 나는 int를 전달한게 아닌가? -> 여기는 받아들여야 하는 부분 n이라는 변수를 주면 int&로 컴파일러가 지정해줌.
  
  chronometry(&hoo, 10);
  // 인자로 rvalue가 전달되고 -> T : int -> T&& : int&& -> int&& 이 된다.
}
```

```cpp
// 정리
// 1. 함수 인자로 forwarding reference를 사용하자
// 2. 캐스팅도 꼭 static_cast를 안쓰고 std에서 만들어진 함수 forward를 써도 좋다.
template<typename F, typename T> 
void chronometry(F f, T&& arg)
{
  f(std::forward<T>(arg));    // forward가 결국 내부적으로 위의 캐스팅 수행
}
```