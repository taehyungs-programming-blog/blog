---
layout: default
title: "24. Prototype Pattern"
parent: (Desing Pattern)
grand_parent: C++
nav_order: 3
---

## Table of contents
{: .no_toc .text-delta }

1. TOC
{:toc}

---

## Prototype

* [참고 사이트 🌍](https://devjino.tistory.com/23)

* 이미 만들어진 객체(프로토타입, 원형)을 복사하여 새로운 객체를 생성하는 패턴입니다.

---

## 하고싶은 것은?

```cpp
struct Address
{
    string street;
    string city;
    int suite;
};

struct Contact
{
    string name;
    Address address;  // 문제 없음
    // Address* address;  // 중복 처리 문제 발생
};
int main()
{
    Contact worker{"", Address{ "123 East Dr", "London", 0 }};

    // 이런 복사를 좀 더 안전한 구조로 하고 싶다.
    Contact john = worker;

    john.name = "John Doe";
    john.address.suite = 10;

    return 0;
}
```

---

## 해결책 1차

```cpp
struct Contact
{
    string name;
    Address* address;

    // (일반 생성자) Address의 항목이 추가 또는 삭제 되었을때 곤란해 진다.
    Contact(string name, Address* address) : name{ name }, address{ address } { }

    // 복사 생성자로 해결해 보자
    /*
    Contact(const Contact& other) : name{ other.name }
    {
        address = new Address{ other.address->street, other.address->city, other.address->suite };
    }

    // 2. Address 항목이 수정되어도 신경 쓸 것이 없다.
    Contact(const Contact& other) : name{ other.name }
    {
        address = new Address{ *other.address };
    }

    // 3. 이것 또한 마찬가지다.
    Contact(const Contact& other) : name{ other.name }, address{ new Address{*other.address} }
    {
    }
    */
};
```

* 코드 가독성까지 원한다면 ???

```cpp
template<typename T> struct Cloneable
{
    virtual T clone() const = 0;
};
struct Contact : public Cloneable<Contact>
{
    string name;
    Address* address;

    Contact clone() const
    {
        return Contact{ name, new Address{ *address } };
    }
};
Contact worker { "sss", new Address{ "123 East Dr", "London", 0 }};
Contact john = worker.clone();
```