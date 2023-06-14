---
title: "(C++) static_assert"
date: 2020-02-03 00:00:00 -0000
---

```cpp
#include <iostream>
using namespace std;

void foo(int age)
{
  // ...
}

int main()
{
   foo(-10);    // 나이가 마이너스???
}
```

```cpp
#include <cassert>

void foo(int age)
{
  assert(age > 0);
  // 실행시간에 expr이 거짓이면 메시지를 출력 후 프로그램 종료
}
```

```cpp
#include <cassert>

void foo(int age)
{
  static_assert(age > 0, "age error");    // error - age는 변수이기에 컴파일 시간에 알 수 없음.
  // 컴파일시간에 에러체크
}
```

---

## static_assert의 활용

```cpp
#include <iostream>
#include <mutex>
using namespace std;

#pragma pack(1)   // padding을 없애달라

struct PACKET
{
  char cmd;
  int data;
};
static_assert( sizeof(PACKET)==sizeof(char)+sizeof(int), "error, unexpected padding!" );

template<typename T> void Swap(T& a, T& b)
{
  // T가 가져야하는 조건을 조사
  static_assert(is_copy_constructible<T>::value, "error. T is not copyable");
  T tmp = a;
  a = b;
  b = tmp;
}

int main()
{
  mutex m1;
  mutex m2;
  Swap(m1, m2);   // mutex는 복사 생성자가 없기에 에러.
}
```

* 함수 안에 만들수 있고 함수 밖에 만들 수 도 있다.
* 함수 또는 클래스 템플릿을 만드 때 type_traits를 사용해서 T가 가져야 하는 조건을 static_assert로 조사하는 코드가 널리 사용
* static_assert를 잘 활용하면 가독성 높은 에러 메시지를 만들 수 있다.