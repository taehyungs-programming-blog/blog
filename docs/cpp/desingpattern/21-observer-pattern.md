---
layout: default
title: "21. Observer Pattern"
parent: (Desing Pattern)
grand_parent: C++
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Observer Pattern

🦄 객체 사이의 1:N의 종속성을 정의 하고 한 객체의 상태가 변하면 종속된 다른 객체에 통보가 가고 자동으로 수정이 일어나게 한다.

```cpp
// Observer 패턴의 기본적 구조
#include <iostream>
#include <vector>
using namespace std;

struct IGraph
{
    virtual void update(int) = 0;
    virtual ~IGraph() {}
}

class Table
{
    vector<IGraph*> v;
    int data;
public:
    void attach(IGraph* p) { v.push_back(p); }
    void detach(IGraph* p) { }

    void SetData(int d) { 
        data = d;
        for( auto p : v )
            p->update(data);
    }
};

class PieGraph : public IGraph
{
public:
    virtual void update(int n)
    {
        Draw(n);
    }

    void Draw(int n)
    {
        cout << "Pie Graph : ";
        for(i = 0; i < n; i++)
            cout << "*";
    }
};

class BarGraph : public IGraph
{
public:
    virtual void update(int n)
    {
        Draw(n);
    }
    
    void Draw(int n)
    {
        cout << "Bar Graph : ";
        for(i = 0; i < n; i++)
            cout << "+";
        cout << endl;
    }
};

int main()
{
    BarGraph bg;
    PieGraph pg;

    Table t;
    t.attach(&bg);
    t.attach(&pg);

    while(1)
    {
        int n;
        cin >> n;
        t.SetData(n);
    }
}
```

---

## Example

```cpp
// 새로운 table을 만들고 싶다
class Table3D
{
    vector<IGraph*> v;
    int data[10];       // 다른데는 모두 동일하고 데이터만 다르다면??
public:
    void attach(IGraph* p) { v.push_back(p); }
    void detach(IGraph* p) { }

    void SetData(int d) { 
        data = d;
        for( auto p : v )
            p->update(data);
    }
};
```

```cpp
class Subject
{
    vector<IGraph*> v;
public:
    void attach(IGraph* p) { v.push_back(p); }
    void detach(IGraph* p) { }
    void notify(int data)
    {
        for( auto p : v )
            p->update(data);
    }
};

class Table3D : public Subject
{
    int data[10];
public:

    void SetData(int d) { 
        data = d;
        notify(data);
    }
};
```

```cpp
class Subject;

struct IGraph
{
    virtual void update(Subject*) = 0;
    virtual ~IGraph() {}
};

class Subject
{
    vector<IGraph*> v;
public:
    void attach(IGraph* p) { v.push_back(p); }
    void detach(IGraph* p) { }
    void notify()
    {
        for( auto p : v )
            p->update(this);
    }
};

class Table3D : public Subject
{
    int data[10];
public:
    int GetData() { return data; }

    void SetData(int d) { 
        data = d;
        notify();
    }
};

class PieGraph : public IGraph
{
public:
    virtual void update(Subject* p)
    {
        // table에 접근해서 data를 꺼내온다.
        int n = static_cast<Table3D*>(p)->GetData();
        Draw(n);
    }

    void Draw(int n)
    {
        cout << "Pie Graph : ";
        for(i = 0; i < n; i++)
            cout << "*";
    }
};
```