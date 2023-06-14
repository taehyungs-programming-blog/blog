---
title: "(C++) std::move"
date: 2020-02-11 00:00:00 -0000
---

```cpp
class Test
{
public:
  Test() {}
  ~Test() {}
  Test(const Test& t) { cout << "Copy" << endl; }
  Test(Test&& t) { cout << "Copy" << endl; }
};

int main()
{
  Test t1;
  Test t2 = t1;       // Copy
  Test t3 = Test();   // Move
  Test t4 = t1;       // Copy -> 그러런데 t1을 더 이상사용하지 않을 거라면?
  Test t4 = static_cast<Test&&>(t1);    // Move, 생성자 호출됨.
  Test t5 = move(t2);   // Move, 위와 동일한 동작
}
```

```cpp
class Test
{
public:
  Test() {}
  ~Test() {}
  Test(const Test& t) { cout << "Copy" << endl; }
  Test(Test&& t) { cout << "Copy" << endl; }
};

int main()
{
  Test t1;
  Test t2 = t1;   // 초기화, 복사 생성자 호출
  t2 = t1;        // 대입, 대입연산자 호출
  
  t2 = move(t1);   // 대입에도 move가 가능?
}
```

```cpp
Test& operator=(const Test& t) { return *this; }  // 복사 대입연산자
Test& operator=(Test&& t) { return *this; }  // move 대입연산자

// 우선은 복사 대입연산자 뿐만 아니라 move 대입연산자도 만들어야함을 기억하자.
```