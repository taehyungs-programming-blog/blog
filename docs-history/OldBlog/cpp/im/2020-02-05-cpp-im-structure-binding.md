---
title: "(C++) inherit constructor"
date: 2020-02-05 00:00:00 -0000
---

## inherit constructor

```cpp
#include <iostream>
#include <tuple>
using namespace std;

struct Point
{
  int x;
  int y;
};

int main()
{
  Point pt = { 1, 2 };
  
  auto [a, b] = pt;
  auto& [rx, ry] = pt;
  // 이런식으로 요소를 꺼내올 수 있다! -> 정말편하군!
  
  int x[2] = { 1, 2 };
  auto[e1, e2] = x;
  
  pair<int, double> p(1, 3.4);
  auto[n, d] = p;
  
  tuple<int, short, double> t3(1, 2, 3.4);
  auto[a1, a2, a3] = t3;
}
```

---

## 활용

```cpp
#include <iostream>
#include <set>
using namespace std;

int main()
{
  set<int> s;
  
  s.insert(10);
  s.insert(10);   // 중복을 허용하지 않기에 fail!
  pair<set<int>::iterator, bool> ret = s.insert(10);
  
  if(ret.second == false)
  {
    cout << "실패" << endl;
  }
}
```

```cpp
int main()
{
  set<int> s;
  
  s.insert(10);
  auto[it, success] = s.insert(10);
  
  if(success == false) {
    cout << "실패" << endl;
  }
```

```cpp
int main()
{
  set<int> s;
  
  s.insert(10);
  if(auto[it, success] = s.insert(10); success == false)
  {
    cout << "실패" << endl;
  }
```