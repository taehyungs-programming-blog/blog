---
title: "(C++) 함수포인터가 인라인치환이 안되는 이유"
date: 2020-03-04 00:00:00 -0000
---

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

* inline은 컴파일 시간에 동작함을 기억하자.
* 그런데 함수 포인터는 컴파일 시간에 확정을 지을 수 없다.
* 결국, inline은 함수포인터에 쓸 수 없다.

