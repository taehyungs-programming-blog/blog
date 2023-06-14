---
title: "(C++) String Array 길이 찾기"
date: 2019-12-05 00:00:00 -0000
---

> 코딩테스트를 진행하다보면 string배열의 길이를 알아야 할 경우가 있다.
>
> 가령 ...

```cpp
int main() {
    string A[] = gets(stdin);
    // 다음과 같이 스트링 배열로 입력이 들어온다면
    Fn(A, sizeof(A)/sizeof(*A));
    // sizeof(A)/sizeof(*A)로 스트링 배열의 길이를 알아 낼 수 있다.
}
```