---
title: "(C++) ranged-for"
date: 2020-02-03 00:00:00 -0000
---

```cpp
#include <iostream>
#include <list>
using namespace std;

int main()
{
    // int x[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };       // ok
    list<int> s = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };        // ok

    for(auto& n : x)
        cout << n << endl;

    /*
    for(int i = 0; i < 10; i++)
        cout << x[i] << endl;
    */
}
```

---

## 사용자 지정 

```cpp
int main()
{
    // int x[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };       // ok
    list<int> s = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };        // ok

    for(auto& n : s)
        cout << n << endl;

    /* 위와 동일한 표현이다.
    for(auto p = being(s); p != end(s); ++p)
        auto& n = *p;
        cout << n << endl;
    */
}
```

```cpp
struct Point3D
{
    int x = 1;
    int y = 2;
    int z = 3;
};

int main()
{
    Point3D p3;

    for( auto& n : p3 )         // erro! - being, end를 부를 수 없음.
        cout << n << endl;      
}
```

```cpp
struct Point3D
{
    int x = 1;
    int y = 2;
    int z = 3;
};
int* being(Point3D& p3) { return &(p3.x); }
int* end(Point3D& p3) { return&(p3.z)+1; }
```