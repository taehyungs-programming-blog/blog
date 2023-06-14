---
title: "(C++) 임시객체와 멤버함수"
date: 2020-02-08 00:00:00 -0000
---

```cpp
using namespace std;

class Test
{
public:
    int data;

    // 요건뭐지?? -> lvalue 객체에 대해서만 호출
    void foo() & { cout << "foo - lvalue" << endl; }

    // rvalue 객체에 대해서만 호출
    void foo() && { cout << "foo - rvalue" << endl; }
};

int main()
{
    Test t;
    t.foo();            // foo - lvalue

    Test().foo();       // foo - rvalue
}
```

