---
title: "(C++) this와 상속"
date: 2020-01-28 00:00:00 -0000
---

## 다중 상속과 this 의 문제

* 기억해야할 점은 멤버함수 포인터의 값은 무조건 4바이트가 아닐 수 있다는 점이다.

```cpp
class A { int a; };
class B { int b; };

class C : public A, public B
{
  int c;
};

int main()
{
  C obj;
  
  A* pA = &obj;
  B* pB = &obj;
  
  cout << &obj << endl;   // 주소를 1000이라 가정을 해보자.
  // pA, pB 모두 1000일까?
  cout << pA << endl;     // 1000
  cout << pB << endl;     // 1004
  // 다중상속이고 두 번째 기반클래스라면
  // obj + sizeof(A)의 주소가 나온다.
}
```

```cpp
class A
{
  int a;
public:
  void fa() { cout << this << endl; }
};

class B
{
  int b;
public:
  void fb() { cout << this << endl; }
};

class C : public A, public B
{
  int c;
};

int main()
{
  C obj;
  cout << &obj << endl; // 역시 1000이라 가정하자.
  
  // 같은 this를 호출하는데... 같은 값이 나오나?
  obj.fa();     // 1000
  obj.fb();     // b입장에서는 1004가 this이다.
  // fb(&obj + sizeof(A)) 가 this call로 넘어감을 기억하자.
}
```

---

## 함수포인터와 this call

```cpp
class A
{
  int a;
public:
  void fa() { cout << this << endl; }
};

class B
{
  int b;
public:
  void fb() { cout << this << endl; }
};

class C : public A, public B
{
  int c;
};

int main()
{
  C obj;
  cout << &obj << endl; // 역시 1000이라 가정하자.
  
  obj.fa();     
  obj.fb();     
  
  void (C::*f)();   // 멤버함수 포인터
  f = &C::fa;
  (obj.*f)();   // 1000
  
  f = &C::fb;
  (obj.*f)();   // ????
  // 이게 문제가 f를 컴파일 순간에 fa, fb중 어디를 가르킬지 모른다는데 있다.
  // 그럼 런타임에 결정?? -> 그건말이 안되고...
  
  // 다중 상속의 멤버 함수 포인터를 생성 시 
  // 멤버 함수 포인터는 함수의 주소만 갖는 것이 아니라 this offset까지 갖게 된다.
  void (C::*f)();
  cout << sizeof(f) << endl;
  // 32bit 환경에서 8 이 나온다.
  
  f = &C::fa;   // f = { fa의 주소 , offset 0 }
  (obj.*f)();   // 1000 (=f.함수주소(&obj + f.this offset))
  
  f = &C::fb;   // f = { fb의 주소, offset 4(sizeof(A)) }
  (obj.*f)();   // 1004 (=f.함수주소(&obj + f.this offset))
}
```