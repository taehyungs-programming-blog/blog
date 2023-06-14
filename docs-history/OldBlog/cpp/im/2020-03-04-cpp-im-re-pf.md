---
title: "(C++) perfect forward 다시 정리"
permalink: cpp/perfect-forward/                # link 직접 지정
#toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-03-04 00:00:00 -0000
last_modified_at: 2020-10-08 00:00:00 -0000
sidebar:
  title: "목차"
  nav: cpp
tag:
  - cpp
category:
  - perfect-forward
classes: wide
excerpt: ""
header:
  teaser: /file/image/cpp-page-teaser.gif
---

## perfect forwarding 개념

역시 이렇게 시작한다.<br>
특정 함수가 동작하는데 걸리는 시간을 측정하는 함수를 만들고 싶다<br>

```cpp
int main()
{
  foo(5);   // 이 함수에 걸리는 시간을 측정하고 싶다면?

  // chronometry라는 함수를 두고 매개변수로 측정하고 싶은 함수명 + 그 함수의 매겨변수를 넣으면
  // 걸리는 시간을 출력하는 함수 chronometry를 만들어보자.
}
```

```cpp
// F가 함수이고
// A가 함수에 들어갈 매개변수라 생각하자
template<typename F, typename A>
void chronometry(F f, A arg)    // 그런데 이렇게 만들면 A를 값으로 받기에 완벽한 복사라 할 수 없다.
{
  f(arg);
}
```

```cpp
template<typename F, typename A>
void chronometry(F f, A& arg)    // 참조로 받자?? -> 이러면 rvalue를 받을 수 없다.
{
  f(arg);
}
```

이런 문제에서 perfect forwarding이 시작된다.<br>
**매개변수를 r이든 l이든 무엇이든 받아서 내가 사용하는 함수에 넣고싶다 -> perfect forwarding!**

```cpp
template<typename F, typename T> 
void chronometry(F f, T&& arg)
{
    f(std::forward<T>(arg));
    // 아래는 위와 동일한 표현이다.
    // f(static_cast<T&&>(arg));
}
```

리턴형까지 정리하면 ...

```cpp
template<typename F, typename T>
decltype(auto) chronometry(F f, T&& arg)
{
    return f(std::forward<T>(arg));
}
```

---

## 그래서 언제쓰는데?

```cpp
#include <iostream>
#include <vector>
using namespace std;

class Point
{
    int x, y;
public:
    Point(int a, int b) { cout << "Point()" << endl; }
    ~Point() { cout << "~Point()" << endl; }
    Point(const Point&)
    {
        cout << "Point(const Point&)" << endl;
    }
};


int main() {
	vector<Point> v;

    Point p(1, 2);
    v.push_back(p);
    
	return 0;
}
```

```s
# 결과
Point()                 # 생성자 호출
Point(const Point&)     # 복사 생성자 호출
~Point()
~Point()
# p가 할당되는 과정에서 복사가 일어나고 생성자/소멸자 2회씩 오버헤드가 발생
```

> * [Run This Code](https://ideone.com/tYXp0c)

```cpp
int main()
{
    vector<Point> v;

    v.emplace_back(1, 2);
}
```

```s
Point()
~Point()
```

> * [Run This Code](https://ideone.com/CTGeVt)

근데 perfect forwarding 말 하다 말고 갑자기 왠 vector?<br>
emplace_back 내부가 perfect forwarding으로 구현된다.<br>
말인즉슨 복사생성이 안일어 난다는말!

---

## 추가

* [참고사이트](https://www.youtube.com/watch?v=0xcCNnWEMgs&list=PL5jc9xFGsL8FWtnZBeTqZBbniyw0uHyaH&index=4)

```cpp
void foo(boVector arg);   
// boVector에게는 move, copy constructor가 있다고 가정한다.

template<typename T>
void relay(T arg) {
  foo(arg);
}

int main() {
  boVector reusable = createBoVector();
  relay(reusable);        // T : boVector -> copy constructor 호출
  // ...
  relay(createBoVector());  // T : boVector&& -> move contructor 호출
}
```

```cpp
// solution:
template<typename T>
void relay(T&& arg) {
  foo(std::forward<T>(arg));
}
```

이게 왜 되나?

## Reference Collapsing Rules

```cpp
T& & => T&
T& && => T&
T&& & => T&
T&& && => T&&
// 그냥 &가 적은쪽으로 취함된다고 외우자
```

```cpp
// remove_reference
template<class T>
struct remove_reference;

// T is int&
remove_reference<int&>::tpye i;   // int i

// T is int
remove_reference<int>::type i;    // int i
```

그럼 다시 위의 예제로 돌아가서 ...

```cpp
template<typename T>
void relay(T&& arg) {
  // ...
}

relay(9);   // => T = int&& => int&& && => int&&
relay(x);   // => T = int& => int& && => int&
```

```cpp
template<typename F, typename T> 
void chronometry(F f, T&& arg)
{
    f(std::forward<T>(arg));
    // 아래는 위와 동일한 표현이다.
    // f(static_cast<T&&>(arg));
}
```