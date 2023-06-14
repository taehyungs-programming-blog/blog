---
title: "(C++) 상태를 가지는 함수"
date: 2020-02-22 00:00:00 -0000
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

---

## 상태를 가지는 함수

```cpp
#include <iostream>
#include <string>
#include <bitset>
using namespace std;

int main()
{
    bitset<10> bs;

    bs.reset();     // 모든 요소를 0
    bs.reset(4);    // 4번째 비트를 0으로

    bs[2] = 1;
    bs[1].flip();   // 1번째 비트 0 -> 1

    string s = bs.to_string();

    unsigned long n = bs.to_ulong();

    cout << s << endl;  // 000000110
    cout << n << endl;  // 6
}
```

```cpp
int ramdon()
{
    return rand() % 10;     // 중복되지 안헥 만들고 싶다?
}

int main()
{
    for(int i = 0; i < 10; i++)
        cout << random() << " ";
}
```

```cpp
// 중복되지 안헥 만들고 싶다?
bitset<10> bs;  // 초기 0으로 초기화 됨.

void clear_random() { bs.reset(); }

int ramdon()
{
    int v = -1;

    do{
        v = rand() % 10;
    } while(bs.test(v));
    bs.set(v);
    return v;   
}
```

> * 일반함수 : 동작만 있고 상태가 없다.
> 위의 예제에서 `bitset<10> bs;`이라는 상태를 한개 더 갖고싶다면?<br>
> 그냥 전역으로 `bitset<10> bs1;`를 만드는 방법 밖에없다<br>
> 이럴때 functor를 쓰면 좋다.

```cpp
class Random
{
    bitset<10> bs;
public:
    Random()
    {
        bs.reset();
    }
    int operator()()
    {
        int v = -1;
        do{
            v = rand() % 10;
        } while(bs.test(v));

        bs.set(v);
        return v;
    }
};

int main()
{
    Random random;
    for(int i = 0; i < 10; i++)
        cout << random() << " ";
    random();
}
```