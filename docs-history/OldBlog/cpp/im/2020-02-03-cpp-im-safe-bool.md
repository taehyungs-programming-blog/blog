---
title: "(C++) safe bool"
date: 2020-02-03 00:00:00 -0000
---

```cpp
#include <iostream>
using namespace std;

int main()
{
  int n = 0;
  
  cin >> n;   // 'a'라는 문자열을 입력받는다면?
  
  if( cin.fail() )
    cout << "실패" endl;
    
  if( cin )   // 그런데 cin이라는 객체가 어떻게 bool이 될 수 있을까?? -> 어떤식으로든 변환이 되고있다는말
  // cin.operator void*()   => C++98/03
  // cin.operator bool()    => C++11
    cout << "성공" << endl;
}
```

> 그런데 처음부터 `cin.operator bool()`로 만들지 왜 `cin.operator void*()`로 만들었을까?

---

```cpp
class istream
{
public:
  bool fail() { return false; }
  
  // 방법 1. bool로 변환
  operator bool() { return fail() ? false : true; }
};
istream cin;

int main()
{
  if( cin.fail() ) {}
  // if ( cin )으로 쓰고싶다!
  
  cin << n;   // ok - bool안에 정수가 들어가기에 shift연산이 되어버린다.
}
```

> `void*`로 변환하자

```cpp
class istream
{
public:
  bool fail() { return false; }
  
  // 방법 2. void*로 변환
  operator void*() { return fail() ? 0 : this; }
};
istream cin;

int main()
{
  int n = 0;
  if ( cin ) {}
  
  cin << n; // error
  // 단,
  delete cin;   // ok -> ???
}
```

> 좀 더 안전하게 만들어보자.

```cpp
void true_function();

class istream
{
public:
  bool fail() { return false; }
  
  // 방법 3. 함수 포인터로 변환
  typedef void(*F)();
  operator F() { return fail() ? 0 : &true_function; }
};
istream cin;

int main()
{
  int n = 0;
  if ( cin ) {}
  
  cin << n;     // error
  delete cin;   // error
  
  // 단,
  void(*f)() = cin;   // ok -> ???
}
```

> 좀 더 안전하게

```cpp
class istream
{
public:
  bool fail() { return false; }
  
  // 방법 4. 멤버 함수 포인터로의 변환
  struct Dummy
  {
    void true_function() {}
  };
  typedef void(Dummy::*F)();
  operator F() { return fail() ? 0 : &Dummy::true_function; }
};

int main()
{
  int n = 0;
  if ( cin ) {}
  
  cin << n;           // error
  delete cin;         // error
  void(*f)() = cin;   // error
}
```

> side effect가 가장 적도록 생성가능! → **Safe bool**이라 한다.