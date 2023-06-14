---
title: "(C++) if-init, if-constexpr"
date: 2020-02-05 00:00:00 -0000
---

## if-init

```cpp
int foo()
{
  return 0;
}

int main()
{
  /*
  int ret = foo();
  
  if(ret == 0)
  {
  }
  */
  
  // if(초기구문; 조건문)
  if(int ret = foo(); ret = 0)
}
switch(int n = foo(); n)
{
case 0: break;
}
```

---

## if-constexpr

```cpp
#include <iostream>
#include <type_traits>
using namespace std;

// T가 포인터라면 주소값 + 값까지 출력
template<typename T> void printv(T v)
{
  if(is_pointer<T>::value)    // error - if는 실행시간에 확인
    cout << v << " : " << *v << endl;   // 결국 여기까지 넘어오고, *v 뭔가를 역참조? -> 이게 포인터가 아니라면 에러이기에 에러출력!
  else
    cout << v << endl;
}

int main()
{
  int n = 10;
  printv(n);
  printv(&n);
}
```

```cpp
template<typename T> void printv(T v)
{
  if constexpr (is_pointer<T>::value)   // 컴파일타임에 확인
    cout << v << " : " << *v << endl;   // 포인터가 아닐경우 여기로 넘어가지 않는다.
  else
    cout << v << endl;
}
```

```cpp
// 단, 컴파일 시간에 조건을 확인하기에
if constexpr (v == 10) // 와 같은 식은 불가능!
```