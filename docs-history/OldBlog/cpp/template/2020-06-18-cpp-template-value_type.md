---
title: "C++ Template : value_type"
permalink: cpp/template/value_type/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-25 00:00:00 -0000
last_modified_at: 2020-06-18 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
header:
  teaser: /file/image/cpp-page-teaser.gif
tag:
  - C++
  - Template
category:
  - 주의사항
excerpt: ""
---

```cpp
void print_first_element(vector<int>& v)
{
  int n = v.front();
  cout << n << endl;
}

int main()
{
// vector<int> v = { 1, 2, 3 };
  vector<double> v = { 1, 2, 3 };   // error 
  print_first_element(v);// - 모든 자료형을 받을 수 있게 만들어보자.
}
```

```cpp
template<typename T>
void print_first_element(vector<T>& v)
{
  T n = v.front();
  cout << n << endl;
}

int main()
{
  // vector<int> v = { 1, 2, 3 };
  // vector<double> v = { 1, 2, 3 };
  list<int> v = { 1, 2, 3 };    // error
  print_first_element(v);       // 모든 타입의 컨테이너를 받을 수 있게 만들어 보자
}
```

```cpp
template<typename T>
void print_first_element(T& v)
{
  typename T::value_type n = v.front();   // typename을 적는 이유는 컴파일러가 값으로 해석할 수 있기 때문이다.
  auto n = v.front();   // C++11 이후에는 이게 가능.
  cout << n << endl;
}

int main()
{
  // vector<int> v = { 1, 2, 3 };
  // vector<double> v = { 1, 2, 3 };
  list<int> v = { 1, 2, 3 };    
  print_first_element(v);       
}
```

```cpp
template<typename T> class Vector
{
  T* buff;
  int size;
public:
  Vector(int sz, T value) {}
};

int main()
{
  Vector v(10, 3);
  list<int> s = {1,2,3};
  
  Vector v2(s);   // 이게 가능할까?
}
```

```cpp
template<typename T> class Vector
{
  T* buff;
  int size;
public:
  Vector(int sz, T value) {}
  template<typename C> Vector(C c) {}
};

template<typename C> 
Vector(C c)->Vector<typename C::value_type>;
// 복사 생성자가 호출될 경우 이렇게 선언해 달라 컴파일러에게 전달(C++17기능)
```