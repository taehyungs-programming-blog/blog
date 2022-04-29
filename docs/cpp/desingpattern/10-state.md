---
layout: default
title: "10. State Pattern"
parent: (Desing Pattern)
grand_parent: C++
nav_order: 2
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

```cpp
#include <iostream>
using namesapce std;

class Character
{
    int gold = 0;
    int item = 0;
public:
    void run() { cout << "run" << endl; }
    void attact() { cout << "attack" << endl; }
};

int main()
{
    Character* p = new Chracter;
    p->run();
    p->attack();
}
```

🍒 현재 캐릭이 보유한 `item`에 따라 `run()`이나 `attact()`함수를 호출하고자 한다면?

```cpp
// 방법 1
// run, attact에 if문으로 구분을 준다.
void run() { 
    if(item == 1)
        cout << "run : 1" << endl; 
    else if(item == 2)
        cout << "run : 2" << endl;
}
```

* 문제
    * 모든 동작에 함수에 조건 분기문이 필요
    * 새로운 아이템이 추가되면 조건문 속에 조건문이 들어가야한다.

```cpp
// 방법 2
// 가상함수를 활용하자
class Character
{
    int gold = 0;
    int item = 0;
public:
    void run() { runImp(); }
    void attact() { attackImp(); }

    virtual void runImp() {}
    virtual void attackImp() {}
};

class PowerItemCharater : public Character
{
public:
    virtual void runImp() { cout << "run" << endl;}
}

int main()
{
    Character* p = new Chracter;
    p->run();
    p->attack();

    p = new PowerItemCharatcter;
    p->run();
    p->attack();
}
```

* 또 문제는 있다.
    * 위의 방식대로 선언하면 상태(gold정보 등)을 넘길수가 없다.

```cpp
// 방법 3
struct IState
{
    virtual void run() = 0;
    virtual void attack() = 0;
    virtual ~IState() {}
};

class Character
{
    int gold = 0;
    int item = 0;
    IState* state;
public:
    void changeState(IState* p) { state = p; }

    void run() { state->run(); }
    void attact() { state->attatck(); }
};

class NormalState : public IState
{
    virtual void run() { cout << "run" << endl; }
    virtual void attack() { cout << "power attack" << endl; }
};

int main()
{
    NormalState ns;

    Chracter* p = new Chracter;
    p->changeState(&ns);
    p->run();
}
```
