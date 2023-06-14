---
title: "(C++) explicit"
date: 2020-02-02 00:00:00 -0000
---

```cpp
class Test
{
    int value;
public:
    Test(int n) : value(n) {}
};

int main()
{
    // 아래 2줄의 차이점은?
    Test t1(5);     // 인자가 하나인 생성자 호출(direct initialization)
    Test t2 = 5;    // 1. 변환 생성자를 사용해서 5르 Test의 임시객체로 생성
                    // 2. 임시객체를 복사 생성자를 사용해서 t2에 복사(copy initialization)
}
```

```cpp
class Test
{
    int value;
public:
    explicit Test(int n) : value(n) {}
};

Test t2 = 5;    // error!
```

---

```cpp
#include <string>
#include <memory>
using namespace std;

int main()
{
    // STL의 string 클래스는 생성자가 explicit이 아님
    string s1("hello");
    string s2 = "hello";

    // shared_ptr은 epxlicit
    shared_ptr<int> p1 = new int;       // error
    shared_ptr<int> p2(new int);        // ok
}
```