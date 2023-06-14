---
title: "(C++) 임시객체가 생성되는 다양한 경우"
date: 2020-02-08 00:00:00 -0000
---

```cpp
struct Base
{
    int v = 10;
};

struct Derived : public Base
{
    int v = 20;
};

int main()
{
    Derived d;      // v가 두 개가 있음.

    cout << d.v << endl;    // 20, Derived v
    cout << d.Base::v << endl;      // 10

    cout << (static_cast<Base>(d)).v << endl;   // 10
    cout << (static_cast<Base&>(d)).v << endl;   // 10
}
```

> 여기서 하고 싶은 말은 아래 두 코드의 차이점을 알자이다.

```cpp
    cout << (static_cast<Base>(d)).v << endl;
    cout << (static_cast<Base&>(d)).v << endl;
```

> * `(static_cast<Base>(d)).v` : 복사생성자(임시객체)를 만들게된다. Derived의 d를 가져오는 것이 아니라 임시객체의 d를 가져온다.
> * `(static_cast<Base&>(d)).v` : 

```cpp
(static_cast<Base>(d)).v = 30;  // error - 임시객체에 넣을 수 없다.
(static_cast<Base&>(d)).v = 30;  // ok
```

---

## 연산자와 임시객체

```cpp
int main()
{
    int n = 3;
    n++;        // operator++(n) -> 어떻게 만들까? 아래를 보자.
    ++n;

    n++ = 10;       // error - 아래코드에서 보면 알 듯 값리턴을 하기에 값에 10을 넣을 순 없다.
    ++n = 10;       // ok - 아래코드와 같이 참조형 리턴이기에 가능
}
```

```cpp
// 후위형의 2번째 매개변수는 전위형과의 차이점을 두기위해 넣음
int operator++(int& n, int)
{
    int temp = n;
    n = n + 1;
    return temp;        // 후위형이기에 현재값을 넘겨야한다.
}
```

```cpp
int& operator++(int& n)     // 참조형 리턴을 하는 이유는 ++(++n)과 같은 처리를 위해서이다.
{
    n = n + 1;
    return n;
}
```