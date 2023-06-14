---
title: "(C++) inline function"
date: 2020-02-22 00:00:00 -0000
---

## 목차

* [inline 기본적 내용](https://8bitscoding.github.io/cpp-im-inline-func/#inline-기본적-내용)
* [인라인 함수와 함수 포인터](https://8bitscoding.github.io/cpp-im-inline-func/#인라인-함수와-함수-포인터)
* [인라인 함수와 linkage](https://8bitscoding.github.io/cpp-im-inline-func/#인라인-함수와-linkage)

---

## inline 기본적 내용

```cpp
int Add1(int a, int b) { return a + b; }
inline int Add2(int a, int b) { return a + b; }

int main()
{
    int n1 = Add1(1, 2);    // 호출
    int n2 = Add2(1, 2);    // 치환
    // 장점 : 속도가 빨라진다.
    // 단점 : 목적코드가 커진다.
}
```

> 그런데 inline을 만들어도 치환이 안되고 호출이 되는 경우가 발생<br>
> 각 컴파일러에서 inline을 강제하는 옵션을 넣어줄 필요가 있다.<br>
> * cl : /Ob1 -> inline을 강제해준다.

> 그리고 실제로 보면 목적코드가 줄어드는 경우도 많이 발생
> * 단, inline내부의 내용이 적을경우 오히려 목적코드 양이 줄어들게 된다.

---

## 인라인 함수와 함수 포인터

```cpp
int Add1(int a, int b) { return a + b; }
inline int Add2(int a, int b) { return a + b; }

int main()
{
    int n1 = Add1(1, 2);    // 호출
    int n2 = Add2(1, 2);    // 치환

    int(*f)(int, int);
    f = &Add2;

    int n3 = f(1, 2);       // 호출? 치환? => 호출!
}
```

> * inline은 컴파일 시간에 동작함을 기억하자.
> * 그런데 함수 포인터는 컴파일 시간에 확정을 지을 수 없다.
> * 결국, inline은 함수포인터에 쓸 수 없다.

---

## 인라인 함수와 linkage

```cpp
// add.h
int Add1(int a, int b);
inline int Add2(int a, int b);
```

```cpp
// add.cpp
int Add1(int a, int b) 
{ 
    return a + b;
}

inline int Add2(int a, int b) 
{ 
    return a + b;
}
```

```cpp
#include "add.h"

int main()
{
    Add1(1, 2);
    Add2(1, 2);     // error! -> linker error가 발생한다.
}
```

> main 이 호출되는 순간 Add2 코드를 inline해서 넣어햐하는데<br>
> add.h에서는 Add2의 내용을 알 수 없다(구현은 cpp에 되어 있기 때문에) 따라서 컴파일 순간에 inline을 넣을 수 없다.

> 결론은 h에 inline구현부를 넣어야 함.

```cpp
// add.h
int Add1(int a, int b);
inline int Add2(int a, int b) 
{ 
    return a + b;
}
```