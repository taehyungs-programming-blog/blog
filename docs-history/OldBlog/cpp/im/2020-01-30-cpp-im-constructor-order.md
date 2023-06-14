---
title: "(C++) 생성자 호출순서"
date: 2020-01-30 00:00:00 -0000
---

```cpp
class Base
{
public:
  Base() { cout << "B()" << endl; }
  Base(int a) { cout << "B(int)" << endl; }
  ~Base() { cout << "~B()" << endl; }
};

class Derived : public Base
{
public:
  Derived() { cout << "D()" << endl; }
  Derived(int a) { cout << "D(int)" << endl; }
  ~Derived() { cout << "~D()" << endl; }
};

int main()
{
  Derived d;
  // Base()
  // Derived()
  // ~Derived()
  // ~Base()
  // 파생 클래스 생성시 기반 클래스의 생성자가 먼저 호출된다.
  
  Derived d1(5);
  // Base()
  // Derived(int)
  // ~Derived()
  // ~Base()
  // 기반 클래스의 생성자는 항상 디폴트 생성자가 호출된다.
  
  // 만약 기반 클래스의 디폴드 생성자가 없다면 객체를 만들 수 없다.
}
```

> 좀 더 자세히 살펴보자.

```cpp
class Base
{
public:
  Base() { cout << "B()" << endl; }
  Base(int a) { cout << "B(int)" << endl; }
  ~Base() { cout << "~B()" << endl; }
};

class Derived : public Base
{
public:
  Derived() // : Base() - 컴파일러가 암묵적으로 넣어준다.
  { 
    cout << "D()" << endl; 
  }
  Derived(int a) : Base(a) // 만약 사용자가 다음과 같이 정의한다면 컴파일러는 별도로 생성하지 않는다.
  { 
    cout << "D(int)" << endl; 
  }
  ~Derived() { cout << "~D()" << endl; }
};

int main()
{
  Derived d(5);
}
```

```cpp
class Animal
{
protected:
  Animal() {}
};

class Dog : public Animal
{
public:
  Dog() : Animal() {}
};

int main()
{
  Animal a;   // error
  Dog d;      // ok
}
```

* protected constructor - 자신은 객체를 만들 수 없지만, 파생클래스의 객체는 만들 수 있게한다.

---

## 다시 한 번 복습!

```cpp
class Point
{
    int x, y;
public:
    Point() : x(0), y(0) {}
    Point(int a, int b) : x(a), y(b) {}
};

class Rect
{
    Point p1;
    Point p2;
public:
    Rect()      // 컴파일러에 의해서 : p1(), p2() 호출됨.
    {
    }
};

int main()
{
    Rect r;     // p1, p2 생성자 호출 후 Rect 생성자 호출
}
```

---

## 호출순서

```cpp
struct BM{ BM() { cout << "BM()" << endl; }};
struct DM{ DM() { cout << "DM()" << endl; }};

struct Base
{
    BM bm;
    int x;
    Base() { cout << "Base()" << endl; }
};

struct Derived : public Base
{
    DM dm;
    int y;
    Derived() { cout << "Derived()" << endl; }
};

int main()
{
    Derived d;
    // BM()
    // Base()
    // DM()
    // Derived()
    // 소멸은 반대로
}
```

---

> 이런 생성자 호출순서를 왜 알아야하는데?? -> 버그를 유발할 수 있다.

## 생성자 호출순서 예제

```cpp
struct stream_buf
{
    stream_buf(size_t sz)
    {
        cout << "stream_buf" << endl;
    }
};

struct stream
{
    stream(stream_buf& buf)
    {
        cout << "stream : using stream_buf" << endl;
    }
};

// 버퍼를 갖는 stream
struct mystream : public stream
{
    stream_buf buf;
public:
    mystream(int sz) : buf(sz), stream(buf) {}
};

int main()
{
    stream_buf buf(1024);
    stream st(buf);

    mystream mst(1024);         // error!
}
```

> 생성자의 호출순서는 정해져 있기에 <br>
> `mystream(int sz) : buf(sz), stream(buf) {}`<br>
> stream이 먼저 호출된다.<br>

> 해결책?

```cpp
// 버퍼만 관리하는 클래스 생성
struct buf_manager
{
protected:
    stream_buf buf;
public:
    buf_manager(size_t sz) : buf(sz) {}
};

struct mystream : public buf_manager, public stream
{
public:
    mystream(int sz) : buf_manager(sz), stream(buf) {}
};
```

---

## 생성자와 가상함수의 관계

```cpp
struct Base
{
    Base() {}
    void foo() { goo(); }
    virtual void goo() { cout << "Base::goo" << endl; }
};

struct Derived : public Base
{
    virtual void goo() { cout << "Derived::goo" << endl; }
};

int main()
{
    Derived d;
    d.foo();            // Derived::goo
}
```

```cpp
struct Base
{
    Base() { goo(); }
    virtual void goo() { cout << "Base::goo" << endl; }
};

struct Derived : public Base
{
    virtual void goo() { cout << "Derived::goo" << endl; }
};

int main()
{
    Derived d;          // Base::goo
    // 생성자에서는 가상함수가 동작하지 않는다.
}
```

> 왜? 생성자에서는 가상함수가 동작하지 않을까?

> 이러한 문제 때문이다.

```cpp
struct Base
{
    Base() { goo(); }
    virtual void goo() { cout << "Base::goo" << endl; }
};

struct Derived : public Base
{
    int x;

    Derived() : x(10) {}
    // x를 10으로 초기화 하는 동작보다 기반클래스 Base의 생성자를 호출하는 순서가 먼저이다.
    // 그런데 Base생성자에서 가상함수가 동작하게 된다면??
    // 제대로 초기화 되지 않은 x를 사용할 가능성이 생기게 된다.
    virtual void goo() { cout << "Derived::goo" << endl; }
};
```

---

## 생성자와 예외

```cpp
struct Resource
{
    Resource() { cout << "acquire Resource" << endl; }
    ~Resource() { cout << "release Resource" << endl; }
};

class Test
{
    Resource* p;
public:
    Test() : p( new Resource )
    {
        cout << "Test()" << endl;
        throw 1;            // 어떠한 문제로 예외를 던진다 가정해보자.
    }
    ~Test()
    {
        // 그런데 생성 중문제가 발생하면 메모리 해제는 어디서하나?? <- 문제
        delete p;
        cout << "~Test()" << endl;
    }
};

int main()
{
    try{
        Test t;
    }
    catch(...)
    {
        cout << "예외발생" << endl;
    }
}
```

> 해결책은??

```cpp
class Test
{
    // Resource* p;
    unique_ptr<Resource> p;
public:
    Test() : p( new Resource )
    {
        cout << "Test()" << endl;
        throw 1;
    }
    ~Test()
    {
        cout << "~Test()" << endl;
    }
};
```

> Test의 소멸자는 호출이 되지 않으나 Resource의 소멸자는 호출됨

> 또 다른 해결책(이단 생성자)

```cpp
class Test
{
    Resource* p;
    
public:
    Test() : p(0)
    {
        // 예외 가능성이 있는 어떠한 작업도 하지 않는다.
    }

    // 자원할당 전용함수를 만든다.
    void Construct()
    {
        cout << "Test()" << endl;
        p = new Resource;
        throw 1;
    }
    ~Test()
    {
        delete p;
        cout << "~Test()" << endl;
    }
};
```

> 단점은 Construct함수를 별도로 만들어야한다는 점이다.