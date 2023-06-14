---
title: "(C++) trivial 활용"
date: 2020-02-01 00:00:00 -0000
---

> 왜 trivial을 쓰나?

```cpp
#include <iostrea>
#include <type_trits>
using namespace std;

int main()
{
    char s1[10] = "hello";
    char s2[10] = { 0 };

    // strcpy(s2, s1);
    copy_type(s1, s2, 10);      // 모든 타입의 배열을 목사하는 함수
}
```

```cpp
template<typename T> void copy_type(T* dst, T* src, int sz)
{
    memcpy(dst, src, sizeof(T) * sz);
}
```

> 복사하는 타입이 Class, Struct등 Object일때 얕은 복사만 된다.

```cpp
People p1[10];
People p2[10];

copy_type(s1, s2, 10);      // 얕은 복사만 됨.
```

> 만약 People의 복사생성자가 하는 일이 없다면??<br>
> 그냥 memcpy해도 무방<br>

```cpp
template<typename T> void copy_type(T* dst, T* src, int sz)
{
    if( is_triviall_copyable<T>::value )
    {
        cout << "복사 생성자가 trivial 할 때" << endl;
        memcpy(dst, src, sizeof(T) * sz);
    }
    else
    {
        cout << "복사 생성자가 trivial하지 않음" << endl;
        while(sz--)
        {
            new(dst) T(*src);       // 복사 생성자 호출
            ++dsx, ++src;
        }
    }
}
```

```cpp
struct People
{
    People() {}
    People(const People&) {}    // 복사생정자.
};
```

---

> 또 다른 copy_type을 만드는 방법

> `enable_if`라는 개념을 알아야하는데... template을 좀 이해해야함. 

```cpp
template<typename T> 
typename enable_if<is_trivially_copyable<T>::value>::type copy_type(T* dst, T* src, int sz)
{
    cout << "복사 생성자가 trivial 할 때" << endl;
    memcpy(dst, src, sizeof(T) * sz);
}

template<typename T> 
typename enable_if<!is_trivially_copyable<T>::value>::type copy_type(T* dst, T* src, int sz)
{
    cout << "복사 생성자가 trivial하지 않음" << endl;
    while(sz--)
    {
        new(dst) T(*src);       // 복사 생성자 호출
        ++dsx, ++src;
    }
}
```
