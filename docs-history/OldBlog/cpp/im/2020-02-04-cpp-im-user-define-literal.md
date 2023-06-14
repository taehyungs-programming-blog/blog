---
title: "(C++) user define literal"
date: 2020-02-03 00:00:00 -0000
---

## user define literal

> liveral뒤에 사용자 정의 접미사를 붙이는 문법

```cpp
#include <iostream>
using namespace std;

// _ : 사용자 사용가능 literal
// _로 시작하지 않는 것 : reserved(미래에 C++에서 사용하지 모른다.)
int operator""_k(unsigned long long v)
{
  return v * 1000;
}

int main()
{
  int n1 = 10_k;
  cout << n1 << endl;
}
```

---

## user define literal은 규칙이 정해져 있다.

* **integer literals** - `operator””suffix(unsigned long long )` or `operator””suffix(const char*)`
* **floating point literals** - `operator””suffix(long double)` or `operator””suffix(const char*)`
* **character literals** - `operator””suffix(char)`
* **string literals** - `operator””suffix(const char*, size_t)`