---
title: "STL : insert-iterator"
permalink: cpp/stl/insert-iterator/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-06 00:00:00 -0000
last_modified_at: 2020-04-06 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

## Lecture Note

* [Lecture Note : back_insert_iterator](https://ideone.com/5l3OyW)
* [Lecture Note : back_insert_iterator - exmaple](https://ideone.com/2sU7P8)
* [Lecture Note : back_insert_iterator 간결하게 쓰기](https://ideone.com/gUd0tw) : `back_inserter`
* [Lecture Note : front_inserter](https://ideone.com/iMhCF0)
* [Lecture Note : inserter](https://ideone.com/CjTy1w)
* [Lecture Note : 구현원리 - 1](https://ideone.com/UNTQST)
* [Lecture Note : 구현원리 - 2](https://ideone.com/c0du8A) : 빌드안됨.

---

## back_insert_iterator

```cpp
#include <iostream>
#include <list>
using namespace std;

int main(int argc, char** argv)
{
    list<int> s = { 1, 2, 3, 4, 5 };

    s.push_back(10);        // 이거말고 삽입할 방법이 없나?

    for( auto& n : s )
        cout << n << endl;
}
```

```cpp
back_insert_iterator<list<int>> p(s);
*p = 20;
```

---

## back_insert_iterator - exmaple

근데 이걸 왜 쓰나??

```cpp
int x[5] = {10,20,30,40,50};
// 이걸 다 넣고 싶을때... for문을 만들까??

back_insert_iterator<list<int>> p(s);
copy(x, x+5, p);
```

---

## back_insert_iterator 간결하게 쓰기

```cpp
#include <iostream>
#include <list>
using namespace std;
 
int main() {
    int x[5] = {10,20,30,40,50};
	list<int> s = { 1, 2, 3, 4, 5 };
 
    copy(x, x+5, s.begin());        // 이렇게 쓰면 덮어 쓰게 된다.
    back_insert_iterator<list<int>> p(s);       // 그런데 list<int>를 아는데 꼭 써야하나??
    copy(x, x+5, p);

 
	for( auto& n : s )
        cout << n << endl;
 
	return 0;
}
```

```cpp
#include <iostream>
#include <list>
using namespace std;

template<typename T>
back_insert_iterator<T>
back_insert(T& c)
{
    return back_insert_iterator<T>(c);
}

int main() {
    int x[5] = {10,20,30,40,50};
	list<int> s = { 1, 2, 3, 4, 5 };
 
    copy(x, x+5, back_insert(s));
 
	for( auto& n : s )
        cout << n << endl;
 
	return 0;
}
```

사실 back_insert를 STL에서 제공한다. -> `back_inserter`

```cpp
int main() {
    int x[5] = {10,20,30,40,50};
	list<int> s = { 1, 2, 3, 4, 5 };
 
    copy(x, x+5, back_inserter(s));
 
	for( auto& n : s )
        cout << n << endl;
 
	return 0;
}
```

---

## front_inserter

```cpp
#include <iostream>
#include <list>
using namespace std;

int main() {
    int x[5] = {1,2,3,4,5};
	list<int> s = { 0, 0, 0, 0, 0 };
 
    copy(x, x+5, front_inserter(s));
 
	for( auto& n : s )
        cout << n << endl;
 
	return 0;
}
```

* 저장되는 순서가 5 4 3 2 1 0 0 0 0 0임을 주의하자.
* 생각해 보면 알겠지만 vector는 front_inserter가 없다

---

## inserter

```cpp
#include <iostream>
#include <list>
using namespace std;

int main() {
    int x[5] = {1,2,3,4,5};
	list<int> s = { 0, 0, 0, 0, 0 };
 
    copy(x, x+5, inserter(s, ++s.begin()));
 
	for( auto& n : s )
        cout << n << endl;
 
	return 0;
}
```

---

## 구현원리 - 1

```cpp
#include <iostream>
#include <list>
using namespace std;

int main() {
    list<int> s = { 1, 2 };

    eback_insert_iterator<list<int>> p(s);      // 만들어 보자.

    *p = 30;
}
```

```cpp
template<typename C>
class eback_insert_iterator
{
    C* container;
public:
    eback_insert_iterator(C& c) : container(&c) {}

    eback_insert_iterator& operator*() { return *this; }
    eback_insert_iterator& operator=(const typename C::value_type& a)
    {
        container->push_back(a);
        return *this;
    }
};

int main() {
    list<int> s = { 1, 2 };

    eback_insert_iterator<list<int>> p(s);

    *p = 30;        // (p.operator*()).operator=(30)-> operator*, operator= 구현필요

    for(auto& n : s)
        cout << n << " ,"; 
}
```

---

## 구현원리 - 2

```cpp
#include <iostream>
#include <list>
using namespace std;

template<typename C>
class eback_insert_iterator
{
    C* container;
public:
    using iterator_category = output_iterator_tag;
    using value_type = void;
    using pointer = void;
    using reference = void;
    using difference = void;
    using container_type = C;

    eback_insert_iterator(C& c) : container(std::addressof(c)) {}

    eback_insert_iterator& operator*() { return *this; }
    eback_insert_iterator& operator++() { return *this; }           // 전위형
    eback_insert_iterator& operator++(int) { return *this; }        // 후위형
    eback_insert_iterator& operator=(const typename C::value_type& a)   // l-value
    {
        container->push_back(a);
        return *this;
    }

    eback_insert_iterator& operator=(const typename C::value_type&& a)  // r-value
    {
        container->push_back(move(a));
        return *this;
    }
};

int main() {
    list<int> s = { 1, 2 };

    eback_insert_iterator<list<int>> p(s);

    *p = 30;

    int x[2] = {1, 2};
    copy(x, x+2, p);        // copy가 가능하게 p++이 가능하게 만들어보자.

    for(auto& n : s)
        cout << n << " ,"; 
}
```