---
title: "C++ Template : type deduction"
permalink: cpp/template/typededuction/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-21 00:00:00 -0000
last_modified_at: 2020-06-17 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
header:
  teaser: /file/image/cpp-page-teaser.gif
tag:
  - C++
category:
  - template
  - type deduction
excerpt: ""
---

```cpp
template<typename T> void foo(T a)
{
  ++a;
}

int main()
{
  int n = 0;
  int& r = n;
  const int c = n;
  
  foo(n);   // T : int
  foo(c);   // T : int
  foo(r);   // T : int
  
  // 뭐지??
  // 만약 들어오는 type에 의해서 T를 변경하게 된다면 더 혼란스러운 상황이 온다
  // 예를들어 const int 인 c를 T에서 const int로 그대로 받는다면
  // template자체에서 에러가 발생해야 하는데 ... 이런 예외상황까지 모두 개발자가 고려해야 한다는 말...
  // 이래서 차라리 원래 자료형으로만 받기로 한다고 정의
}
```

```cpp
// template에서 type deduction하는 규칙
template<typename T> void foo(T a)
{
  cout << type_id_with_cvr<T>().pretty_name() << endl;
}

int main()
{
  int n = 0;
  int& r = n;
  const int c = n;
  const int& cr = c;
  
  foo(n);   // T : int
  foo(c);   // T : int
  foo(r);   // T : int
  foo(cr);  // T : int
  
  const char* s1 = "hello";
  foo(s1);    // T : char const*
  
  const char* const s2 = "hello";
  foo(s2);    // T : char const *
  
  // ??? 이건 또 왜 그렇지 ???
  // const와 포인터의 위치가 헷갈려서 그럼
  // const int * p; -> 가리키는 포인터를 const해라
  // *p = 10; -> error
  // p = &n; -> okay
  // int * const p; -> 주소값 p를 const해라
  // *p = 10; -> okay
  // p = &n; -> error
  // 따라서 const를 지운다면 자신의 const를 지우게 되니 * 기준 왼쪽 const는 살아남는다
}
```

```cpp
template<typename T> void foo(T& a)   // 참조로 선언
{
  cout << type_id_with_cvr<T>().pretty_name() << endl;
  cout << type_id_with_cvr<decltype(a)>().pretty_name() << endl;
}

int main()
{
  int n = 0;
  int& r = n;
  const int c = n;
  const int& cr = c;
  
  foo(n);   // T : int, a : int&
  foo(c);   // T : const int, a : const int&    
  // T가 const int로 나오는 이유는 그냥 int로 나오게 되면
  // template내부에서 변경이 가능하기 때문이다.
  // 따라서 a도 역시 const int&가 된다.
  foo(r);   // T : int, a : int&
  foo(cr);  // T : const int, a : const int&
```

```cpp
template<typename T> void foo(const T& a)   // 참조로 선언
{
  cout << type_id_with_cvr<T>().pretty_name() << endl;
  cout << type_id_with_cvr<decltype(a)>().pretty_name() << endl;
}

int main()
{
  int n = 0;
  int& r = n;
  const int c = n;
  const int& cr = c;
  
  foo(n);   // T : int, a : const int&
  foo(c);   // T : int, a : const int&    
  foo(r);   // T : int, a : const int&
  foo(cr);  // T : int, a : const int&
}
```