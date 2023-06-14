---
title: "C++ Template : CRTP"
permalink: cpp/template/CRTP/                # link 직접 지정
toc: true                       # for Sub-title (On this page)
comments: true                  # for disqus Comments
categories:                     # for categories
date: 2020-04-30 00:00:00 -0000
last_modified_at: 2020-04-30 00:00:00 -0000
sidebar:
  title: "C++"
  nav: cpp
---

* Curiously Recurring Template Pattern(신기하게도 언급되는 템플릿 패턴)

```cpp
class Base
{
    // Base에서는 Derived의 이름을 알 수 없다.
    // 알게 하고 싶다면?? -> CRTP
};

class Derived : public Base
{

};

int main()
{
    Derived d;
}
```

```cpp
template<typename T> class Base
{
public:
    Base()
    {
        cout << typeid(T).name() << endl;
    }
};

class Derived : public Base<Derived>        // 파생클래스를 만들때 이름을 전달해준다.
{

};

int main()
{
    Derived d;
}
```

어디쓰나?

```cpp
#include <iostream>
using namespace std;

class Window
{
public:
    void msgLoop()
    {
        OnClick();
    }
    void OnClick() { cout << "Window OnClick" << endl; }
};

class FrameWindow : public Window
{
public:
    void OnClick() { cout << "FrameWindow Window OnClick" << endl; }
};

int main()
{
    FrameWindow fw;
    fw.msgLoop();   // Window OnClick가 호출된다
    // 원하는 것은 FrameWindow Window OnClick
    // 물론 virtual선언하면 되지만 ... 오버헤드가 커진다.
}
```

```cpp
template<typename T> class Window
{
public:
    void msgLoop()
    {
        //OnClick();
        static_cast<T*>(this)->OnClick();
    }
    void OnClick() { cout << "Window OnClick" << endl; }
};

class FrameWindow : public Window<FrameWindow>
{
public:
    void OnClick() { cout << "FrameWindow Window OnClick" << endl; }
};
```

CRTP 활용

```cpp
// CRTP를 이용해서 singleton 만들어보자.
class Cursor
{
private:
    Cursor() {}
public:
    Cursor(const Cursor& c) = delete;
    void operator=(const Cursor& c) = delete;

    static Cursor& getInstance()
    {
        static Cursor instance;
        return instance;
    }
};

int main()
{
    // Cursor c1, c2;
    Cursor& c1 = Cursor::getInstance();
    Cursor& c2 = Cursor::getInstance();
    // 같음
}
```

```cpp
class Mouse
{
    // Mouse도 Singleton화 하고 싶다면?
};
```

```cpp
class Mouse : public Singleton  // 이렇게?
{
};
```

```cpp
template<typename T> class Singleton
{
protected:
    Singleton() {}
public:
    Singleton(const Singleton& c) = delete;
    void operator=(const Singleton& c) = delete;

    static T& getInstance()
    {
        static T instance;
        return instance;
    }
};

class Mouse : public Singleton<Mouse>
{
};

int main()
{
    Mouse& m = Mouse::getInstance();
}
```

또 다른 예제

```cpp
#include <iostream>
using namespace std;

class Object
{
public:
    static int cnt;

    Object() { ++cnt; }
    ~Object() { --cnt; }

    static int getCount() { return cnt; }
};
int Object::cnt = 0;

class Mouse
{
    // Mouse에서도 객체의 개수를 관리하고 싶다면??
};

int main()
{
    Object c1, c2;
    cout << c1.getCount() << endl;
}
```

```cpp
class Mouse : Object
{
};

class Keyboard : Object
{
};

int main()
{
    Mouse m1, m2;
    Keyboard k1, k2;
    cout << m1.getCount(); << endl; // 4 -> 잉??
}
```

```cpp
template<typename T> class Object
{
public:
    static int cnt;

    Object() { ++cnt; }
    ~Object() { --cnt; }

    static int getCount() { return cnt; }
};
template<typename T> int Object::cnt = 0;

class Mouse : Object<Mouse>
{
};

class Keyboard : Object<Keyboard>
{
};

int main()
{
    Mouse m1, m2;
    Keyboard k1, k2;
    cout << m1.getCount(); << endl; // 2
}
```