---
title: "(C++) chronometry 함수 완성"
date: 2020-02-13 00:00:00 -0000
---

```cpp
int x = 10;
void foo(int a) {}

template<typename F, typename T>
void chronometry(F f, T&& arg)
{
  f(std::forward<T>(arg));
}

int main()
{
  chronometry(&foo, 10);    // 그런데 리턴처리는?
}
```

```cpp
int x = 10;
int foo(int a) { return x; }

// 리턴 타입을 아래와 같이 지정하고 싶지만 ... f의 함수 선언시점 보다 리턴 시점에서 사용하는게 빨라서 error
template<typename F, typename T>
decltype(f(std::forward<T>(arg))) chronometry(F f, T&& arg)
{
   return f(std::forward<T>(arg));
}
```

```cpp
template<typename F, typename T>
auto chronometry(F f, T&& arg) -> decltype(f(std::forward<T>(arg))) 
{
   return f(std::forward<T>(arg));
}
```

```cpp
// 이건 되나?
template<typename F, typename T>
auto chronometry(F f, T&& arg)  // decltype을 지워도 상관없다(C++14)
{
   return f(std::forward<T>(arg));
}
```

```cpp
// 요것도 되나?
int x = 10;
int& foo(int a) { return x; }

template<typename F, typename T>
auto chronometry(F f, T&& arg)
{
   return f(std::forward<T>(arg));
}

int main()
{
  int& ret = chronometry(&foo, 10);   // error! -> auto는 참조속성을 제거해 버리기 때문!
  ret = 20;
  cout << x << endl;
}
```

```cpp
// 그런데 이렇게 하면 정상동작
template<typename F, typename T>
auto chronometry(F f, T&& arg) -> decltype(f(std::forward<T>(arg)))   // 자료형을 명시해줘야한다.
{
   return f(std::forward<T>(arg));
}
```

```cpp
// 좀더 간단히 해보자.
template<typename F, typename T>
decltype(auto) chronometry(F f, T&& arg)
{
   return f(std::forward<T>(arg));
}
```

---

## 최종 완성형

```cpp
// 인자를 여러개 받는 함수를 처리하려면? + 참고로 가변인자 템플릿은 인자가 없어도 동작함.
template<typename F, typename Types>
decltype(auto) chronometry(F f, Types&& ... arg)
{
   return f(std::forward<Types>(arg)...);
}
```