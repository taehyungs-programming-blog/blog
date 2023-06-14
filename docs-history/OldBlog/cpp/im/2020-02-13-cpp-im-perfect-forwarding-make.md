---
title: "(C++) perfect forwarding 구현"
date: 2020-02-13 00:00:00 -0000
---

```cpp
void goo(int& a) { cout << "goo" << endl; a = 30; }
void foo(int a) { cout << "foo" << endl; }

// 잠시 두 번째 인자를 int로 받는다고 가정하자.
template<typename F>
void chronometry(F f, int& arg)     // 핵심 1. 함수 인자를 받을 때 reference를 사용해야한다.
{
  f(arg);
}

// 핵심 2. 인자로 rvalue가 들어올 수 있으니 함수를 하나 더 만든다.
template<typename F>
void chronometry(F f, int&& arg)
{
  f(arg);
}

int main()
{
  int n = 0;
  
  chronometry(&goo, n);
  chronometry(&foo, 5);
  
  cout << n << endl;
}

// 그러나... 아직 한 가지 문제가 남았다.
```

```cpp
void goo(int& a) { cout << "goo" << endl; a = 30; }
void foo(int a) { cout << "foo" << endl; }
void hoo(int&& a) { cout << "hoo" << endl; }

template<typename F> void chronometry(F f, int& arg)
{
  f(arg);
}

template<typename F> void chronometry(F f, int&& arg)
{
  f(arg);
}

int main()
{
  int n = 0;
  
  chronometry(&goo, n);
  chronometry(&foo, 5);
  chronometry(&hoo, 10);    // error! - 
  
  // 약간은 어려우나 이런 개념이라 보면 된다.
  int&& arg = 10;   // 10은 rvalue
                    // arg는 lvalue이다.
  // chronomertry에서도 10을 넘겨주나
  // hoo는 rvalue만 받을 수 있기에 에러가 발생하게 된다.
  
  cout << n << endl;
}
```

```cpp
template<typename F> void chronometry(F f, int&& arg)
{
  f(static_cast<int&&>(arg));   // 핵심 3. 강제로 캐스팅하는 방법으로 해결할 수 있다.
}
```