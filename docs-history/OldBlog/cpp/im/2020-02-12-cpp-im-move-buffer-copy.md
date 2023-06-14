---
title: "(C++) move를 활용한 버퍼 복사"
date: 2020-02-12 00:00:00 -0000
---

```cpp
#include <iostream>
#include <vector>
#include <type_traits>
#include "Test.h"
using namespace std;

int main()
{
  Test* p1 = new Test[2];
  Test* p1 = static_cast<Test*>(operator new(sizeof(Test)*2));  // Test에 디폴트 생성자가 없다면 이런 선언도 좋다.
  
  for(int i = 0; i < 2; i++)
    new(&p1[i]) Test;
  
  Test* p2 = new Test[4];   // 얘는 복사 받을 버퍼인데 디폴트 생성자로 메모리 초기화를 해줘야하나? -> 이것 또한 오버헤드아닌가? -> 메모리만 있으면 될 거 같다!
  // 1. 신규 버퍼는 메모리만 할당하자.
  Test* p2 = static_cast<Test*>(operator new(sizeof(Test)*4));
  
  for(int i = 0; i < 2; i++)
    p2[i] = move(p1[i]);    // move 대입 호출, 위에서 생성자를 호출하지 않았기에 
    new(&p2[i]) Test(move(p1[i]));  // move 생성자를 호출하면 된다.
    new(&p2[i]) Test(move_if_noexcept(p1[i]));    // 더 좋은 코드!
    
  // 2. 새로운 객체는 디폴트 생성자 호출
  for(int i = 2; i < 4; i++)
    new(&p2[i]) Test;   // 디폴트 생성자 호출
    
    
  // 버퍼 파괴
  for(int i = 1; i>=0;i--)
    p1[i].~Test();
  operator delete(p1);
  
  for(int i = 3; i>=0;i--)
    p2[i].~Test();
  operator delete(p2);
}
```

```cpp
// STL vector를 쓴다면?
vector<Test> v(2);
v.resize(4);
// 위와 동일한 작업을 수행하게 된다.
```