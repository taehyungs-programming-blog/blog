---
title: "(C++) function object"
date: 2020-02-22 00:00:00 -0000
---

## 함수 객체(functor)의 정의

```cpp
class Plus
{
public:
    int operator()(int a, int b)
    {
        return a + b;
    }
};

int main()
{
    Plus p; // plus라는 객체

    int n = p(1, 2);       // plus라는 객체를 마치 함수를 쓰듯 사용 -> Function Object라 한다.
    cout << n << endl;

    // a + b; // a.operator+(b)
    // a - b; // a.operator-(b)
    // a();   // a.operator()()
    // a(1,2);// a.operator()(1,2)
}
```

```cpp
template<typename T> struct Plus     // 간단한 함수 객체는 struct로 만든다.
{
public:
    T operator()(T a, T b) const // 상수로 불러질때를 대비
    {
        return a + b;
    }
};

void foo(const Plus<int>& p)
{
    int n = p(1, 2)
}

int main()
{
    Plus<int> p;
}
```

---

## 함수 객체(functor)의 장점

```cpp
#include <algorithm>

void Sort(int* x, int n)
{
    for(int i = 0; i < n - 1; i++)
    {
        for(int j = i + 1; i < n; j++)
        {
            if(x[i] > x[j])
                swap(x[i], x[j]);
        }
    }
}

int main()
{
    int x[10] = { 1,3,5,7,9,2,4,6,8,10 };
    Sort(x, 10);        // 오름차순으로 정리되는데 내림차순이 필요한 순간이 온다면? -> 새로만들어야 하나?
    // 라이브러리 설계자들은 변하지 않은 전체 흐름속에서 변경되어야 하는 부분을 분리한다.
}
```

```cpp
void Sort(int* x, int n, bool(*cmp)(int, int)))
{
    for(int i = 0; i < n - 1; i++)
    {
        for(int j = i + 1; i < n; j++)
        {
            // if(x[i] > x[j])
            if(cmp(x[i], x[j]))
                swap(x[i], x[j]);
        }
    }
}

bool cmp1(int a, int b) { return a > b; }
bool cmp2(int a, int b) { return a < b; }

int main()
{
    int x[10] = { 1,3,5,7,9,2,4,6,8,10 };
    Sort(x, 10, cmp2);
}

// 단, 이러한 호출은 cmp를 재 호출해야하기에 속도에서 분리하다.
```

> 여기서 해결책으로 보이는 부분은 cmp에 넣을 수 있는 함수는 bool을 리턴하며 int를 2개 넣을수 있는 함수는 뭐든 들어갈 수 있다는 점이다.<br>
> 이 문제를 함수객체로 해결해 본다.

```cpp
struct Less
{
    inline bool operator()(int a, int b) const { return a < b; }
};

struct Greater
{
    inline bool operator()(int a, int b) const { return a > b; }
};

void Sort(int* x, int n, Less cmp)
{
    for(int i = 0; i < n - 1; i++)
    {
        for(int j = i + 1; i < n; j++)
        {
            if(cmp(x[i], x[j]))
                swap(x[i], x[j]);
        }
    }
}

int main()
{
    Less    f1;
    Greater f2;

    int x[10] = { 1,3,5,7,9,2,4,6,8,10 };
    Sort(x, 10, f1);
    Sort(x, 10, f2);        // compile error -> 요 문제까지 해결해보자.
}
```

```cpp
template<typename T> void Sort(int* x, int n, T cmp)
{
    for(int i = 0; i < n - 1; i++)
    {
        for(int j = i + 1; i < n; j++)
        {
            if(cmp(x[i], x[j]))
                swap(x[i], x[j]);
        }
    }
}
```

```cpp
int main()
{
    Less    f1;
    Greater f2;

    int x[10] = { 1,3,5,7,9,2,4,6,8,10 };
    Sort(x, 10, f1);
    Sort(x, 10, f2);
    // 단, template을 사용하기에 Sort가 매번 생성이 되어 목적코드가 커지는 현상이 발생
    // 물론 위 예제는 Sort가 작기에 이렇게 만드는게 더 이득이긴하다.
}
```