---
title: "(C++) example"
date: 2020-03-02 00:00:00 -0000
---

## Shape 1차

```cpp
#include <iostream>
#include <vector>
using namespace std;

class Shape
{
    virtual void Draw() = 0;
};

class Rect : public Sahpe
{
public:
    virtual void Draw() override { cout << "Draw Rect" << endl; }
};

class Circle : public Sahpe
{
public:
    virtual void Draw() override { cout << "Draw Circle" << endl; }
}

int main()
{
    vector<Rect*> v;
    vector<Circle*> v;
    // ??? -> 비효율적!
    
    vector<Shape*> v;       // ok

    while(1)
    {
        int cmd;
        cin >> cmd;

        if( cmd == 1) v.push_back(new Rect);
        else if(cmd == 2) v.push_back(new Circle);
        else if(cmd == 9)
        {
            for(auto p : v)
                p->Draw();
        }
    }
}
```

---

## Shape 2차

```cpp
#include <iostream>
#include <vector>
using namespace std;

class Shape
{
    virtual void Draw() = 0;
};

class Rect : public Sahpe
{
public:
    virtual void Draw() override { cout << "Draw Rect" << endl; }
};

class Circle : public Sahpe
{
public:
    virtual void Draw() override { cout << "Draw Circle" << endl; }
};

class Triangle : public Sahpe       // 다형성으로 인해 새로운 클래스가 추가되어도 그대로 동작함! -> OCP
{
public:
    virtual void Draw() override { cout << "Draw Circle" << endl; }
};

int main()
{
    vector<Rect*> v;
    vector<Circle*> v;
    
    vector<Shape*> v;

    while(1)
    {
        int cmd;
        cin >> cmd;

        if( cmd == 1) v.push_back(new Rect);
        else if(cmd == 2) v.push_back(new Circle);
        else if(cmd == 3) v.push_back(new Triangle);
        else if(cmd == 8)
        {
            cout << "index >>";
            int k;
            cin >> k;
            // k 번째 도형의 복사본을 v에 추가 -> 그런데 복사본이 Rect, Circle, Triangle인지 어떻게 알지?
            // 해결방법은 아래서
        }
        else if(cmd == 9)
        {
            for(auto p : v)
                p->Draw();      // 상황에 따라 다른 객체의 Draw를 호출하게 된다. -> 다형성(Polymorphism)이라 한다.
        }
    }
}
```

```cpp
class Shape
{
    virtual void Draw() = 0;

    // 자신의 복사본을 만드는 가상함수.
    virtual Shape* Clone() { return new Shape(*this); }
};

class Rect : public Sahpe
{
public:
    virtual void Draw() override { cout << "Draw Rect" << endl; }
    virtual Shape* Clone() override { return new Rect(*this); }
};
```

```cpp
int main()
{
    vector<Rect*> v;
    vector<Circle*> v;
    
    vector<Shape*> v;

    while(1)
    {
        int cmd;
        cin >> cmd;

        if( cmd == 1) v.push_back(new Rect);
        else if(cmd == 2) v.push_back(new Circle);
        else if(cmd == 3) v.push_back(new Triangle);
        else if(cmd == 8)
        {
            cout << "index >>";
            int k;
            cin >> k;
            
            v.push_back(v[k]->Clone());     // ok
        }
        else if(cmd == 9)
        {
            for(auto p : v)
                p->Draw();
        }
    }
}
```

---

## Shape 3차 - final 사용

```cpp
class Shape
{
    virtual void Draw()
    {
        cout << "mutex lock" << endl;       // 매번 mutex lock / unlock을 반복해야하나?
        cout << "Draw Shape" << endl;
        cout << "mutex unlock" << endl;
    }

    virtual Shape* Clone() { return new Shape(*this); }
};
```

* 변하지 않는 코드에서 변하는 부분을 분리하자.

```cpp
class Shape
{
protected:      // Draw를 직접 호출하지 못하게 한다.
    virtual void DrawImp()
    {
        cout << "Draw Shape" << endl;
    }

public:
    virtual void Draw() final       // 재정의를 막는다.
    {
        cout << "mutex lock" << endl;
        DrawImp();
        cout << "mutex unlock" << endl;
    }
};

class Rect : public Sahpe
{
public:
    virtual void DrawImp() override { cout << "Draw Rect" << endl; }
    virtual Shape* Clone() override { return new Rect(*this); }
};
```
