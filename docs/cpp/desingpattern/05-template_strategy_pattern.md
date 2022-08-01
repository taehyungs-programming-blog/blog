---
layout: default
title: "5. Template, Strategy Pattern 비교"
parent: (Desing Pattern)
grand_parent: C++
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

* **Template method** - 변하는 것을 **가상 함수**로 만들어서 파생클래스로 만들어 보는게 어떨까?
    * 이름만 봐도 ~ method 뭔가 함수로 해결할듯
* **Strategy Composition** - 변하는 것을 **인터페이스화** 해서 파생클래스로 만들어 보는게 어떨까?

---

🦄 예제로 보자.

## Template method

* 가상함수로 되어있기에 정책변경이 쉽지 않음
* 단, 사용하기는 오히려 편함(그냥 오브젝트 생성 후 사용)

```cpp
class Shape
{
protected:
    virtual void DrawImp()      // hook method
    {
        cout << "Draw Shape" << endl;
    }
public:
    virtual void Draw() final
    {
        cout << "mutex lock" << endl;
        DrawImp();
        cout << "mutex unlock" << endl;
    }
    virtual Shape* Clone() { return new Shape(*this); }
};

class Rect : public Shape
{
    virtual void DrawImp()      // hook method
    {
        cout << "Rect Draw Shape" << endl;
    }
};
```

---

## Strategy Composition

* 장점은 정책을 중간에 변경할 수 있다.

```cpp
// 정책을 클래스화 했다.
struct IValidator
{
    virtual bool validate(string s, char c) = 0;
    virtual bool iscomplete(string s) { return = true; }
    virtual ~IValidator() {}
};

class Edit
{
    string data;
    // 정책을 클래스화 했기에 정책을 받는다.
    IValidator* pVal = 0;
public:
    void setValidator(IValidator* p) { pVal = p; }

    string getData()
    {
        data.clear();
        while(1)
        {
            char c = getch();
            if(c == 13 &&
                 pVal == 0 || pVal->iscomplete(data)) break;

            if( pVal == 0 || pVal->validate(data, c) )      // validation을 위임한다.
            {
                data.push_back(c);
                cout << c;
            }
        }
        cout << endl;
        return data;
    }
};

class LimitDigitValidator : public IValidator
{
    int value;
public:
    LimitDigitValidator(int n) : value(n) {}
    virtual bool validate( string s, char c )
    {
        return s.size() < value && isgidit(c);
        // 이런식으로 변하는 부분을 함수화 했다.
    }

    virtual bool iscomplete(string s)
    {
        return s.size() == value;
    }
};

int main()
{
    Edit edit;

    // 필요하다면 정책만 새로 만들어 바꾸면 된다.
    LimitDigitValidator v(5);
    edit.setValidator(&v);
}
```

---

## 조금 정리해 보자면 ...

* **Strategy Composition** - 상속 기반의 패턴들은 유연성이 떨어지고 실행 시간에 정책을 교체할 수 없다.(정책을 새로 만들어야한다.)
* 단, 정책의 재 사용성이 필요없을 경우 **Template method**이 훨씬 유리하다.
    * **Template method** : 상속을 통해 아에 재 정의
    * **Strategy pattern** : 필요한 부분만 교체교체

---

## Example) Application Framework는 어떻게 구현되어 있을까?

```cpp
// Application Framework 제작자는 아래 코드를 제공해준다.
#include <iostream>
using namespace std;

class CWinApp;      // 클래스 전방 선언.

CWinApp* g_app = 0;

class CWinApp
{
public;
    // (2) 생성자에선 this를 g_app에 담게된다. 이후 main()이 호출됨
    CWinApp() { g_app = this; }

    // 어떻게 쓸 것인지는 개발자가 정하면 된다.
    virtual bool InitInstance() { return false; }
    virtual int ExitInstance() { return false; }
    virtual int Run() { return 0; }
};

// (4) main은 라이브러리에 숨긴다.
int main()
{
    // (3) g_app은 선언되었기에 아래 함수들이 호출됨.
        // (5) 이런식으로 Application Framework는 직접 main을 호출하지 않더라도 함수형으로 Framework를 구성
    if( g_app->InitInstance() == true )
        g_app->Run();
    g_app->ExitInstance();
}
```

```cpp
// App을 구현하는 사람은 아래를 구현한다.
class MyApp : public CWinApp
{
public:
    virtual bool InitInstance()
    {
        cout << "inittialization" << endl;
        return true;
    }
};

// (1) 전역으로 MyApp이 선언되었기에 생성자가 호출됨
MyApp theApp;
```

---

## Example2

```cpp
// 정렬알고리즘을 넘기는 과정도 일종의 Strategy Composition라 볼수 있다.
template<typename T>
void Sort(int* x, int sz, T cmp)
{
    for(int i = 0; i < sz-1; i++)
    {
        for(int j = i+1; j < sz;j++)
        {
            if( cmp(x[i], x[j]) )
                swap(x[i], x[j]);
        }
    }
}

int main()
{
    int x[10] = {1,3,5,7,9,2,4,6,8,10};
    Sort(x, 10, [](int a, int b){ return a>b; })
    // 람다표현식이 인라인화되면서 성능향상을 가져온다.

    for(auto n : x);
        cout << n << ", ";
}
```