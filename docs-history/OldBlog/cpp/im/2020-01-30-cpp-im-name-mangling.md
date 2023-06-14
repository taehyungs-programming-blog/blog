---
title: "(C++) name mangling"
date: 2020-01-30 00:00:00 -0000
---

```cpp
int square(int n)
{
  return n * n;
}

double square(double d)
{
  return d * d;
}

int main()
{
  square(3);
  square(3.3);
  // name mangling - 컴파일러에 의해서 함수의 이름이 변경된다.
  // 예를 들어 int형을 쓰는 square는 컴파일러에 의해서 squarei로 mangling된다.
  // 위는 그냥 예시지 꼭 저런식으로 이름을 변경한다는 말은아님
  // g++과 cl 컴파일러에 따라 다른식으로 mangling하기도 한다.
}
```

> * 이러한 name mangling으로 C/C++호환성 문제가 발생한다.
> * 자세한것은 extern에서 설명!

---

## extern "C"

```cpp
// square.h
int square(int);
```

```cpp
// square.c
int square(int n)
{
  return n * n;
}
```

```cpp
// main.cpp
int main()
{
  square(5);    // error!
  // 각 컴파일러는 컴파일시 파일의 확장자 명을 보고 C/C++로 컴파일하게 된다.
  // square.c가 c로 되어있기에 함수오버로딩이 없는 c문법대로 컴파일하게된다.
  // main.cpp는 square를 찾을 수 없게 되어 error!
}
```

> 해결책

```cpp
extern "C"
int square(int);    // 컴파일러에게 C언어라고 통보해준다.
```

> 또 다른 문제점?

```cpp
// main.c
int main()
{
  square(5);    // error!
  // extern "C"는 C++문법인데 C에서 사용하여 발생!
}
```

> 최후 결론 해결책!

```cpp
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif
```