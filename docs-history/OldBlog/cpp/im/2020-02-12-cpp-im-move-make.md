---
title: "(C++) std::move 만들기"
date: 2020-02-12 00:00:00 -0000
---

```cpp
#include <iostream>
#include "Test.h"
using namespace std;

int main()
{
  Test t1;
  Test t2 = t1;   // copy
  Test t3 = move(t1);   // 결국 이 동작은 static_cast<Test&&>(t1); 을 대신한다.
  
  // 우리가 xmove()를 따로 만들어보자!
}
```

```cpp
template<typename T> T&& xmove(T& obj)
{
  return static_cast<T&&>(obj);
}
```

```cpp
// 한번 사용해보자.
int main()
{
  Test t1;
  Test t2 = t1;         // copy
  Test t3 = xmove(t1);  // move
  
  // 그런데? 이런거 까지 다 되게 하고 싶다??
  Test t4 = xmove( Test() );    // 매개변수가 rvalue이기에 rvalue를 받을 수 있게 수정을 해야겠구만...
}
```

```cpp
template<typename T> T&& xmove(T&& obj)   // 수정완료!
{
  return static_cast<T&&>(obj);
}
```

```cpp
// 다시해보자.
int main()
{
  Test t1;
  Test t2 = t1;         // copy
  Test t3 = xmove(t1);  // copy -> ??? -> 왜 여기서 copy가 되지?
  Test t4 = xmove( Test() );    // move
}
```

```cpp
// 인자로 lvalue 전달 : T => Test&, T&& => Test& && => Test& 되기에 copy
// 인자로 rvalue 전달 : T => Test, T && => Test&& 되기에 move
template<typename T> T&& xmove(T&& obj)
{
  // 목표 rvalue로 캐스팅!
  return static_cast<T&&>(obj);
  
  return static_cast<remove_reference<T>::type &&>(obj); // 이렇게 하면된다. -> 말 그대로 레퍼런스를 제거해 달라는명령
  // 리턴도 바꿔야 하기에(아래참조)
}
```

```cpp
template<typename T>
typename remove_reference<T>::type && xmove(T&& obj)
{
  return static_cast<remove_reference<T>::type &&>(obj);
}
```