---
title: "C++ Template : tuple"
permalink: cpp/template/tuple/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-27 00:00:00 -0000
last_modified_at: 2020-06-30 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
header:
  teaser: /file/image/cpp-page-teaser.gif
tag:
  - C++
  - Template
category:
  - tuple
excerpt: ""
---

```cpp
#include <iostream>
#include <tuple>
using namespace std;

int main()
{
  tuple<> t0;
  tuple<int> t1(1);
  tuple<int, double, int, char> t4(1, 3.4, 2, 'A');
  // 이런 tuple을 만들어 보자
  
  get<2>(t4) = 10;
  
  cout << get<2>(t4) << endl;   // 10
}
```

```cpp
#include <iostream>
using namespace std;

template<typename ... Types> struct xtuple
{
  static constexpr int N = 0;
};

int main()
{
  xtuple<> t0;
  xtuple<int> t1(1);
  xtuple<int, double, int, char> t4(1, 3.4, 2, 'A');
  // 여기까지하면 빌드는 되지만 값은 보관하지 못한다.
}
```

```cpp
template<typename ... Types> struct xtuple
{
  static constexpr int N = 0;
};

template<typename T, typename ... Types> 
struct xtuple<T, Types...>
{
  T value;
  
  xtuple() = defualt;
  xtuple(const T& v) : value(v) {}
  
  static constexpr int N = 1;
  
  // 여기까지하면 1개의 인자를 보관할 수 있다.
};
```

```cpp
template<typename ... Types> struct xtuple
{
  static constexpr int N = 0;
};

template<typename T, typename ... Types> 
struct xtuple<T, Types...> : public xtuple<Types...>
{
  // 상속처리만으로 모든 인자를 보관 할 수 있다.
  T value;
  
  xtuple() = defualt;
  xtuple(const T& v, const Types& ... args) : value(v), xtuple<Types...>(args...) {}
  
  static constexpr int N = xtuple<Types...>::N + 1;
};

int main()
{
  xtuple<int, double, int, char> t4(1, 3.4, 2, 'A');
  // xtuple<  double, int, char> t4(1, 3.4, 2, 'A');
  // xtuple<          int, char> t4(1, 3.4, 2, 'A');
  // xtuple<                char> t4(1, 3.4, 2, 'A');
}
```

![](/file/image/template-tuple.png)

---

## get 구현

```cpp
#include <iostream>
using namespace std;

struct Base
{
  int value = 10;
};

struct Derived : public Base
{
 int value = 20;
};

int main()
{
  Derived d;
  cout << d.value << endl;    // 20 - Derived값이 나오게 된다.
  cout << d.Base::value << endl;  // 10
  cout << static_cast<Base>(d).value << endl;   // 10
  cout << static_cast<Base&>(d).value << endl;  // 임시객체를 생성하기 싫다면 이렇게 쓰자
  
  static_cast<Base>(d).value = 30;    // 임시객체에 값을 넣기에 error
  static_cast<Base&>(d).value = 30;   // okay
}
```

```cpp
// 우선 기존에 만들었던 tuple을 보자.
template<typename ... Types> struct xtuple
{
  static constexpr int N = 0;
};

template<typename T, typename ... Types> 
struct xtuple<T, Types...> : public xtuple<Types...>
{
  // 상속처리만으로 모든 인자를 보관 할 수 있다.
  T value;
  
  xtuple() = defualt;
  xtuple(const T& v, const Types& ... args) : value(v), xtuple<Types...>(args...) {}
  
  static constexpr int N = xtuple<Types...>::N + 1;
};

int main()
{
  xtuple<int, double, int, char> t4(1, 3.4, 2, 'A');
  
  cout << t3.value << endl;   // 1
  cout << static_cast<xtuple<double, char>&(t3).value << endl;   // 3.4
  cout << static_cast<xtuple<char>&(t3).value << endl;   // A
  
  // get을 어떻게 만들지 대충 감이 오는가?
}
```

```cpp
template<size_t N, typename TP>
튜플TP의 N번째 요소의 타입&
xget(TP& tp)
{
  return static_cast<TP의 N번째 기반클래스&>(tp).value;
}

// 요렇게 만들면 된다.
// 말은 쉽군
```

```cpp
template<size_t N, typename TP>
struct xtuple_element;

// 요소의 타입을 구할수 있도로 부분 특수화
// N == 0
template<typename T, typename ... Types>
struct xtuple_element<0, xtuple<T, Types...>>
{
  typedef T type;
};

// N != 0
template<size_t N, typename T, typename ... Types>
struct xtuple_element<N, xtuple<T, Types...>>
{
  typedef typename xtuple_element<N-1, xtuple<Types...>>::type type;
};

int main()
{
  xtuple_element<1, tuple<int, double, char>>::type n;
  cout << typeid(n).name() << endl;
}
```

```cpp
// 이제 get을 만들어 보자.
template<size_t N, typename TP>
struct xtuple_element;

template<typename T, typename ... Types>
struct xtuple_element<0, xtuple<T, Types...>>
{
  typedef T type;   // 0번째 요소의 타입
  typedef tupleType xtuple<T, Types...>;  // 0번째 요소를 저장하는 타입
};

template<size_t N, typename T, typename ... Types>
struct xtuple_element<N, xtuple<T, Types...>>
{
  typedef typename xtuple_element<N-1, xtuple<Types...>>::type type;
  typedef typename xtuple_element<N-1, xtuple<Types...>>::tupleType tupleType;
};

template<size_t N, typename TP>
xtuple_element<N, TP>::type& xget(TP& tp)
{
  return static_cast<typename xtuple_element<N, TP>::tupleType&>(tp).value;
}

int main()
{
  xtuple_element<1, tuple<int, double, char>>::type n;
  xtuple_element<1, tuple<int, double, char>>::tupleType tp;
  cout << typeid(n).name() << endl;
  
  xget<1>(t3) = 1.1;
}
```