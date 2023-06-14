---
title: "(C++) setter using move & copy"
date: 2020-02-18 00:00:00 -0000
---

```cpp
// move, copy를 모두 지원하는 setter를 만들어보자.

#include <iostream>
using namespace std;

class Data
{
public:
  Data() {}
  ~Data() {}
  
  Data(const Data& t)
  {
    cout << "Copy" << endl;
  }
  Data(Data&& t) noexcept
  {
    cout << "Move" << endl;
  }
  Data& operator=(const Data& t)
  {
    cout << "Copy=" << endl;
    return *this;
  }
  Data& operator=(Data&& t) noexcept
  {
    cout << "Move=" << endl;
    return *this;
  }
};
```

```cpp
// 여기부터 시작

class Test
{
  Data data;
public:
  // void setData(Data d) { data = d; }
  
  // 아래 코드는 무조건 copy가 불린다.
  void setData(const Data& d) { data = d; }
};

int main()
{
  Test test;
  Data d;
  
  test.setData(d);        // 실행후에도 d사용가능
  test.setData(move(d));  // 실행후에는 d사용불가 -> 이걸만들어보자.
}
```

```cpp
// 이렇게 바꾸면 무조건 move가 불리나? -> Nope copy가 불림
// 상수로(const Data& d) 받아지기 때문
void setData(const Data& d) { data = move(d); }
```

```cpp
// 해결해보자.
// 방법 1. move, copy setter를 별도로 만들자
// 오버헤드가 없지만, setter가 항상 2개 존재해야한다.
void setData(const Data& d) { data = d; }
void setData(Data&& d) { data = move(d); }    // 요걸만들자.

int main()
{
  Test test;
  
  Data d;
  test.setData(d);
  test.setData(move(d));
}
```

```cpp
// 방법 2. call by value로 받자
// setter를 하나만 만들어도 되지만, 약간의 오버헤드(move1회)가 있음(그런데 move는 오버헤드가 매우작다.)
void setData(Data d) { data = move(d); }

int main()
{
  Test test;
  
  Data d;
  test.setData(d);      // copy 생성, move 호출
  test.setData(move(d));// move 생성, move 호출
}
```

```cpp
// 방법 3. forwarding reference
// 오버헤드가 없고 하나의 함수 템플릿만 제공하면 된다.(하나의 setter면 된다)
// 단 side effect가 발생하는데 방지법은 다음강에서 설명
template<typename T> void setData(T&& a)
{
  // data = a;          // copy
  // data = move(a);    // move
  data = std::forward<T>(a);    // 이전단계를 확인해서 할당해달라
}
```