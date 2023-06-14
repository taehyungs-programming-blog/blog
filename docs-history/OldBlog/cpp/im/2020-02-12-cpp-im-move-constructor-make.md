---
title: "(C++) move와 const object"
date: 2020-02-12 00:00:00 -0000
---

```cpp
#include <iostream>
#include <string>
#include "Test.h"
using namespace std;

class Buffer
{
  size_t sz;
  int* buf;
  string tag;
  Test test;
public:
  Buffer(size_t s, string t) : sz(s), tag(t), buf(new int[s]) {}
  ~Buffer() { ~delete[] buf; }
  
  // 깊은 복사
  Buffer(const Buffer& b) : sz(b.sz), tag(b.tag), test(b.test)
  {
    buf = new int[sz];
    memcpy(buf, b.buf, sizeof(int)*sz)
  }
};

int main()
{
  Buffer b1(1024, "SHARED");
  Buffer b2 = b1;   // copy
  
  Buffer b2 = move(b1);   // move 이걸 하고 싶다
}
```

```cpp
Buffer(Buffer&& b) : sz(b.sz), tag(b.tag), test(b.test)
{
    buf = b.buf;
    b.buf = 0; // 자원 포기

    // 그런데 Test내부는?? move해야하잖아?
}
```

```cpp
// move 생성자는 모든 멤버를 move로 옮기도록 작성하자
Buffer(Buffer&& b) noexcept
: sz(move(b.sz), tag(move(b.tag)), test(move(b.test))
{
    buf = b.buf;
    b.buf = 0; // 자원 포기
}
```

---

```cpp
class Buffer
{
  size_t sz;
//  int* buf;
  string tag;
  Test test;
public:
  Buffer(size_t s, string t) : sz(s), tag(t), buf(new int[s]) {}
  ~Buffer() { ~delete[] buf; }
  
  // 얕은 복사
  // 사용자가 만들지 않으면 컴파일러가 아래모양대로 제공
  Buffer(const Buffer& b) : sz(b.sz), tag(b.tag), test(b.test)
  {
  }
  
  // 이거도 컴파일러가 만들어줄까? -> okay! 기본적으로 제공한다.
  Buffer(Buffer&& b) noexcept
  : sz(move(b.sz), tag(move(b.tag)), test(move(b.test))
  {
  }
  
  // 그래도 만들어주는게 확실함...
};
```