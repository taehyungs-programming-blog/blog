---
title: "(C++) initializer_list"
date: 2020-02-05 00:00:00 -0000
---

```cpp
#include <iostream>
#include <initializer_list>
using namespace std;

int main()
{
  // brase-init-list
  initializer_list<int> s = { 1, 2, 3, 4, 5 };
  
  auto p = begin(s);    // 상수를 가리키는 반복자
  *p = 20;    // error
  
  cout << *p << endl;   // read는 ok
}
```

### Initializer_list
* brace-init-list로 초기화된 요소의 시작과 끝(개수)를 가리키는 클래스
* 반복자는 상수를 가리킨다.(변경불가)
* (결론) read-only 메모리에 기록되며 컴파일러는 시작과 끝을 가리키기에 읽기만 한다.

 ```cpp
#include <iostream>
#include <initializer_list>
using namespace std;

void foo(initializer_list<int> e)
{
  auto p1 = begin(e);
  auto p2 = end(e);
  
  for(auto n : e)
    cout << n << endl;
}

int main()
{
  initializer_list<int> e1 = { 1, 2, 3 };
  
  foo(e1);
  foo({ 1, 2, 3, 4 });    // 모두 가능
}
 ```

 > 어떤 함수가 `initializer_list`를 쓰고있는다면 어떠한 길이의 list도 보낼 수 있다.

 ```cpp
// 클래스의 생성자 인자로 사용할때
class Point
{
  int x, y;
public:
  Point(int a, int b)   { cout << "int, int" << endl; }           // 1번
  Point(initializer_list<int>) { cout << "initializer_list<int>" << endl; } // 2번
};

int main()
{
  Point p1(1, 1);         // 1번 우선, 없다면 error
  
  Point p2( { 1, 1 } );   // 2번 우선, 없다면 {1,1}가 임시객체가 되어 복사생성자로 복사됨.
  
  Point p3{1, 1};         // 핵심, 2번 우선, 없다면, 1번 우선(위와 같은 경로)
  
  Point p4 = {1, 1};         // 2번 우선, 없다면 1번
  
  Point p5(1, 1, 3);          // error
  
  Point p6{1, 1, 3};          // 2번
  
  Point p7 = {1, 1, 3};       // 2번
}
 ```

 ```cpp
// STL컨테이너와 initializer_list
#include <iostream>
#include <vector>
using namespace std;

template<typename T, typename Ax = allocator<T>> class vector
{
  T* buff;
public:
  vector<size_t sz, T v = T()) {}   // 1
  vector<initizlizer_list<T> s) {}  // 2
};

int main()
{
  vector<int> v = { 1, 2, 3, 4, 5 };    // ok
  
  vector<int> v1(10, 3);        // 1번, 크기와 초기값 을 받는다.
  vector<int> v2{10, 3};        // 2번, 2개의 요소를 10, 3으로 초기화
}
 ```