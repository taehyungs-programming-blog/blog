---
layout: default
title: "4. 디자인패턴! 어디다 써야할까?"
parent: (Desing Pattern)
grand_parent: C++
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

🦄 예로서 설명해 보자면 ...

```cpp
#include <iostream>
#include <string>
using namespace std;

class Edit
{
    string data;
public:
    string getData()
    {
        // Edit이라는 클래스를 기반으로 콘솔에서 문자를 받고 출력하고자 한다.
        cin >> data;
        return data;
    }
};

int main()
{
    Edit edit;

    while(1)
    {
        string s = edit.getData();      
        // 만약 기획이 바뀌어 숫자만 받아야 한다면??
        cout << s << endl;
    }
}
```

```cpp
#include <conio.h>      // getch()

class Edit
{
    string data;
public:
    string getData()
    {
        data.clear();
        while(1)
        {
            char c = getch();
            if(c == 13) break;      

            // 이런식으로 문자입력을 막을 수 있다.
            if(isdigit(c))
            {
                data.push_back(c);
                cout << c;
            }
        }
        cout << endl;
        return data;
    }
};

// 기획이 또 바뀌었다.
// 다시 string으로 바꿔 보자
```

🦄 매번 `Edit` 클래스를 만져야할까?<br>
🦄 `Edit`가 매우 코어한 클래스라면?? 이게 과연 좋은 방법일까?

```cpp
class Edit
{
    string data;
public:
    virtual bool validate(char c)
    {
        return isdigit(c);
    }

    string getData()
    {
        data.clear();
        while(1)
        {
            char c = getch();
            if(c == 13) break;

            if(validate(c))
            {
                data.push_back(c);
                cout << c;
            }
        }
        cout << endl;
        return data;
    }
};

class AddressEdit : public Edit
{
    public:
    virtual bool validate(char c) override
    {
        return true;        
        // validate 정책을 항상 true로 둔다. -> 숫자, 문자 모두 받는다.
        // 핵심은 정책을 클래스 외부로 빼는것에 있다.
    }
};

int main()
{
    AddressEdit edit;

    while(1)
    {
        string s = edit.getData();
        cout << s << endl;
    }
}
```

---

## 그래서 디자인 패턴이 왜 필요한데??

* 조금 더 고민해보면 두 가지 방법이 있는데,

* 변하는 것을 **가상 함수**로 만들어서 파생클래스로 만들어 보는게 어떨까? -> **Template method**
* 변하는 것을 **인터페이스화** 해서 파생클래스로 만들어 보는게 어떨까? -> **Strategy Composition**
* 더 자세한 설명은 **Template, Strategy Pattern**에서 설명

