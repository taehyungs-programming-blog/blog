---
title: "(C++) delete 함수삭제"
date: 2020-01-07 00:00:00 -0000
---

> 컴파일 에러로 심각한 버그를 방지할 수 있다.

```cpp
int gcd(int a, int b)
{
  return b != 0 ? gcd(b, a % b) : a;
}

int gcd(double a, double b) = delete;     // compile error

int main()
{
  gcd(2, 10);
  gcd(2.2, 4.3);    // gcd(double, double) ?? -> compile error 가 발생하지 않는다.
}
```

---

### Example

```cpp
template<typename T>
T square(T a)
{
  return a * a;
}

char square(char) = delete;

int main()
{
  square(3);    // T = int
  square(3.3);  // T = double
  square('a');  // T = char
}
```