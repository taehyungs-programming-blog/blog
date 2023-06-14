---
title: "(C++) upcasting"
date: 2020-03-02 00:00:00 -0000
---

```cpp
class Animal
{
    int age;
};

class Dog : public Animal
{
    int color;
};

int main()
{
    Dog d;
    Dog* p1 = &d;           // ok
    double* p2 = &d;        // error
    Animal* p3 = &d;        // ok - upcasting
}
```

* 상속을 동족끼리 묶을때 사용한다는 개념으로 사용된다.

---

```cpp
class Animal
{
    int age;
public:
    void Cry() { cout << "Animal Cry" << endl; }
};

class Dog : public Animal
{
    int color;
public:
    // override
    void Cry() { cout << "Dog Cry" << endl; }
};

int main()
{
    Dog d;
    Animal* p = &d;

    p->Cry();   // "Animal Cry" -> 포인터를 보고 따라간다.
    // 단, 자바와 C#에서는 Dog를 부름
}
```

```cpp
// 만약 Dog Cry를 부르고 싶다면?
class Animal
{
    int age;
public:
    virtual void Cry() { cout << "Animal Cry" << endl; }
};

class Dog : public Animal
{
    int color;
public:
    // override
    virtual void Cry() override { cout << "Dog Cry" << endl; }
};
```