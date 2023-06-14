---
title: "STL : dispatching"
permalink: cpp/stl/dispatching/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-04 00:00:00 -0000
last_modified_at: 2020-04-04 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

## Lecture Note

* [Lecture Note : advance 함수 써보기](https://ideone.com/b5S4Ae)
* [Lecture Note : advance 만들어 보기](https://ideone.com/0NSBdJ)

---

## advance 함수 써보기

```cpp
#include <iostream>
#include <vector>
#include <list>
#include <iterator>
using namespace std;

int main()
{
  list<int> s = {1,2,3,4,5,6,7,8,9,10};
  auto p = begin(s);

  advance(p, 5);    // p+5를 의미한다.
  cout << *p << endl;
}
```

---

## advance 만들어 보기

이 advance를 만들어보자.

```cpp
template<typename T>
void eadvance(T& p, int n)
{
  while(n--)
    ++p;      // 이런식으로 모든 컨테이너에 적용이 가능하지만, 
              // 만들면 성능의 저하가 크다
}
```

tag dispatching을 써서 만들어 보자.

```cpp
struct input_iterator_tag {};   // 아무런 멤버가 없는 empty class
struct output_iterator_tag {};

struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};
```

위를 카테고리 테그라 하며 algorhtim에 정의 되어 있다.

```cpp
template<typename T> class vector_iterator
{
public:
  using iterator_categoty = random_access_iterator_tag;
};

template<typename T> class list_iterator
{
public:
  using iterator_categoty = bidirectional_iterator_tag;
};
```

어떤 종류의 반복자인지 알 수 있다.

```cpp
template<typename T>
void eadvance_imp(T& p, int n, random_access_iterator_tag)
{
  cout << "rand ver." << endl;
  p = p + n;
}

template<typename T>
void eadvance_imp(T& p, int n, input_iterator_tag)
{
  cout << "input ver." << endl;
  while(n--) ++p;
}

template<typename T>
void eadvance(T& p, int n)
{
  // 반복자의 종류에 따라
  eadvance_imp(p, n, typename T::iterator_category());
}
```

모든 iterator에는 tag를 넣어서 자신이 어떤 타입인지 알려준다.

---

## 

```cpp
template<typename T>
void eadvance_imp(T& p, int n, random_access_iterator_tag)
{
  cout << "rand ver." << endl;
  p = p + n;
}

template<typename T>
void eadvance_imp(T& p, int n, input_iterator_tag)
{
  cout << "input ver." << endl;
  while(n--) ++p;
}

template<typename T>
void eadvance(T& p, int n)
{
  // dispathcing 방벙 1 - 함수 인자를 이용한 오버로딩
  eadvance_imp(p, n, typename T::iterator_category());

  // dispathcing 방벙 2 - if문 사용(C++17)
  if constexpr (is_same<typename T::iterator_category, random_access_iterator_tag>::value)
  {
    p = p + n;
  }
  else
    while(n--) ++p;
}

// dispathcing 방벙 3 - enable_if 사용 : SFINAE 사용
template<typename T>
typename enable_if<is_same<typename T::iterator_category, random_access_iterator_tag>::value>::type eadvance(T& p, int n)
{
  p = p + n;
}

template<typename T>
typename enable_if<is_same<typename T::iterator_category, input_iterator_tag>::value>::type eadvance(T& p, int n)
{
  while(n--) ++p;
}
```