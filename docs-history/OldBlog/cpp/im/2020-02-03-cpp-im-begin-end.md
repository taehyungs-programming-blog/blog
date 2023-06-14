---
title: "(C++) begin/end"
date: 2020-02-03 00:00:00 -0000
---

```cpp
#include <iostream>
#include <list>
#include <vector>
using namespace std;

// 컨테이너의 모든 요소를 출력
template<typename T> void show(T& c)
{
    auto p1 = c.begin();
    auto p2 = c.end();

    while(p1 != p2)
    {
        cout << *p1 << endl;
        ++p1;
    }
}

int main()
{
    list<int> c = { 1, 2, 3 };
    // vector<int> c = { 1, 2 ,3 };
    int x[3] = {1, 2, 3};       // 이거도 가능???
    show(c);
}
```

> 가능하게 만들어보자!

```cpp
template<typename T> void show(T& c)
{
    auto p1 = begin(c);
    auto p2 = end(c);

    while(p1 != p2)
    {
        cout << *p1 << endl;
        ++p1;
    }
}
```

* 반복자를 꺼내는 2가지 방법을 기억(아래방법이 더 좋음!)

---

```cpp
#include <iostream>
#include <list>
#include <vector>
using namespace std;

template<typename C>
constexpr auto begin(C& c) -> decltype(c.being())
{
    return c.begin();
}

template<typename C>
constexpr auto end(C& c) -> decltype(c.end())
{
    return c.end();
}

// for array
constexpr template<typaname T, std::size_t N>
T* begin(T(&arr)[N])
{
    return arr;
}

constexpr template<typaname T, std::size_t N>
T* end(T(&arr)[N])
{
    return arr+N;
}

int main()
{
    list<int> s = { 1, 2, 3 };
    int x[3] = { 1, 2, 3 };

    auto p1 = begin(s);
    auto p2 = begin(x);
}
```

* constexpr : 컴파일타임에 실행