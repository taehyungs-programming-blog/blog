---
title: "(C++) function vs functor"
date: 2020-02-22 00:00:00 -0000
---

```cpp
#include <algorithm>
using namespace std;

// 일반 함수
inline bool cmp1(int a, int b) { return a > b; }
inline bool cmp2(int a, int b) { return a < b; }

// functor
struct Less { inline bool operator()(int a, int b) const { return a < b; }}
struct Greater { inline bool operator()(int a, int b) const { return a > b; }}

int main()
{
    int x[10] = { 1,3,5,7,9,2,4,6,8,10 };

    // STL sort : 모든 인자가 템플릿으로 되어 있다.
    
    // 비교정책으로 일반함수를 전달
    sort(x, x+10, cmp1);    // sort(int*, int*, bool(*)(int, int)) 인 함수 생성.
    sort(x, x+10, cmp2);    // sort(int*, int*, bool(*)(int, int)) 인 함수 생성.
    // 장점 : 정책을 교체해도 sort기계어는 하나 -> 코드 메모리 절약
    // 단점 : 정책이 인라인 치환될 수 없다. -> 데이터량이 많을시 성능 저하의 요인이 된다.

    Less f1;
    Greator f2;
    sort(x, x+10, f1);  // sort(int*, int*, Less)
    sort(x, x+10, f2);  // sort(int*, int*, Greator)
    // 장점 : 정책함수가 인라인치환이 된다.
    // 단점 : 정책을 교체한 횟수 만큼 sort기계어 생성

    // 정리 : 비교정책의 데이터가 많을 시 -> functor
}
```

```cpp
// Less, Greator는 이미 C++에서 지원한다.
#include <iostream>
#include <algorithm>
#include <functional>       // less<>, greater<>
using namespace std;

int main()
{
    int x[10] = { 1,3,5,7,9,2,4,6,8,10 };

    // 이런식으로 쓰면 된다.
    less<int> f1;
    sort(x, x+10, f1);

    sort(x, x+10, less<int>());
}
```

---

## 문제

```cpp
inline bool cmp1(int a, int b) { return a > b; }
inline bool cmp2(int a, int b) { return a < b; }
struct Less { inline bool operator()(int a, int b) const { return a < b; }}
struct Greater { inline bool operator()(int a, int b) const { return a > b; }}

template<typename T> void foo(T f)
{
    bool b = f(1, 2);
}

int main()
{
    foo(cmp1);
    foo(cmp2);
    foo(Less());
    foo(Greater());
}

// 문제 1. 컴파일 했을때 foo 함수는 몇개가 생서될까?
// 문제 2. 각각의 foo 함수에서 f(1, 2)의 인라인 치환 여부?
// 문제 3. 컴파일했을때 foo를 제외한 함수의 갯수는?
```