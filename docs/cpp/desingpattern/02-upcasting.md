---
layout: default
title: "2. (기초) Upcasting"
parent: (Desing Pattern)
grand_parent: C++
nav_order: 1
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Upcasting이란

🦄 부모로 캐스팅한다고 생각하면 된다.

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

---

## 주의할 점은?

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
    Animal* p = &d; // 업캐스팅 후

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