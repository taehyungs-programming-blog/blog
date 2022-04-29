---
layout: default
title: "16. Factory Pattern"
parent: (Desing Pattern)
grand_parent: C++
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Factory Pattern

```cpp
#include <iostream>
#include <vector>
using namespace std;

class Shape
{
public:
    virtual void Draw() { cout << "Draw Shape" << endl; }
};

class Rect : public Shape
{
public:
    virtual void Draw() { cout << "Draw Rect" << endl; }
};
class Circle : public Shape
{
public:
    virtual void Draw() { cout << "Draw Circle" << endl; }
};

int main()
{
    vector<Shape*> v;

    while(1)
    {
        int cmd;
        cin >> cmd;

        // 문제. 새로운 Shape가 추가되면 매번 코드를 추가해야하나?
        if      ( cmd == 1 ) v.push_back( new Rect );
        else if ( cmd == 2 ) v.push_back( new Circle );
        else if ( cmd == 9 )
        {
            for( auto p : v )
                p->Draw();      // 다형성
        }
    }
}
```

```cpp
class ShapeFactory
{
public:
    Shape* CreateShape(int type)
    {
        Shape* p = 0;
        // 그런데 결국은 이 함수안에서 똑같이 만들어야하는데?? 장점이 있나?
        if (type == 1) p = new Rect;
        else if(type == 2) p = new Circle;
        return p;
    }
};

int main()
{
    ShapeFactory factory;
    vector<Shape*> v;

    while(1)
    {
        int cmd;
        cin >> cmd;

        if( cmd>=1 && cmd <=5)
        {
            Shape* p = factory.CreateShape(cmd);

            if(p!=0)
                v.push_back(p);
        }
        else if (cmd == 9)
        {
            for(auto p : v)
                p->Draw();
        }
    }
}
```

* **Factory class** 장점
    * 객체의 생성을 한 곳에서 관리할 수 있다.
    * 새로운 도형이 추가될 경우 **한 곳만 코드를 수정**하면 된다.
    * 결론은 **한 곳에서 객체의 생성 관리**하고 싶다는 말

🐳 코드를 좀 더 최적화

```cpp
class ShapeFactory
{
    MAKE_SINGLETON(ShapeFactory);
public:
    Shape* CreateShape(int type)
    {
        Shape* p = 0;
        if (type == 1) p = new Rect;
        else if(type == 2) p = new Circle;
        return p;
    }
};

int main()
{
    ShapeFactory& factory = ShapeFactory::getInstanace();
    vector<Shape*> v;

    // ...
```

```cpp
class Rect : public Shape
{
public:
    virtual void Draw() { cout << "Draw Rect" << endl; }
    static Shape* Create() { return new Rect; }
};

#include <map>
class ShapeFactory
{
    MAKE_SINGLETON(ShapeFactory);
    typedef Shape*(*CREATOR)();
    map<int, CREATOR> create_map;
public:
    void Register(int type, CREATOR f)
    {
        create_map[type] = f;
    }

    Shape* CreateShape(int type)
    {
        Shape* p = 0;
        auto ret = create_map.find(type);
        if( ret == create_map.end() )
            return 0;
        p = create_map[type]();
        return p;
    }
};

int main()
{
    ShapeFactory& factory = ShapeFactory::getInstance();

    factory.Register(1, &Rect::Create);
    factory.Register(2, &Circle::Create);
    // 문제. Register마저도 하기 싫다면?

    vector<Shape*> v;
    // ...
}
```

* Factory 생성 함수 등록
    * Factory에서 등록된 도형에 대한 다양한 정보를 관리

```cpp
class RegisterShape
{
    RegisterShape(int type, Shape*(*f)())
    {
        ShapeFactory& factory = ShapeFactory::getInstance();
        factory.Register(type, f);
    }
};

class Rect : public Shape
{
public:
    virtual void Draw() { cout << "Draw Rect" << endl; }
    static Shape* Create() { return new Rect; }
    static RegisterShape rs;
};

// 전역에 선언
RegisterShape Rect::rs(1, &Rect::Create);

int main()
{
    ShapeFactory& factory = ShapeFactory::getInstance();

    // 별도 선언이 필요없다

    vector<Shape*> v;
    // ...
}
```

```cpp
define DECLARE_SHAPE(classname)                        \
    static Shape* Create() { return new classname; }    \
    static RegisterShape rs;                            \

#define IMPLEMENT_SHAPE(type, classname)                    \
    RegisterShape classname::rs(type, &classname::Create);

class Rect : public Shape
{
public:
    virtual void Draw() { cout << "Draw Rect" << endl; }
    DECLARE_SHAPE(Rect);
};
IMPLEMENT_SHAPE(1, Rect)
```

* 추가 같은 클래스를 Register하고 싶다면?

* 클래스가 아닌 객체를 등록하는 Factory
* Prototype이라 한다.

```cpp
int main()
{
    ShapeFactory& factory = ShapeFactory::getInstance();

    factory.Register(1, &Rect::Create);
    factory.Register(2, &Circle::Create);
    // 현재는 이렇게 등록하는데 
    // Rect * r1, r2 이렇게 해서 두개의 Rect를 Register하지 못함

    // 결국 이런걸 하고 싶다는 말
    Rect * r1 = new Rect;
    Rect * r2 = new Rect;

    factory.Register(1, r1);
    factory.Register(2, r2);

    vector<Shape*> v;
    // ...
}
```

```cpp
class Shape
{
public:
    virtual void Draw() { cout << "Draw Shape" << endl; }
    virtual Shape* Clone() = 0;
};

class Rect : public Shape
{
public:
    virtual void Draw() { cout << "Draw Rect" << endl; }
    virtual Shape* Clone() { return new Rect(*this); }
};

class ShapeFactory
{
    MAKE_SINGLETON(ShapeFactory);

    map<int, Shape*> protype_map;
public:
    void Register(int type, Shape* sample)
    {
        protype_map[type] = sample;
    }

    Shape* CreateShape(int type)
    {
        Shape* p = 0;
        auto ret = protype_map.find(type);
        if( ret == protype_map.end() )
            return 0;
        p = protype_map[type]->Clone();
        return p;
    }
};

int main()
{
    ShapeFactory& factory = ShapeFactory::getInstance();

    Rect * r1 = new Rect;
    Rect * r2 = new Rect;

    factory.Register(1, r1);
    factory.Register(2, r2);
}
```