---
title: "(C++) reference_wrapper"
date: 2020-02-19 00:00:00 -0000
---

```cpp
void foo(int& a) { a = 30; }

template<typename F, typename T>
void chronometry(F f, T& arg)
{
    f(arg);
}

int main()
{
    int n = 0;
    chronometry(&foo, n);

    function<void()> f;
    f = bind(&foo, n);      // n이 언제 파괴될지 모르니 n을 참조로 받으면 안된다.
    // 그래도 참조로 받고 싶다면?
    // bind(&foo, ref(n));
    f();
}
```

```cpp
// chronometry에서도 두 번째 인자를 값으로 받아서 처리해보자.
template<typename F, typename T>
void chronometry(F f, T arg)
{
    f(arg);
}

int main()
{
    int n = 0;
    chronometry(&foo, &n);      // 두 번째 인자를 참조형으로 준다면? -> 나중에 파괴될 걱정도 없고 좋은데?
}
```

```cpp
#include <iostream>
using namespace std;

template<typename T> struct xreference_wrapper
{
    T* ptr;
public:
    xreference_wrapper(T& r) : ptr(&r) {}
};

int main()
{
    int n = 0;
    xreference_wrapper<int> ref(n);     // 결국 주소보관

    int& r = ref;       // 주소를 보관하고 있다가 참조로 꺼내고 싶다
}
```

```cpp
template<typename T> struct xreference_wrapper
{
    T* ptr;
public:
    xreference_wrapper(T& r) : ptr(&r) {}

    operator T&() { return *ptr; }
};

int main()
{
    int n = 0;
    xreference_wrapper<int> ref(n); 

    int& r = ref; 
    r = 30;

    cout << n << endl; // 30 <- 우리가 원하는 값이 나온다.
}
```

* reference_wrapper
    - 참조와 유사하게 동작하는 클래스 템플릿
    - 참조로 변환 가능 포인터

* ref, cref
    - reference_wrapper를 생성하는 helper함수

```cpp
template<typename T> struct xreference_wrapper
{
    T* ptr;
public:
    xreference_wrapper(T& r) : ptr(&r) {}
    operator T&() { return *ptr; }
};

void foo(int& a) { a = 30; }

template<typename F, typename T>
void chronometry(F f, T arg)
{
    f(arg);
}

int main()
{
    int n = 0;
    xreference_wrapper<int> r(n);       // n의 주소를 기록
    chronometry(&foo, r);

    cout << n << endl; // 30
}
```

> 매번 이렇게 보내야하나?

```cpp
chronometry(&foo, xreference_wrapper<int>(n));
// 이거도 길다
```

```cpp
template<typename T>
xreference_wrapper<T> xref(T& obj)
{
    return xreference_wrapper<T>(obj);
}

int main()
{
    int n = 0;
    chronometry(&foo, xref(n));     // 값 방식으로 받는 chronometry함수에 참조형으로 전달이 가능!
}
```

---

```cpp
#include <iostream>
#include <functional>
using namespace std;

int main()
{
    int n1 = 10;
    int n2 = 20;

    int& r1 = n1;
    int& r2 = n2;

    r1 = r2;            
    // r1이 r2를 가리키게 될까?
    // 아니면 값이 20으로 변경되나?
    // 정답은 값만 20으로 변경! -> 참조형은 한 번 초기화 되면 변하지 않음.

    cout << n1 << endl;
    cout << n2 << endl;
    cout << r1 << endl;
    cout << r2 << endl;
}
```

```cpp
reference_wrapper<int> r1(n1);
reference_wrapper<int> r2(n2);

r1 = r2;        // 얕은 복사가 일어난다. -> r1이 가리키는 값이 n2로 변경이 된다는 말!
```

* 일반 참조
    - 대입시 참조가 아닌 값이 이동한다.
* reference_wrapper
    - 값이 아닌 참조가 이동한다.

```cpp
vector<int&> v(10);              // vector에는 참조를 못넣음
vector<reference_wrapper<int>>;  // 단, reference_wrapper는 들어간다.
```